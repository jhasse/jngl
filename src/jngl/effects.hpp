// Copyright 2020-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Effects that can be applied to jngl::Widget
/// @file
#pragma once

#include "Vec2.hpp"
#include <cstdint>
#include <functional>

namespace jngl {

class Mat3;

/// Base class for effects that can be applied to jngl::Widget
class Effect {
public:
	enum class Action : uint8_t {
		NONE,
		REMOVE_EFFECT,
		REMOVE_WIDGET,
	};

	virtual ~Effect();
	[[nodiscard]] virtual Action step() = 0;
	virtual void beginDraw() const = 0;
	virtual void endDraw() const = 0;

	/// Called before drawing the widget
	virtual void updateModelview(Mat3& modelview) const;
};

class UpdateModelview : public Effect {
public:
	/// Pass a function that gets the time the effect is in use and a reference to the Modelview so
	/// that it can update it
	///
	/// Wiggle effect example:
	/// \code
	/// widget->addEffect<jngl::UpdateModelview>([](float t, jngl::Mat3& modelview) {
	/// 	float effectIntensity = std::max(std::sin(t * 3), 0.f);
	/// 	modelview.rotate(std::sin(t * 50) * 0.08 * effectIntensity)
	/// 	         .scale(1 + effectIntensity * 0.1);
	/// });
	/// \endcode
	explicit UpdateModelview(std::function<void(float t, Mat3&)>);
	Action step() override;
	void beginDraw() const override;
	void endDraw() const override;
	void updateModelview(Mat3& modelview) const override;

private:
	std::function<void(float t, Mat3&)> function;
	float time = 0;
};

/// Scales the ModelView matrix
class Zoom : public Effect {
public:
	/// f(t)
	explicit Zoom(std::function<float(float)>);
	Action step() override;
	void beginDraw() const override;
	void endDraw() const override;

private:
	std::function<float(float)> function;
	float time = 0;
};

/// Executes a specific action, e.g. removes the Widget
class Executor : public Effect {
public:
	explicit Executor(std::function<Action(float)>);
	Action step() override;

	/// Does nothing
	void beginDraw() const override;
	void endDraw() const override;

private:
	std::function<Action(float)> function;
	float time = 0;
};

class Move : public Effect {
public:
	explicit Move(Vec2 offset, std::function<float(float)>);
	Action step() override;
	void beginDraw() const override;
	void endDraw() const override;

private:
	Vec2 offset;
	float progress;
	std::function<float(float)> function;
	float time = 0;
};

namespace easing {

/// Easing functions f(t) mapping t in [0, 1] to a value in [0, 1] (values
/// outside this range are clamped). They share the same shape: f(0) == 0 and
/// f(1) == 1, only the path between differs.

/// Constant speed, no easing.
///
///   1|        /
///    |      /
///    |    /
///    |  /
///   0|/_________
///    0         1
float linear(float);

/// Overshoots past 1 and oscillates back, like a spring settling into place.
///
///   1|    /\  ___
///    |   /  \/
///    |  /
///    | /
///   0|/_________
///    0         1
float elastic(float);

/// Ease-out: starts fast, decelerates into the target (1 - (1 - t)^3).
///
///   1|     ____----
///    |   /
///    |  /
///    | /
///   0|/_________
///    0         1
float cubic(float);

/// Ease-out like cubic(), but with a sharper start and flatter tail.
///
///   1| _---------
///    |/
///    |
///    |
///   0|__________
///    0         1
float expo(float);

} // namespace easing

} // namespace jngl
