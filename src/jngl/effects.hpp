// Copyright 2020-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Effects that can be applied to jngl::Widget
/// @file
#pragma once

#include "Vec2.hpp"
#include <cstdint>
#include <functional>

namespace jngl {

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

float linear(float);
float elastic(float);
float cubic(float);
float expo(float);

} // namespace easing

} // namespace jngl
