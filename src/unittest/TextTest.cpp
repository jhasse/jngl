#include "../jngl/text.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(TextTest) {
	Fixture f;
	jngl::Text text("test string\nline 2");
	BOOST_CHECK_EQUAL(text.getWidth(), 68);
	text.setCenter(-10, -10);
	BOOST_CHECK_EQUAL(text.getX(), -44);
	text.draw();
	std::string screenshotCentered = f.getAsciiArt();

	text.setPos(-10, -10);
	BOOST_CHECK_EQUAL(text.getX(), -10);
	text.draw();
	BOOST_CHECK(f.getAsciiArt() != screenshotCentered);

	text.setCenter(-10, -10); // restore previous state
	text.draw();
	BOOST_CHECK_EQUAL(text.getX(), -44);
	BOOST_CHECK_EQUAL(f.getAsciiArt(), screenshotCentered);

	text.setAlign(jngl::Alignment::CENTER);
	BOOST_CHECK_EQUAL(text.getX(), -44);
	text.draw(); // the second line should now be centered below the first
	BOOST_CHECK(f.getAsciiArt() != screenshotCentered);
}
