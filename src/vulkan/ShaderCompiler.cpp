// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "ShaderCompiler.hpp"

#include <shaderc/shaderc.hpp>
#include <stdexcept>
#include <string>

namespace jngl {

std::vector<uint32_t> compileGlslToSpirv(const char* const source,
                                         const VkShaderStageFlagBits stage, const char* const name) {
	shaderc_shader_kind kind = shaderc_glsl_vertex_shader;
	switch (stage) {
	case VK_SHADER_STAGE_VERTEX_BIT:
		kind = shaderc_glsl_vertex_shader;
		break;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		kind = shaderc_glsl_fragment_shader;
		break;
	default:
		throw std::runtime_error("Unsupported shader stage for SPIR-V compilation.");
	}

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetOptimizationLevel(shaderc_optimization_level_performance);
	options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);

	const shaderc::SpvCompilationResult result =
	    compiler.CompileGlslToSpv(source, kind, name, options);
	if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
		throw std::runtime_error("Failed to compile " + std::string(name) + " to SPIR-V: " +
		                         result.GetErrorMessage());
	}
	return { result.cbegin(), result.cend() };
}

} // namespace jngl
