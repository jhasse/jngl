// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "../App.hpp"
#include "../jngl/Finally.hpp"
#include "../jngl/Sprite.hpp"
#include "../jngl/input.hpp"
#include "../jngl/other.hpp"
#include "Fixture.hpp"

#include <boost/ut.hpp>
#include <filesystem>

boost::ut::suite suite = [] {
	using namespace boost::ut; // NOLINT

	"FinallyTest"_test = [] {
		bool called = false;
		{
			jngl::Finally _([&called]() {
				expect(!called);
				called = true;
			});
			expect(!called);
		}
		expect(called);
		{ // don't crash
			jngl::Finally _(nullptr);
		}
		called = false;
		{
			jngl::Finally f1([&called]() {
				expect(!called);
				called = true;
			});
			jngl::Finally f2 = std::move(f1);
			jngl::Finally f3(std::move(f2));
			{
				jngl::Finally f4(nullptr);
				f4 = std::move(f3);
				f3 = std::move(f4);
			}
			expect(!called);
		}
		expect(called);
	};

	"halfLoadTest"_test = [] {
		std::error_code err;
		std::filesystem::current_path("data", err);
		if (err) {
			std::filesystem::current_path("../data", err); // move out of build/bin folder
			if (err) {
				std::filesystem::current_path("../../data", err); // move out of build/Debug folder
				if (err) {
					std::filesystem::current_path("../../../data",
					                              err); // move out of out\build\x64-Debug
				}
			}
		}
		expect(jngl::getWidth("jngl") == 600_i);
		expect(jngl::getHeight("jngl") == 300_i);
		expect(throws<std::runtime_error>([] { jngl::load("jngl"); }));
	};

	"getBinaryPath"_test = [] { expect(!jngl::getBinaryPath().empty()); };

	"readAsset"_test = [] {
		expect(!jngl::readAsset("non existing file"));
		expect(throws<std::runtime_error>([] { jngl::readAsset("/some/absolute/path"); }));
	};

	"keyDown"_test = [] {
		Fixture f(1);
		jngl::keyDown("a");
		expect(throws<std::runtime_error>([] { jngl::keyDown("aa"); }));
		jngl::keyDown("ä");
		expect(throws<std::runtime_error>([] { jngl::keyDown("ää"); }));
	};

	"getConfigPath"_test = [] {
		// Note: This test only works if jngl::internal::getConfigPath() hasn't been called before

		// Explicitly set the display name (showWindow only sets it if empty)
		jngl::App::instance().setDisplayName("test:with?invalid*chars");

		// Verify that App::getDisplayName() has invalid chars
		const std::string displayName = jngl::App::instance().getDisplayName();
		expect(eq(displayName, std::string("test:with?invalid*chars")));

		const auto p = jngl::internal::getConfigPath();
		expect(eq(p.substr(p.size() - 22), std::string("/testwithinvalidchars/")));
	};
};
