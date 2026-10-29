// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "ShaderCompiler.hpp"

#include <algorithm>
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

/// std140 base alignment and the number of bytes setUniform writes for \a type.
/// `bool` is 32-bit in std140, matching setUniform(int) / glUniform1i for bool uniforms.
std::pair<uint32_t, uint32_t> std140AlignAndSize(std::string_view type) {
	if (type == "bool" || type == "int" || type == "uint" || type == "float") {
		return { 4, 4 };
	}
	if (type == "vec2" || type == "ivec2" || type == "uvec2") {
		return { 8, 8 };
	}
	if (type == "vec3" || type == "ivec3" || type == "uvec3") {
		return { 16, 12 }; // 16-byte slot; Rgb writes 12 bytes
	}
	if (type == "vec4" || type == "ivec4" || type == "uvec4" || type == "mat2") {
		return { 16, 16 };
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

/// After the leading #version / #define / #extension lines so macros like HIGHP are visible.
std::size_t insertPositionAfterPreprocessor(const std::string& source) {
	std::size_t pos = 0;
	while (pos < source.size()) {
		const std::size_t lineEnd = source.find('\n', pos);
		const std::size_t end = lineEnd == std::string::npos ? source.size() : lineEnd + 1;
		std::string_view line(source.data() + pos, end - pos);
		while (!line.empty() && (line.front() == ' ' || line.front() == '\t')) {
			line.remove_prefix(1);
		}
		if (line.empty() || line.front() == '#' || line.starts_with("precision")) {
			pos = end;
			continue;
		}
		break;
	}
	return pos;
}

/// Wraps OpenGL-style bare uniforms into `layout(set=1,binding=0) uniform JnglUniforms { … } u;`
/// and gives samplers an explicit `layout(set=0,binding=0)`. Fills \a outUniforms / \a
/// outBlockSize.
std::string wrapUniformsInUbo(std::string source, std::vector<ShaderUniform>& outUniforms,
                              uint32_t& outBlockSize) {
	static const std::regex re(
	    R"(\buniform\s+((?:(?:lowp|mediump|highp|HIGHP)\s+)*)(\w+)\s+(\w+)\s*(?:\[\s*(\w+)\s*\])?\s*;)");

	std::vector<ParsedUniform> parsed;
	std::string withoutDecls;
	withoutDecls.reserve(source.size());
	std::size_t last = 0;
	for (std::sregex_iterator it(source.begin(), source.end(), re), end; it != end; ++it) {
		const std::smatch& m = *it;
		ParsedUniform u;
		u.precision = m[1].str();
		u.type = m[2].str();
		u.name = m[3].str();
		if (m[4].matched) {
			const std::string length = m[4].str();
			if (!std::ranges::all_of(length, [](const char c) { return c >= '0' && c <= '9'; }) ||
			    std::stoul(length) == 0) {
				throw std::runtime_error("The length of the uniform array " + u.name +
				                         " has to be a positive number, not " + length + ".");
			}
			u.arrayLength = static_cast<uint32_t>(std::stoul(length));
		}
		u.opaque = u.type.find("sampler") == 0;
		parsed.push_back(std::move(u));
		withoutDecls.append(source, last, static_cast<std::size_t>(m.position()) - last);
		last = static_cast<std::size_t>(m.position() + m.length());
	}
	withoutDecls.append(source, last, std::string::npos);

	outUniforms.clear();
	outBlockSize = 0;
	std::string blockMembers;
	uint32_t offset = 0;
	for (const ParsedUniform& u : parsed) {
		if (u.opaque) {
			continue;
		}
		const auto [alignment, size] = std140AlignAndSize(u.type);
		const uint32_t occupied = alignment == 16 && size == 12 ? 16u : size; // vec3: 16 bytes
		blockMembers += '\t';
		blockMembers += uboMemberType(u.type);
		blockMembers += ' ';
		blockMembers += u.name;
		if (u.arrayLength == 0) {
			offset = alignUp(offset, alignment);
			outUniforms.push_back({ u.name, offset, size });
			offset += occupied;
		} else {
			blockMembers += '[' + std::to_string(u.arrayLength) + ']';
			// std140 starts the array and each of its elements at a multiple of 16 bytes
			const uint32_t stride = alignUp(occupied, 16);
			offset = alignUp(offset, 16);
			// Like on OpenGL, the name alone is the first element, and "name[i]" the others
			outUniforms.push_back({ u.name, offset, size });
			for (uint32_t i = 1; i < u.arrayLength; ++i) {
				outUniforms.push_back(
				    { u.name + '[' + std::to_string(i) + ']', offset + i * stride, size });
			}
			offset += u.arrayLength * stride;
		}
		blockMembers += ";\n";
	}
	if (!outUniforms.empty()) {
		outBlockSize = alignUp(offset, 16);
	}

	// Qualify non-opaque names as u.name now that their bare declarations are gone.
	for (const ParsedUniform& u : parsed) {
		if (u.opaque) {
			continue;
		}
		const std::regex nameRe("\\b" + u.name + "\\b");
		withoutDecls = std::regex_replace(withoutDecls, nameRe, "u." + u.name);
	}

	std::string header;
	for (const ParsedUniform& u : parsed) {
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
	if (!outUniforms.empty()) {
		header += "layout(std140, set = 1, binding = 0) uniform JnglUniforms {\n";
		header += blockMembers;
		header += "} u;\n";
	}

	const std::size_t insertAt = insertPositionAfterPreprocessor(withoutDecls);
	withoutDecls.insert(insertAt, header);
	return withoutDecls;
}

} // namespace

CompiledShader compileGlslToSpirv(const char* const source, const Shader::Type stage,
                                  const char* const name) {
	const shaderc_shader_kind kind =
	    stage == Shader::Type::VERTEX ? shaderc_glsl_vertex_shader : shaderc_glsl_fragment_shader;

	// JNGL's shaders (and user shaders written for the OpenGL backend) use "#version 300 es", which
	// glslang rejects for SPIR-V (it requires ES 310+). Swap it for a desktop version that compiles
	// to Vulkan SPIR-V; the "es" precision qualifiers (lowp/mediump/highp) are still accepted.
	std::string adjusted = source;
	if (const auto pos = adjusted.find("#version 300 es"); pos != std::string::npos) {
		adjusted.replace(pos, sizeof("#version 300 es") - 1, "#version 450    ");
	}

	CompiledShader compiled;
	adjusted = wrapUniformsInUbo(std::move(adjusted), compiled.uniforms, compiled.uniformBlockSize);

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetOptimizationLevel(shaderc_optimization_level_performance);
	options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
	// in/out without explicit locations (OpenGL habit) still need auto-assigned locations. Samplers
	// and the UBO already have explicit set/binding from wrapUniformsInUbo.
	options.SetAutoMapLocations(true);
	options.SetVulkanRulesRelaxed(true);

	const shaderc::SpvCompilationResult result =
	    compiler.CompileGlslToSpv(adjusted, kind, name, options);
	if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
		throw std::runtime_error("Failed to compile " + std::string(name) +
		                         " to SPIR-V: " + result.GetErrorMessage());
	}
	compiled.spirv.assign(result.cbegin(), result.cend());
	return compiled;
}

} // namespace jngl
