// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "../App.hpp"
#include "../jngl/Finally.hpp"
#include "../jngl/Sprite.hpp"
#include "../jngl/input.hpp"
#include "../jngl/other.hpp"
#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("FinallyTest") {
	bool called = false;
	{
		jngl::Finally _([&called]() {
			REQUIRE(!called);
			called = true;
		});
		REQUIRE(!called);
	}
	REQUIRE(called);
	{ // don't crash
		jngl::Finally _(nullptr);
	}
	called = false;
	{
		jngl::Finally f1([&called]() {
			REQUIRE(!called);
			called = true;
		});
		jngl::Finally f2 = std::move(f1);
		jngl::Finally f3(std::move(f2));
		{
			jngl::Finally f4(nullptr);
			f4 = std::move(f3);
			f3 = std::move(f4);
		}
		REQUIRE(!called);
	}
	REQUIRE(called);
}

TEST_CASE("halfLoadTest") {
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
	REQUIRE(jngl::getWidth("jngl") == 600);
	REQUIRE(jngl::getHeight("jngl") == 300);
	REQUIRE_THROWS_AS(jngl::load("jngl"), std::runtime_error);
}

TEST_CASE("getBinaryPath") {
	REQUIRE(!jngl::getBinaryPath().empty());
}

TEST_CASE("readAsset") {
	REQUIRE(!jngl::readAsset("non existing file"));
	REQUIRE_THROWS_AS(jngl::readAsset("/some/absolute/path"), std::runtime_error);
}

TEST_CASE("keyDown") {
	Fixture f(1);
	jngl::keyDown("a");
	REQUIRE_THROWS_AS(jngl::keyDown("aa"), std::runtime_error);
	jngl::keyDown("ä");
	REQUIRE_THROWS_AS(jngl::keyDown("ää"), std::runtime_error);
}

TEST_CASE("getConfigPath") {
	// Note: This test only works if jngl::internal::getConfigPath() hasn't been called before

	// Explicitly set the display name (showWindow only sets it if empty)
	jngl::App::instance().setDisplayName("test:with?invalid*chars");

	// Verify that App::getDisplayName() has invalid chars
	const std::string displayName = jngl::App::instance().getDisplayName();
	REQUIRE(displayName == "test:with?invalid*chars");

	const auto p = jngl::internal::getConfigPath();
	REQUIRE(p.substr(p.size() - 22) == "/testwithinvalidchars/");
}
