// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "TextInputSession.hpp"

#include "../windowptr.hpp"

#include <algorithm>
#include <optional>

namespace jngl {

namespace {

/// Cap for TextInputSession::Impl::buffer so an instance that's constructed but never read from
/// (take() never called) can't grow forever
constexpr size_t MAX_BUFFERED_BYTES = 4096;

} // namespace

struct TextInputSession::Impl {
	std::string buffer;
	TextInputType type;
	std::optional<Rect> area;
	double cursor = 0;
};

// The most recently constructed, not yet destroyed TextInputSession is stack.back(). Lives
// outside of Window (and outside of any Impl's ownership) so it survives jngl::showWindow
// recreating the Window, e.g. when toggling fullscreen or an Android app resuming.
std::vector<TextInputSession::Impl*> TextInputSession::stack;

// Set the first time jngl::getTextInput() (deprecated) is called, see
// internal::ensureLegacyTextInputStarted
bool TextInputSession::legacyStarted = false;

void TextInputSession::applyActive() {
	if (!pWindow) {
		return;
	}
	if (!stack.empty()) {
		pWindow->startTextInputSession(stack.back()->type);
		applyArea();
		return;
	}
	if (legacyStarted) {
		pWindow->startTextInputSession(TextInputType::Text);
		return;
	}
	pWindow->stopTextInputSession();
}

void TextInputSession::applyArea() {
	if (!pWindow || stack.empty()) {
		return;
	}
	const Impl& top = *stack.back();
	if (top.area) {
		pWindow->setTextInputArea(*top.area, top.cursor);
	}
}

TextInputSession::TextInputSession(const TextInputType type)
: impl(std::make_unique<Impl>(Impl{ .buffer = {}, .type = type, .area = {}, .cursor = 0 })) {
	stack.push_back(impl.get());
	applyActive();
}

TextInputSession::TextInputSession(TextInputSession&&) noexcept = default;
TextInputSession& TextInputSession::operator=(TextInputSession&&) noexcept = default;

TextInputSession::~TextInputSession() {
	if (!impl) { // moved-from
		return;
	}
	const bool wasActive = !stack.empty() && stack.back() == impl.get();
	stack.erase(std::remove(stack.begin(), stack.end(), impl.get()), stack.end());
	if (wasActive) {
		applyActive();
	}
}

std::string TextInputSession::take() {
	std::string result;
	std::swap(result, impl->buffer);
	return result;
}

void TextInputSession::setInputArea(const Rect area, const double cursor) {
	impl->area = area;
	impl->cursor = cursor;
	if (!stack.empty() && stack.back() == impl.get()) {
		applyActive();
	}
}

namespace internal {

void feedTextInput(const std::string& utf8) {
	if (TextInputSession::stack.empty()) {
		return;
	}
	auto& buffer = TextInputSession::stack.back()->buffer;
	if (buffer.size() >= MAX_BUFFERED_BYTES) {
		return;
	}
	buffer += utf8;
}

void reapplyTextInputSession() {
	TextInputSession::applyActive();
}

void reapplyTextInputArea() {
	TextInputSession::applyArea();
}

void ensureLegacyTextInputStarted() {
	if (TextInputSession::legacyStarted) {
		return;
	}
	TextInputSession::legacyStarted = true;
	TextInputSession::applyActive();
}

} // namespace internal

} // namespace jngl
