// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "shapes.hpp"

#include "../main.hpp"
#include "../opengl.hpp"
#include "../spriteimpl.hpp"
#include "Alpha.hpp"
#include "matrix.hpp"
#include "screen.hpp"

#include <stack>

namespace jngl {

Rgba gShapeColor{ 0, 0, 0, 1 };

void setColor(const Rgb rgb) {
	gShapeColor.setRgb(rgb);
}

void setColor(const Rgb color, const unsigned char alpha) {
	gShapeColor = Rgba(color, Alpha::u8(alpha));
}

void setColor(unsigned char red, unsigned char green, unsigned char blue) {
	gShapeColor.setRgb(Rgb::u8(red, green, blue));
}

void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
	gShapeColor = Rgba::u8(red, green, blue, alpha);
}

void setAlpha(const uint8_t alpha) {
	gShapeColor.setAlpha(Alpha::u8(alpha));
}

std::stack<Alpha> alphas;

void pushAlpha(unsigned char alpha) {
	alphas.emplace(gShapeColor.getAlpha());
	setAlpha(Alpha(gShapeColor.getAlpha() * static_cast<float>(alpha) / 255).u8());
}

void popAlpha() {
	setAlpha(alphas.top().u8());
	alphas.pop();
}

void drawEllipse(float xmid, float ymid, float width, float height, float startAngle) {
	drawEllipse(modelview().translate({xmid, ymid}), width, height, startAngle);
}

void drawEllipse(const Vec2 position, const float width, const float height,
                 const float startAngle) {
	drawEllipse(modelview().translate(position), width, height, startAngle);
}

void drawEllipse(Mat3 modelview, float width, float height, float startAngle) {
	glBindVertexArray(opengl::vaoStream);
	auto tmp = useSimpleShaderProgram(
	    modelview.scale(static_cast<float>(getScaleFactor()), static_cast<float>(getScaleFactor())),
	    gShapeColor);
	std::vector<float> vertexes;
	vertexes.push_back(0.f);
	vertexes.push_back(0.f);
	for (float t = startAngle; t < 2.f * M_PI; t += 0.1f) {
		vertexes.push_back(width * std::sin(t));
		vertexes.push_back(-height * std::cos(t));
	}
	vertexes.push_back(0.f);
	vertexes.push_back(-height);
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexes.size() * sizeof(float)),
	             vertexes.data(), GL_STREAM_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vertexes.size() / 2));
}

void drawCircle(const Vec2 position, const float radius, const float startAngle) {
	drawEllipse(modelview().translate(position), radius, radius, startAngle);
}

void drawCircle(Mat3 modelview, const float radius, const float startAngle) {
	drawEllipse(modelview, radius, radius, startAngle);
}

void drawCircle(Mat3 modelview, const float radius) {
	drawCircle(modelview, radius, gShapeColor);
}

void drawCircle(Mat3 modelview, const float radius, const Rgba color) {
	glBindVertexArray(opengl::vaoStream);
	auto tmp =
	    useSimpleShaderProgram(modelview.scale(radius).scale(static_cast<float>(getScaleFactor()),
	                                                         static_cast<float>(getScaleFactor())),
	                           color);
	// clang-format off
	const static float vertexes[] = {
		1, 0, 0.9951847, 0.09801714, 0.9807853, 0.1950903, 0.9569403, 0.2902847, 0.9238795,
		0.3826834, 0.8819213, 0.4713967, 0.8314696, 0.5555702, 0.7730105, 0.6343933, 0.7071068,
		0.7071068, 0.6343933, 0.7730105, 0.5555702, 0.8314696, 0.4713967, 0.8819213, 0.3826834,
		0.9238795, 0.2902847, 0.9569403, 0.1950903, 0.9807853, 0.09801714, 0.9951847, 6.123234e-17,
		1, -0.09801714, 0.9951847, -0.1950903, 0.9807853, -0.2902847, 0.9569403, -0.3826834,
		0.9238795, -0.4713967, 0.8819213, -0.5555702, 0.8314696, -0.6343933, 0.7730105, -0.7071068,
		0.7071068, -0.7730105, 0.6343933, -0.8314696, 0.5555702, -0.8819213, 0.4713967, -0.9238795,
		0.3826834, -0.9569403, 0.2902847, -0.9807853, 0.1950903, -0.9951847, 0.09801714, -1,
		1.224647e-16, -0.9951847, -0.09801714, -0.9807853, -0.1950903, -0.9569403, -0.2902847,
		-0.9238795, -0.3826834, -0.8819213, -0.4713967, -0.8314696, -0.5555702, -0.7730105,
		-0.6343933, -0.7071068, -0.7071068, -0.6343933, -0.7730105, -0.5555702, -0.8314696,
		-0.4713967, -0.8819213, -0.3826834, -0.9238795, -0.2902847, -0.9569403, -0.1950903,
		-0.9807853, -0.09801714, -0.9951847, -1.83697e-16, -1, 0.09801714, -0.9951847,
		0.1950903, -0.9807853, 0.2902847, -0.9569403, 0.3826834, -0.9238795, 0.4713967,
		-0.8819213, 0.5555702, -0.8314696, 0.6343933, -0.7730105, 0.7071068, -0.7071068,
		0.7730105, -0.6343933, 0.8314696, -0.5555702, 0.8819213, -0.4713967, 0.9238795,
		-0.3826834, 0.9569403, -0.2902847, 0.9807853, -0.1950903, 0.9951847, -0.09801714
	};
	// clang-format on
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(vertexes)), vertexes,
	             GL_STREAM_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(sizeof(vertexes) / sizeof(float) / 2));
}

} // namespace jngl
