// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Fixture.hpp"

#include <boost/ut.hpp>
#include <cmath>
#include <jngl.hpp>
#include <jngl/AppParameters.hpp>

Fixture::Fixture(const double scaleFactor) {
	jngl::setScaleFactor(scaleFactor);
	jngl::showWindow("unit test", static_cast<int>(std::lround(320 * scaleFactor)),
	                 static_cast<int>(std::lround(70 * scaleFactor)), false, { 32, 7 }, { 32, 7 });
	reset();
	emptyAsciiArt = getAsciiArt();
	using namespace boost::ut; // NOLINT
	expect(eq(emptyAsciiArt, std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒                              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
}

Fixture::~Fixture() {
	jngl::hideWindow();
}

std::string Fixture::getAsciiArt() const {
	const int w = jngl::getWindowWidth();
	const int h = jngl::getWindowHeight();
	auto buffer = jngl::readPixels();

	// ASCII art should always have the same size, therefore let's take the scaleFactor into
	// account:
	auto reduceFactorAsFloat = static_cast<float>(10 * jngl::getScaleFactor());
	int reduceFactor = static_cast<int>(std::lround(reduceFactorAsFloat));
	using namespace boost::ut; // NOLINT
	expect(approx(reduceFactorAsFloat, reduceFactor, 1e-6));

	expect(eq(w % reduceFactor, 0));
	expect(eq(h % reduceFactor, 0));
	size_t reducedW = w / reduceFactor;
	std::vector<std::vector<std::vector<float>>> reduced;
	size_t index = 0;
	for (int y = 0; y < h; y += reduceFactor) {
		reduced.emplace_back(reducedW, std::vector<float>(3, 0.0f));
		for (int i = 0; i < reduceFactor; ++i) {
			for (int x = 0; x < w; x += reduceFactor) {
				for (int j = 0; j < reduceFactor; ++j) {
					for (auto& cell : reduced.back().at(x / reduceFactor)) {
						assert(0.0f <= cell && cell <= 1.0f);
						cell += buffer[index] / float(reduceFactor * reduceFactor);
						++index;
					}
				}
			}
		}
	}
	expect(eq(index, buffer.size()));
	assert(reduced.size() == size_t(h / reduceFactor));
	std::string out = "\n"; // Start with a newline for prettier output by Boost.Test
	for (auto itRow = reduced.rbegin(); itRow != reduced.rend(); ++itRow) {
		assert(itRow->size() == reducedW);
		for (const auto& cell : *itRow) {
			// ASCII:
			// const static std::vector<std::string> chars = { "@", "#", "%", "x", "o",
			//                                                 ";", ":", ",", ".", " " };

			// UTF-8:
			const static std::vector<std::string> chars = { "█", "▓", "▒", "░", " " };

			float gray = (cell.at(0) + cell.at(1) + cell.at(2)) / 3.0f;
			const size_t index = std::lround(gray * float(chars.size() - 1));
			assert(index < chars.size());
			out += chars[index];
		}
		out += "\n";
	}
	// assert(emptyAsciiArt != out); // We shouldn't call this function without drawing something first
	reset();
	return out;
}

void Fixture::reset() {
	jngl::swapBuffers();
	jngl::updateInput();

	const double frameSize = 5;
	const jngl::Vec2 screen(jngl::getScreenWidth(), jngl::getScreenHeight());
	jngl::setColor(0x000000_rgb);
	jngl::drawRect(-screen.x / 2, screen.y / 2 - frameSize, screen.x, frameSize); // bottom
	jngl::drawRect(-screen.x / 2, -screen.y / 2, screen.x, frameSize);            // top
	jngl::drawRect(screen.x / 2 - frameSize, -screen.y / 2, frameSize, screen.y); // right
	jngl::drawRect(-screen.x / 2, -screen.y / 2, frameSize, screen.y);            // left
}
