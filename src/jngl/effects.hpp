// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <functional>

namespace jngl {

class Effect {
public:
	enum class Action {
		NONE,
		REMOVE_EFFECT,
		REMOVE_WIDGET,
	};

	virtual ~Effect();
	[[nodiscard]] virtual Action step() = 0;
	virtual void beginDraw() const = 0;
	virtual void endDraw() const = 0;
};

class Zoom : public Effect {
public:
	explicit Zoom(std::function<float(float)>);
	Action step() override;
	void beginDraw() const override;
	void endDraw() const override;

private:
	std::function<float(float)> function;
	float time = 0;
};

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

} // namespace jngl
