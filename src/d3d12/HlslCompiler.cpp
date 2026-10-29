// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "HlslCompiler.hpp"

#include "../log.hpp"

#include <d3dcompiler.h>
#include <spirv_cross/spirv_cross_c.h>
#include <stdexcept>
#include <string>
#include <wrl/client.h>

namespace jngl {

namespace {

/// Owns a SPIRV-Cross context, which owns everything that's created from it
class SpirvCrossContext {
public:
	SpirvCrossContext() {
		if (spvc_context_create(&context) != SPVC_SUCCESS) {
			throw std::runtime_error("spvc_context_create failed.");
		}
	}
	~SpirvCrossContext() {
		spvc_context_destroy(context);
	}
	SpirvCrossContext(const SpirvCrossContext&) = delete;
	SpirvCrossContext& operator=(const SpirvCrossContext&) = delete;
	SpirvCrossContext(SpirvCrossContext&&) = delete;
	SpirvCrossContext& operator=(SpirvCrossContext&&) = delete;

	/// Throws if \a result isn't SPVC_SUCCESS, with what \a what was trying to do and why it failed
	void check(const spvc_result result, const char* const what) const {
		if (result != SPVC_SUCCESS) {
			throw std::runtime_error(std::string(what) +
			                         " failed: " + spvc_context_get_last_error_string(context));
		}
	}

	spvc_context context = nullptr;
};

std::string translateToHlsl(const std::span<const uint32_t> spirv, const SpvExecutionModel stage) {
	SpirvCrossContext spvc;
	spvc_parsed_ir ir = nullptr;
	spvc.check(spvc_context_parse_spirv(spvc.context, spirv.data(), spirv.size(), &ir),
	           "Parsing SPIR-V");
	spvc_compiler compiler = nullptr;
	spvc.check(spvc_context_create_compiler(spvc.context, SPVC_BACKEND_HLSL, ir,
	                                        SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler),
	           "Creating the HLSL compiler");
	spvc_compiler_options options = nullptr;
	spvc.check(spvc_compiler_create_compiler_options(compiler, &options),
	           "Creating the HLSL options");
	// For register spaces, which D3D12 supports, and D3DCompile's newest target
	spvc.check(spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_HLSL_SHADER_MODEL, 51),
	           "Setting the shader model");
	spvc.check(spvc_compiler_install_compiler_options(compiler, options),
	           "Installing the HLSL options");

	// See HlslCompiler.hpp
	spvc_hlsl_resource_binding binding;
	spvc_hlsl_resource_binding_init(&binding);
	binding.stage = stage;
	binding.desc_set = SPVC_HLSL_PUSH_CONSTANT_DESC_SET;
	binding.binding = SPVC_HLSL_PUSH_CONSTANT_BINDING;
	binding.cbv = { 0, 0 };
	spvc.check(spvc_compiler_hlsl_add_resource_binding(compiler, &binding),
	           "Mapping the push constants");
	spvc_hlsl_resource_binding_init(&binding);
	binding.stage = stage;
	binding.desc_set = 0;
	binding.binding = 0;
	binding.srv = { 0, 0 };
	binding.sampler = { 0, 0 };
	spvc.check(spvc_compiler_hlsl_add_resource_binding(compiler, &binding), "Mapping the texture");
	spvc_hlsl_resource_binding_init(&binding);
	binding.stage = stage;
	binding.desc_set = 1;
	binding.binding = 0;
	binding.cbv = { 0, 1 };
	spvc.check(spvc_compiler_hlsl_add_resource_binding(compiler, &binding), "Mapping the uniforms");

	const char* hlsl = nullptr;
	spvc.check(spvc_compiler_compile(compiler, &hlsl), "Translating SPIR-V to HLSL");
	return hlsl;
}

} // namespace

std::vector<uint8_t> compileSpirvToDxbc(const std::span<const uint32_t> spirv,
                                        const Shader::Type stage, const char* const name) {
	const bool vertex = stage == Shader::Type::VERTEX;
	std::string hlsl;
	try {
		hlsl = translateToHlsl(spirv, vertex ? SpvExecutionModelVertex : SpvExecutionModelFragment);
	} catch (const std::exception& e) {
		throw std::runtime_error("Failed to translate " + std::string(name) +
		                         " shader to HLSL: " + e.what());
	}

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef NDEBUG
	flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#else
	flags |= D3DCOMPILE_DEBUG;
#endif
	Microsoft::WRL::ComPtr<ID3DBlob> bytecode;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	const HRESULT result = D3DCompile(hlsl.data(), hlsl.size(), name, nullptr, nullptr, "main",
	                                  vertex ? "vs_5_1" : "ps_5_1", flags, 0, &bytecode, &errors);
	if (FAILED(result)) {
		std::string message = "Failed to compile " + std::string(name) + " shader";
		if (errors) {
			message += ": ";
			message.append(static_cast<const char*>(errors->GetBufferPointer()),
			               errors->GetBufferSize());
		}
		internal::debug("HLSL of the shader that failed to compile:\n{}", hlsl);
		throw std::runtime_error(message);
	}
	const auto* const begin = static_cast<const uint8_t*>(bytecode->GetBufferPointer());
	return { begin, begin + bytecode->GetBufferSize() };
}

} // namespace jngl
