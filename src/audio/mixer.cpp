// Copyright 2023-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#include "mixer.hpp"

#include <atomic_queue/atomic_queue.h>

#include <algorithm>
#include <memory>
#include <vector>

namespace jngl {

struct Mixer::Impl {
	atomic_queue::AtomicQueue<Stream*, 10> streamsToRemoveOnMainThread;
	std::vector<Stream*> streamsToRemoveLater;

	constexpr static uintptr_t REMOVE_FLAG = 1;
	atomic_queue::AtomicQueue<uintptr_t, 20> commands;
};

Mixer::Mixer() : impl(std::make_unique<Impl>()) {
}
Mixer::~Mixer() = default;

void Mixer::gc() {
	Stream* tmp;
	while (impl->streamsToRemoveOnMainThread.try_pop(tmp)) {
		auto it = std::find_if(streamsOnMainThread.begin(), streamsOnMainThread.end(),
		                       [tmp](const auto& stream) { return stream.get() == tmp; });
		assert(it != streamsOnMainThread.end());
		streamsOnMainThread.erase(it);
	}
}

void Mixer::remove(const Stream* stream) {
	gc();
	assert(reinterpret_cast<uintptr_t>(stream) % 2 == 0); // NOLINT
	impl->commands.push(reinterpret_cast<uintptr_t>(stream) + Impl::REMOVE_FLAG); // NOLINT
}

void Mixer::rewind() {
	assert(false);
}

bool Mixer::isPlaying() const {
	for (const auto& stream : streamsOnMainThread) {
		if (stream->isPlaying()) {
			return true;
		}
	}
	return false;
}

void Mixer::add(std::shared_ptr<Stream> stream) {
	gc();
	assert(reinterpret_cast<uintptr_t>(stream.get()) % 2 == 0); // NOLINT
	impl->commands.push(reinterpret_cast<uintptr_t>(stream.get())); // NOLINT
	streamsOnMainThread.emplace_back(std::move(stream));
}

size_t Mixer::read(float* data, size_t sample_count) {
	if (sample_count > BUFFER_SIZE) {
		size_t first = read(data, BUFFER_SIZE);
		return first + read(data + BUFFER_SIZE, sample_count - BUFFER_SIZE);
	}

	auto end = impl->streamsToRemoveLater.end();
	for (auto it = impl->streamsToRemoveLater.begin(); it != end;) {
		if (*it) {
			if (impl->streamsToRemoveOnMainThread.try_push(*it)) {
				*it = nullptr;
			} else {
				break;
			}
			++it;
		} else {
			++it;
			if (it == end) {
				impl->streamsToRemoveLater.clear();
				break;
			}
		}
	}

	uintptr_t tmp;
	while (impl->commands.try_pop(tmp)) {
		if (tmp % 2 == 0) { // REMOVE_FLAG not set, means we have to add a new Stream*
			if (numberOfActiveStreams < MAX_ACTIVE_STREAMS) {
				activeStreams[numberOfActiveStreams++] = reinterpret_cast<Stream*>(tmp); // NOLINT
			}
		} else { // REMOVE_FLAG set
			auto stream = reinterpret_cast<Stream*>(tmp - Impl::REMOVE_FLAG); // NOLINT
			auto it = std::find(activeStreams, activeStreams + numberOfActiveStreams, stream);
			if (it != activeStreams + numberOfActiveStreams) {
				*it = activeStreams[--numberOfActiveStreams]; // move the last element to the one to
				                                              // be erased
				if (!impl->streamsToRemoveOnMainThread.try_push(stream)) {
					// we shall never ever block in this thread. Therefore only use try_push and
					// lets hope that the main thread will GC at some point.
					impl->streamsToRemoveLater.emplace_back(stream);
				}
			}
		}
	}

	std::fill(data, data + sample_count, 0.f);

	for (auto it = activeStreams; it != activeStreams + numberOfActiveStreams;) {
		auto& stream = *it;
		if (!stream) {
			continue;
		}

		auto read = stream->read(buffer, sample_count);

		{
			auto begin = buffer;
			auto end = begin + read;
			auto dst = data;
			for (; begin < end;) {
				*dst++ += *begin++;
			}
		}

		if (read < sample_count) {
			if (!impl->streamsToRemoveOnMainThread.try_push(*it)) {
				// we shall never ever block in this thread. Therefore only use try_push and lets
				// hope that the main thread will GC at some point. (see above)
				impl->streamsToRemoveLater.emplace_back(*it);
			}
			*it = activeStreams[--numberOfActiveStreams]; // move the last element to the one to be
			                                              // erased
		} else {
			++it;
		}
	}

	played_.fetch_add(sample_count);

	return sample_count;
}

} // namespace jngl
