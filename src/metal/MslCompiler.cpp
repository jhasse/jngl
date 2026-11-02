// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "MslCompiler.hpp"

#include <spirv_cross_c.h>
#include <stdexcept>
#include <string>

namespace jngl {

namespace {

/// Owns a SPIRV-Cross context, which owns everything that's created from it, like the one in
/// HlslCompiler.cpp
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

std::string translate(const std::span<const uint32_t> spirv, const SpvExecutionModel stage) {
	SpirvCrossContext spvc;
	spvc_parsed_ir ir = nullptr;
	spvc.check(spvc_context_parse_spirv(spvc.context, spirv.data(), spirv.size(), &ir),
	           "Parsing SPIR-V");
	spvc_compiler compiler = nullptr;
	spvc.check(spvc_context_create_compiler(spvc.context, SPVC_BACKEND_MSL, ir,
	                                        SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler),
	           "Creating the MSL compiler");
	spvc_compiler_options options = nullptr;
	spvc.check(spvc_compiler_create_compiler_options(compiler, &options),
	           "Creating the MSL options");
	spvc.check(spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_MSL_PLATFORM,
	                                          SPVC_MSL_PLATFORM_MACOS),
	           "Setting the platform");
	// What every Mac that runs macOS 10.14 supports, see MetalRenderer's MTLCompileOptions
	spvc.check(spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_MSL_VERSION,
	                                          SPVC_MAKE_MSL_VERSION(2, 1, 0)),
	           "Setting the MSL version");
	spvc.check(spvc_compiler_install_compiler_options(compiler, options),
	           "Installing the MSL options");

	// See MslCompiler.hpp
	const auto bind = [&](const unsigned descSet, const unsigned binding, const char* const what) {
		spvc_msl_resource_binding_2 resource;
		spvc_msl_resource_binding_init_2(&resource);
		resource.stage = stage;
		resource.desc_set = descSet;
		resource.binding = binding;
		resource.count = 1;
		resource.msl_buffer = 0;
		resource.msl_texture = 0;
		resource.msl_sampler = 0;
		spvc.check(spvc_compiler_msl_add_resource_binding_2(compiler, &resource), what);
	};
	bind(SPVC_MSL_PUSH_CONSTANT_DESC_SET, SPVC_MSL_PUSH_CONSTANT_BINDING,
	     "Mapping the push constants");
	bind(0, 0, "Mapping the texture");
	bind(1, 0, "Mapping the uniforms");

	spvc.check(spvc_compiler_rename_entry_point(compiler, "main",
	                                            stage == SpvExecutionModelVertex
	                                                ? metalVertexEntryPoint
	                                                : metalFragmentEntryPoint,
	                                            stage),
	           "Renaming the entry point");

	const char* msl = nullptr;
	spvc.check(spvc_compiler_compile(compiler, &msl), "Translating SPIR-V to MSL");
	return msl;
}

} // namespace

std::string translateSpirvToMsl(const std::span<const uint32_t> spirv, const Shader::Type stage,
                                const char* const name) {
	try {
		return translate(spirv, stage == Shader::Type::VERTEX ? SpvExecutionModelVertex
		                                                      : SpvExecutionModelFragment);
	} catch (const std::exception& e) {
		throw std::runtime_error("Failed to translate " + std::string(name) +
		                         " shader to MSL: " + e.what());
	}
}

} // namespace jngl
