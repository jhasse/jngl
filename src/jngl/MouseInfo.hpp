// Copyright 2025-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::MouseInfo class
/// @file
#pragma once

#include "Vec2.hpp"

#include <optional>
#include <vector>

namespace jngl {

class Mat3;
class Window;

class MouseInfo {
	struct Impl;

public:
	/// Applies the \a transformationMatrix on the mouse position
	///
	/// Let's say you want to move the move position in MouseInfo 50 pixel to the right, you'd do:
	///
	/// \code
	/// jngl::Mat3 matrix;
	/// matrix.translate(jngl::Vec2(50, 0));
	/// jngl::mouseInfo().transform(matrix);
	/// \endcode
	///
	/// If `(mousePos.x, mousePos.y, 1)` is \f$p\f$ and \a transformationMatrix is \f$M\f$ this
	/// does:
	/// \f[
	/// p' = M^{-1} \cdot p
	/// \f]
	///
	/// It uses the inverse of the passed matrix, because normally transformations are for rendering
	/// where world coordinates are transformed into screen coordinates. The mouse position is in
	/// screen coordinates and you want to transform it to world coordinates. This is useful if you
	/// have a camera matrix and use it for rendering (applying it on jngl::modelview()) and here.
	void transform(const jngl::Mat3& transformationMatrix);

	struct Down {
		/// returns nullopt when the mouse has been released
		Vec2 newPos() const;
		bool released();

		Vec2 startPos() const;

		Down(MouseInfo::Impl&, Vec2 objectPos);
		Down(const Down&) = delete;
		Down(Down&&) noexcept;
		Down& operator=(const Down&) = delete;
		Down& operator=(Down&&) noexcept;
		~Down();

	private:
		MouseInfo::Impl* parent = nullptr;
		Vec2 startReference;
	};

	struct Over {
		Vec2 pos() const;

		/// calling this will result in all future calls of MouseInfo::cursors not returning this
		/// cursor
		///
		/// objectPos will be used to calculate newPos
		std::optional<Down> pressed(Vec2 objectPos);

		explicit Over(MouseInfo::Impl&);

	private:
		MouseInfo::Impl& parent;
	};

	/// Only returns mouse cursors that are currently not over something
	std::span<Over> cursors();

private:
	friend class jngl::Window;

	MouseInfo();
	~MouseInfo();
	void setMousePos(Vec2);

	std::vector<Impl> impls;
	std::vector<Over> activeCursors;
};

/// Returns a reference to the global MouseInfo instance, similar to jngl::modelview()
///
/// Any transformations on the position will be reset at the beginning of a step.
MouseInfo& input();

} // namespace jngl
