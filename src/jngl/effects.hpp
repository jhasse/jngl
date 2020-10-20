// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

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

} // namespace jngl
