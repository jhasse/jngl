// Copyright 2009-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "windowptr.hpp"

#include <cassert>
#include <stdexcept>

namespace jngl {

WindowPointer::WindowPointer() = default;

WindowPointer::~WindowPointer() {
	Delete();
}

Window* WindowPointer::operator->() const {
	ThrowIfNull();
	return ptr_;
}

WindowPointer::operator bool() const noexcept {
	return ptr_ != nullptr;
}

void WindowPointer::Set(Window* ptr) {
	assert(ptr_ == nullptr);
	ptr_ = ptr;
}

void WindowPointer::Delete() {
	delete ptr_;
	ptr_ = nullptr;
}

void WindowPointer::ThrowIfNull() const {
	if (ptr_ == nullptr) {
		throw std::runtime_error("Window hasn't been created yet. Use jngl::showWindow.");
	}
}

Window* WindowPointer::get() const noexcept {
	return ptr_;
}

} // namespace jngl
