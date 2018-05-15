#include "../jngl/text.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(TextTest) {
	// Test with two rather big scale factors to avoid rounding errors:
	for (double scaleFactor : { 4, 6 }) {
		Fixture f(scaleFactor);
		jngl::Text text("test string\nline 2");
		BOOST_CHECK_EQUAL(text.getWidth(), 69);
		text.setCenter(-10, -10);
		BOOST_CHECK_EQUAL(text.getX(), -44);
		text.draw();
		const std::string screenshotCentered = R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒           ░░                 ▒
▒                              ▒
▒          ░░░░░░░░            ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)";
		BOOST_CHECK_EQUAL(f.getAsciiArt(), screenshotCentered);

		text.setPos(-10, -10);
		BOOST_CHECK_EQUAL(text.getX(), -10);
		text.draw();
		BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒              ░░ ░            ▒
▒              ░  ░            ▒
▒              ░░░░░░░         ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

		text.setCenter(-10, -10); // restore previous state
		text.draw();
		BOOST_CHECK_EQUAL(text.getX(), -44);
		BOOST_CHECK_EQUAL(f.getAsciiArt(), screenshotCentered);

		text.setAlign(jngl::Alignment::CENTER);
		BOOST_CHECK_EQUAL(text.getX(), -44);
		text.draw(); // the second line should now be centered below the first
		BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒            ░░░░              ▒
▒               ░              ▒
▒          ░░░░░░░░            ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	}
}
