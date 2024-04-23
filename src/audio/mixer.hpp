// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include "Stream.hpp"

#include <atomic>
#include <memory>
#include <vector>

namespace jngl {

class Mixer : public Stream {
public:
	Mixer();
	~Mixer() override;

	void add(std::shared_ptr<Stream> stream);
	void remove(const Stream*);

	void rewind() override;
	bool isPlaying() const override;
	size_t read(float*, size_t) override;

private:
	void gc();

	struct Impl;
	std::unique_ptr<Impl> impl;

	size_t numberOfActiveStreams = 0;

	constexpr static size_t BUFFER_SIZE = 996;
	float buffer[BUFFER_SIZE] = { 0 };

	constexpr static size_t MAX_ACTIVE_STREAMS = 256;
	Stream* activeStreams[MAX_ACTIVE_STREAMS] = { nullptr }; //< only used on mixer thread

	std::atomic<std::size_t> played_{ 0 };

	std::vector<std::shared_ptr<Stream>> streamsOnMainThread;
};

} // namespace jngl
