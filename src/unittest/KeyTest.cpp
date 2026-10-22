// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Key.hpp"
#include "../jngl/other.hpp"
#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>

#if __has_include(<SDL3/SDL_events.h>) && !defined(_WIN32)
#include <SDL3/SDL_events.h> // the SDL backend's, see "Key with an SDL event"
#define JNGL_KEY_TEST_SDL
#endif

TEST_CASE("Key") {
	CHECK(jngl::Key('q') == jngl::Key('Q'));
	CHECK(jngl::Key('q') == jngl::Key("q"));
	CHECK(jngl::Key("ö") == jngl::Key("Ö"));
	CHECK(jngl::Key(' ') == jngl::key::Space);
	CHECK(jngl::Key(" ") == jngl::key::Space);
	CHECK(jngl::Key('1') != jngl::Key('!'));
	CHECK(jngl::Key("ß") != jngl::Key("ẞ"));
	CHECK(jngl::Key(jngl::key::Ctrl) != jngl::key::CtrlL);
	CHECK_THROWS_AS(jngl::Key(""), std::runtime_error);
	CHECK_THROWS_AS(jngl::Key("qq"), std::runtime_error);
	CHECK_THROWS_AS(jngl::Key("\xC3"), std::runtime_error); // the first half of "ä"
	CHECK_THROWS_AS(jngl::Key(static_cast<char>(0xC3)), std::runtime_error);

	CHECK(jngl::Key('q').toString() == "Q");
	CHECK(jngl::Key("ä").toString() == "Ä");
	CHECK(jngl::Key("ß").toString() == "ß");
	CHECK(jngl::Key("÷").toString() == "÷");
	CHECK(jngl::Key('^').toString() == "^");
	CHECK(jngl::Key(' ').toString() == "Space");
	CHECK(jngl::Key(jngl::key::F10).toString() == "F10");

	for (int i = 0; i <= jngl::key::Any; ++i) {
		const jngl::Key key = static_cast<jngl::key::KeyType>(i);
		INFO(key.toString());
		CHECK(jngl::Key::fromString(key.toString()) == key);
	}
	for (const char* const character : { "q", "1", "^", "<", "ö", "ß", "é", "€", "😀" }) {
		const jngl::Key key(character);
		INFO(character);
		CHECK(jngl::Key::fromString(key.toString()) == key);
	}
	CHECK(jngl::Key::fromString("q") == jngl::Key('q'));
	CHECK(!jngl::Key::fromString(""));
	CHECK(!jngl::Key::fromString("qq"));
	CHECK(!jngl::Key::fromString("space"));
	CHECK(!jngl::Key::fromString("\xC3"));
}

TEST_CASE("keysPressed") {
	Fixture f(1);
	CHECK(jngl::keysPressed().empty());
	jngl::setKeyPressed("Q", true);
	jngl::setKeyPressed(jngl::key::F1, true);
	jngl::setKeyPressed("q", true); // the same key again
	CHECK(jngl::keysPressed() == std::vector<jngl::Key>{ 'q', jngl::key::F1 });
	CHECK(jngl::Key('q').pressed());
	CHECK(jngl::Key(jngl::key::F1).pressed());
	CHECK(!jngl::Key('w').pressed());
	jngl::updateInput();
	CHECK(jngl::keysPressed().empty());
	CHECK(!jngl::Key('q').pressed());
	CHECK(!jngl::Key(jngl::key::F1).pressed());
}

#ifdef JNGL_KEY_TEST_SDL
TEST_CASE("Key with an SDL event") {
	Fixture f(1);
	const auto send = [](const SDL_Keycode keycode, const bool down) {
		SDL_Event event{};
		event.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
		event.key.key = keycode;
		event.key.down = down;
		REQUIRE(SDL_PushEvent(&event));
		jngl::updateInput();
	};
	const jngl::Key ae("ä");

	send(0xE4, true); // the key that types ä on a German keyboard
	CHECK(jngl::keyDown("ä"));
	CHECK(jngl::keyPressed("ä"));
	CHECK(!jngl::keyDown("Ä"));
	CHECK(ae.down());
	CHECK(ae.pressed());
	CHECK(jngl::keysPressed() == std::vector{ ae });
	send(0xE4, false);
	CHECK(!jngl::keyDown("ä"));
	CHECK(!ae.down());

	send(SDLK_LSHIFT, true);
	CHECK(jngl::keysPressed() == std::vector<jngl::Key>{ jngl::key::ShiftL });
	send(0xE4, true);
	CHECK(jngl::keyDown("Ä"));
	CHECK(!jngl::keyDown("ä"));
	CHECK(ae.down());
	CHECK(ae.pressed());
	CHECK(jngl::keysPressed() == std::vector{ ae });
	send(SDLK_LSHIFT, false); // before the key
	send(0xE4, false);
	CHECK(!jngl::keyDown("Ä"));
	CHECK(!ae.down());

	// Shift doesn't change digits, see jngl::keyDown(char)
	send(SDLK_LSHIFT, true);
	send(SDLK_1, true);
	CHECK(jngl::keyDown("1"));
	CHECK(jngl::keyDown('1'));
	CHECK(jngl::Key('1').down());
	CHECK(jngl::keysPressed() == std::vector<jngl::Key>{ '1' });
	send(SDLK_1, false);
	send(SDLK_LSHIFT, false);

	send(SDLK_SPACE, true);
	CHECK(jngl::keyDown(" "));
	CHECK(jngl::keysPressed() == std::vector<jngl::Key>{ jngl::key::Space });
	send(SDLK_SPACE, false);
	CHECK(!jngl::keyDown(" "));

	send(SDLK_KP_1, true); // there's no jngl::Key for the numpad's keys
	CHECK(jngl::keysPressed().empty());
	send(SDLK_KP_1, false);
}
#endif
