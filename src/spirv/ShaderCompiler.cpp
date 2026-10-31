// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "ShaderCompiler.hpp"

#include <algorithm>
#include <initializer_list>
#include <regex>
#include <shaderc/shaderc.hpp>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace jngl {
namespace {

struct ParsedUniform {
	std::string precision; // e.g. "HIGHP " or ""
	std::string type; // vec2, bool, sampler2D, …
	std::string name;
	uint32_t arrayLength = 0; // 0 if it isn't an array
	bool opaque = false;
};

uint32_t alignUp(const uint32_t offset, const uint32_t alignment) {
	return (offset + alignment - 1u) & ~(alignment - 1u);
}

/// std140 base alignment and size of \a type. `bool` is 32-bit in std140, matching setUniform(int)
/// / glUniform1i for bool uniforms.
std::pair<uint32_t, uint32_t> std140AlignAndSize(std::string_view type) {
	if (type == "bool" || type == "int" || type == "uint" || type == "float") {
		return { 4, 4 };
	}
	if (type == "vec2" || type == "ivec2" || type == "uvec2") {
		return { 8, 8 };
	}
	if (type == "vec3" || type == "ivec3" || type == "uvec3") {
		return { 16, 12 }; // a scalar after it takes the remaining 4 bytes
	}
	if (type == "vec4" || type == "ivec4" || type == "uvec4") {
		return { 16, 16 };
	}
	// Matrices are arrays of their columns, each of which starts at a multiple of 16 bytes
	if (type == "mat2") {
		return { 16, 32 };
	}
	if (type == "mat3") {
		return { 16, 48 };
	}
	if (type == "mat4") {
		return { 16, 64 };
	}
	throw std::runtime_error("Unsupported uniform type for the uniform block: " +
	                         std::string(type));
}

std::string uboMemberType(std::string_view type) {
	return std::string(type);
}

/// After the leading #version / #define / #extension lines so macros like HIGHP are visible, and
/// precision statements. Only the statements: a minified shader has the rest of its code on the
/// same line.
std::size_t insertPositionAfterPreprocessor(const std::string& source) {
	const auto isSpace = [](const char c) {
		return c == ' ' || c == '\t' || c == '\r' || c == '\n';
	};
	std::size_t pos = 0;
	while (pos < source.size()) {
		if (isSpace(source[pos])) {
			++pos;
		} else if (source[pos] == '#') { // a directive takes the rest of the line
			const std::size_t lineEnd = source.find('\n', pos);
			pos = lineEnd == std::string::npos ? source.size() : lineEnd + 1;
		} else if (const std::string_view keyword = "precision";
		           source.compare(pos, keyword.size(), keyword) == 0 &&
		           pos + keyword.size() < source.size() && isSpace(source[pos + keyword.size()])) {
			const std::size_t semicolon = source.find(';', pos);
			if (semicolon == std::string::npos) {
				break;
			}
			pos = semicolon + 1;
		} else {
			break;
		}
	}
	return pos;
}

/// A group of one or more declarators, each a name and maybe an array length, e.g. "a, b[2]"
/// (minified shaders declare variables of the same type like this)
constexpr std::string_view declaratorsPattern =
    R"((\w+\s*(?:\[\s*\w+\s*\])?(?:\s*,\s*\w+\s*(?:\[\s*\w+\s*\])?)*))";

struct Declarator {
	std::string name;
	uint32_t arrayLength = 0; // 0 if it isn't an array
};

/// The names and array lengths in \a declarators, see declaratorsPattern
std::vector<Declarator> parseDeclarators(const std::string& declarators) {
	static const std::regex declarator(R"((\w+)\s*(?:\[\s*(\w+)\s*\])?)");
	std::vector<Declarator> parsed;
	for (std::sregex_iterator d(declarators.begin(), declarators.end(), declarator), end; d != end;
	     ++d) {
		Declarator& result = parsed.emplace_back(Declarator{ .name = (*d)[1].str() });
		if ((*d)[2].matched) {
			const std::string length = (*d)[2].str();
			if (!std::ranges::all_of(length, [](const char c) { return c >= '0' && c <= '9'; }) ||
			    std::stoul(length) == 0) {
				throw std::runtime_error("The length of the array " + result.name +
				                         " has to be a positive number, not " + length + ".");
			}
			result.arrayLength = static_cast<uint32_t>(std::stoul(length));
		}
	}
	return parsed;
}

/// A shader's OpenGL-style uniform declarations and the rest of it
struct ParsedSource {
	/// Without the declarations, and with "#version 450" instead of "#version 300 es"
	std::string withoutDecls;
	std::vector<ParsedUniform> uniforms;
};

ParsedSource parseUniforms(const char* const source) {
	static const std::regex re(
	    std::string(R"(\buniform\s+((?:(?:lowp|mediump|highp|HIGHP)\s+)*)(\w+)\s+)")
	        .append(declaratorsPattern)
	        .append(R"(\s*;)"));

	// JNGL's shaders (and user shaders written for the OpenGL backend) use "#version 300 es", which
	// glslang rejects for SPIR-V (it requires ES 310+). Swap it for a desktop version that compiles
	// to Vulkan SPIR-V; the "es" precision qualifiers (lowp/mediump/highp) are still accepted.
	std::string adjusted = source;
	if (const auto pos = adjusted.find("#version 300 es"); pos != std::string::npos) {
		adjusted.replace(pos, sizeof("#version 300 es") - 1, "#version 450    ");
	}

	ParsedSource parsed;
	parsed.withoutDecls.reserve(adjusted.size());
	std::size_t last = 0;
	for (std::sregex_iterator it(adjusted.begin(), adjusted.end(), re), end; it != end; ++it) {
		const std::smatch& m = *it;
		for (Declarator& declarator : parseDeclarators(m[3].str())) {
			ParsedUniform u;
			u.precision = m[1].str();
			u.type = m[2].str();
			u.name = std::move(declarator.name);
			u.arrayLength = declarator.arrayLength;
			u.opaque = u.type.find("sampler") == 0;
			parsed.uniforms.push_back(std::move(u));
		}
		parsed.withoutDecls.append(adjusted, last, static_cast<std::size_t>(m.position()) - last);
		last = static_cast<std::size_t>(m.position() + m.length());
	}
	parsed.withoutDecls.append(adjusted, last, std::string::npos);
	return parsed;
}

/// The uniform block that all stages of a program share
struct UniformLayout {
	std::vector<ShaderUniform> uniforms;
	uint32_t blockSize = 0;
	bool pushConstants = false;
	/// Its members, one declaration per line
	std::string members;
};

/// Lays out the non-opaque uniforms of \a stages in the order they're declared, the first stage's
/// first. One that several stages declare is the same.
UniformLayout layoutUniforms(const std::initializer_list<const ParsedSource*> stages) {
	std::vector<const ParsedUniform*> merged;
	for (const ParsedSource* const stage : stages) {
		for (const ParsedUniform& u : stage->uniforms) {
			if (u.opaque) {
				continue;
			}
			const auto existing = std::ranges::find_if(
			    merged, [&u](const ParsedUniform* const other) { return other->name == u.name; });
			if (existing == merged.end()) {
				merged.push_back(&u);
			} else if ((*existing)->type != u.type || (*existing)->arrayLength != u.arrayLength) {
				throw std::runtime_error("The uniform " + u.name +
				                         " is declared differently in the shaders.");
			}
		}
	}

	UniformLayout layout;
	uint32_t offset = 0;
	for (const ParsedUniform* const u : merged) {
		const auto [alignment, size] = std140AlignAndSize(u->type);
		layout.members += '\t';
		layout.members += uboMemberType(u->type);
		layout.members += ' ';
		layout.members += u->name;
		if (u->arrayLength == 0) {
			offset = alignUp(offset, alignment);
			layout.uniforms.push_back({ u->name, offset, size });
			offset += size;
		} else {
			layout.members += '[' + std::to_string(u->arrayLength) + ']';
			// std140 starts the array and each of its elements at a multiple of 16 bytes
			const uint32_t stride = alignUp(size, 16);
			offset = alignUp(offset, 16);
			// Like on OpenGL, the name alone is the first element, and "name[i]" the others
			layout.uniforms.push_back({ u->name, offset, size });
			for (uint32_t i = 1; i < u->arrayLength; ++i) {
				layout.uniforms.push_back(
				    { u->name + '[' + std::to_string(i) + ']', offset + i * stride, size });
			}
			offset += u->arrayLength * stride;
		}
		layout.members += ";\n";
	}
	if (!layout.uniforms.empty()) {
		layout.blockSize = alignUp(offset, 16);
	}
	layout.pushConstants = layout.blockSize > 0 && layout.blockSize <= maxPushConstantsSize;
	return layout;
}

/// \a stage's source with its OpenGL-style uniforms in the block of \a layout, which all stages
/// declare the same, and its samplers at set 0, binding 0
std::string wrapUniforms(const ParsedSource& stage, const UniformLayout& layout) {
	std::string source = stage.withoutDecls;
	// Qualify non-opaque names as jnglUniforms.name now that their bare declarations are gone, but
	// not those of members, e.g. light.color. Not "u." or the like, which minified shaders might
	// have as a local variable.
	for (const ParsedUniform& u : stage.uniforms) {
		if (u.opaque) {
			continue;
		}
		const std::regex nameRe("(^|[^.\\w])(" + u.name + ")\\b");
		source = std::regex_replace(source, nameRe, "$1jnglUniforms.$2");
	}

	std::string header;
	for (const ParsedUniform& u : stage.uniforms) {
		if (!u.opaque) {
			continue;
		}
		header += "layout(set = 0, binding = 0) uniform ";
		header += u.precision;
		header += u.type;
		header += ' ';
		header += u.name;
		if (u.arrayLength > 0) {
			header += '[' + std::to_string(u.arrayLength) + ']';
		}
		header += ";\n";
	}
	if (!layout.uniforms.empty()) {
		// std140 for push constants, too, so that the offsets are the same either way
		header += layout.pushConstants
		              ? "layout(push_constant, std140) uniform JnglUniforms {\n"
		              : "layout(std140, set = 1, binding = 0) uniform JnglUniforms {\n";
		header += layout.members;
		header += "} jnglUniforms;\n";
	}

	const std::size_t insertAt = insertPositionAfterPreprocessor(source);
	source.insert(insertAt, header);
	return source;
}

/// Where the vertex shader's outputs are. OpenGL matches the fragment shader's inputs to them by
/// name, Vulkan and Direct3D 12 by location.
struct Varyings {
	std::vector<std::pair<std::string, uint32_t>> locations;
	uint32_t end = 0; ///< the first location after them
};

/// How many locations an input or output of \a type takes
uint32_t locationCount(const std::string& type, const std::string& name) {
	static const std::regex vector(R"((?:float|int|uint|[iu]?vec[234]))");
	static const std::regex matrix(R"(mat([234])(?:x[234])?)");
	if (std::regex_match(type, vector)) {
		return 1;
	}
	if (std::smatch m; std::regex_match(type, m, matrix)) {
		return static_cast<uint32_t>(m[1].str()[0] - '0'); // one for each column
	}
	throw std::runtime_error("The type " + type + " of the shader input/output " + name +
	                         " isn't supported.");
}

/// \a source with explicit locations for its inputs and outputs between the stages, where it
/// doesn't have them already. A vertex shader's inputs are where JNGL puts the vertices, and its
/// outputs are put into \a varyings. A fragment shader's inputs get the same locations as those
/// of the same name there.
std::string assignLocations(const std::string& source, const Shader::Type stage,
                            Varyings& varyings) {
	// Only declarations, which begin a statement, not parameters like "in vec2 p" of a function.
	// Without the semicolon, which might be where the next one begins.
	static const std::regex re(
	    std::string(R"((^|[;{}\n])(\s*)((?:(?:invariant|flat|smooth|noperspective|centroid)\s+)*))")
	        .append(R"((in|out)\s+((?:(?:lowp|mediump|highp|HIGHP)\s+)*)(\w+)\s+)")
	        .append(declaratorsPattern)
	        .append(R"((?=\s*;))"));
	const bool vertex = stage == Shader::Type::VERTEX;
	std::string result;
	std::size_t last = 0;
	for (std::sregex_iterator it(source.begin(), source.end(), re), end; it != end; ++it) {
		const std::smatch& m = *it;
		const bool input = m[4] == "in";
		if (!vertex && !input) {
			continue; // the fragment shader's color
		}
		result.append(source, last, static_cast<std::size_t>(m.position()) - last);
		last = static_cast<std::size_t>(m.position() + m.length());
		result += m[1].str() + m[2].str();
		const std::string type = m[6].str();
		const std::vector<Declarator> declarators = parseDeclarators(m[7].str());
		for (const Declarator& declarator : declarators) {
			if (&declarator != &declarators.front()) {
				result += "; "; // one declaration each, the source has the last semicolon
			}
			const uint32_t count =
			    locationCount(type, declarator.name) * std::max(declarator.arrayLength, 1u);
			uint32_t location = 0;
			if (vertex && input) {
				// See Sprite::vertexShader(), OpenGL binds them to these, too
				if (declarator.name == "position") {
					location = 0;
				} else if (declarator.name == "inTexCoord") {
					location = 1;
				} else {
					throw std::runtime_error("The vertex shader input " + declarator.name +
					                         " isn't supported, only position and inTexCoord.");
				}
			} else if (vertex) {
				location = varyings.end;
				varyings.locations.emplace_back(declarator.name, location);
				varyings.end += count;
			} else if (const auto output = std::ranges::find(
			               varyings.locations, declarator.name,
			               &std::pair<std::string, uint32_t>::first);
			           output != varyings.locations.end()) {
				location = output->second;
			} else { // not written by the vertex shader
				location = varyings.end;
				varyings.end += count;
			}
			result += "layout(location = " + std::to_string(location) + ") " + m[3].str() +
			          m[4].str() + ' ' + m[5].str() + type + ' ' + declarator.name;
			if (declarator.arrayLength > 0) {
				result += '[' + std::to_string(declarator.arrayLength) + ']';
			}
		}
	}
	result.append(source, last, std::string::npos);
	return result;
}

std::vector<uint32_t> compileToSpirv(const std::string& source, const Shader::Type stage,
                                     const char* const name) {
	const shaderc_shader_kind kind =
	    stage == Shader::Type::VERTEX ? shaderc_glsl_vertex_shader : shaderc_glsl_fragment_shader;
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetOptimizationLevel(shaderc_optimization_level_performance);
	options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
	// For inputs and outputs that assignLocations doesn't handle, e.g. interface blocks. Samplers
	// and the UBO already have explicit set/binding from wrapUniforms.
	options.SetAutoMapLocations(true);
	options.SetVulkanRulesRelaxed(true);

	const shaderc::SpvCompilationResult result =
	    compiler.CompileGlslToSpv(source, kind, name, options);
	if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
		throw std::runtime_error("Failed to compile " + std::string(name) +
		                         " to SPIR-V: " + result.GetErrorMessage());
	}
	return { result.cbegin(), result.cend() };
}

} // namespace

void checkGlsl(const char* const source, const Shader::Type stage, const char* const name) {
	const ParsedSource parsed = parseUniforms(source);
	Varyings varyings;
	compileToSpirv(
	    assignLocations(wrapUniforms(parsed, layoutUniforms({ &parsed })), stage, varyings), stage,
	    name);
}

CompiledProgram compileGlslProgram(const char* const vertexSource,
                                   const char* const fragmentSource) {
	const ParsedSource vertex = parseUniforms(vertexSource);
	const ParsedSource fragment = parseUniforms(fragmentSource);
	UniformLayout layout = layoutUniforms({ &vertex, &fragment });
	CompiledProgram compiled;
	Varyings varyings;
	compiled.vertexSpirv = compileToSpirv(
	    assignLocations(wrapUniforms(vertex, layout), Shader::Type::VERTEX, varyings),
	    Shader::Type::VERTEX, "vertex");
	compiled.fragmentSpirv = compileToSpirv(
	    assignLocations(wrapUniforms(fragment, layout), Shader::Type::FRAGMENT, varyings),
	    Shader::Type::FRAGMENT, "fragment");
	compiled.uniforms = std::move(layout.uniforms);
	compiled.uniformBlockSize = layout.blockSize;
	compiled.pushConstants = layout.pushConstants;
	return compiled;
}

} // namespace jngl
