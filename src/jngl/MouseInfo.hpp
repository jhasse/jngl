// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::MouseInfo class
/// @file
#pragma once

#include "Vec2.hpp"

#include <optional>

namespace jngl {

class MouseInfo {
public:
	void setMousePos(Vec2);

	struct Down {
		/// returns nullopt when the mouse has been released
		Vec2 newPos() const;
		bool released();

		Vec2 startPos() const;

		Down(MouseInfo&, Vec2 objectPos);
		Down(const Down&) = delete;
		Down(Down&&) noexcept;
		Down& operator=(const Down&) = delete;
		Down& operator=(Down&&) noexcept;
		~Down();

	private:
		MouseInfo* parent = nullptr;
		Vec2 startReference;
	};

	struct Over {
		Vec2 pos() const;

		/// calling this will result in all future calls of MouseInfo::pos returning nullopt
		///
		/// objectPos will be used to calculate newPos
		std::optional<Down> pressed(Vec2 objectPos);

		explicit Over(MouseInfo&);

	private:
		MouseInfo& parent;
	};

	/// will return nullopt if the mouse is already over another object
	std::optional<Over> pos();

private:
	bool enabled = true;
	bool down = false;
	Vec2 mousePos;
};

} // namespace jngl
