#include "mixer.hpp"

#include <atomic_queue/atomic_queue.h>

#include <algorithm>
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>

namespace psemek::audio {

namespace {

struct mixer_impl final : mixer, std::enable_shared_from_this<mixer_impl> {
	void add(std::shared_ptr<Stream> stream) override;

	void remove(Stream* stream) override {
		gc();
		streamsToStop.push(stream);
	}

	std::size_t read(float* data, std::size_t sample_count) override;

	void rewind() override {
		assert(false);
	}

private:
	void gc() {
		Stream* tmp;
		while (streamsToRemoveOnMainThread.try_pop(tmp)) {
			auto it = std::find_if(streamsOnMainThread.begin(), streamsOnMainThread.end(),
			                       [tmp](const auto& stream) { return stream.get() == tmp; });
			assert(it != streamsOnMainThread.end());
			streamsOnMainThread.erase(it);
		}
	}
	std::vector<std::shared_ptr<Stream>> streamsOnMainThread;

	atomic_queue::AtomicQueue<Stream*, 10> streamsToRemoveOnMainThread;

	constexpr static size_t MAX_ACTIVE_STREAMS = 256;
	size_t numberOfActiveStreams = 0;
	Stream* activeStreams[MAX_ACTIVE_STREAMS]; //< only used on mixer thread

	constexpr static size_t BUFFER_SIZE = 996;
	float buffer[BUFFER_SIZE];

	atomic_queue::AtomicQueue<Stream*, 10> newStreams;

	atomic_queue::AtomicQueue<Stream*, 10> streamsToStop;

	std::atomic<std::size_t> played_{ 0 };
};

void mixer_impl::add(std::shared_ptr<Stream> stream) {
	gc();
	newStreams.push(stream.get());
	streamsOnMainThread.emplace_back(std::move(stream));
}

std::size_t mixer_impl::read(float* data, std::size_t sample_count) {
	if (sample_count > BUFFER_SIZE) {
		size_t first = read(data, BUFFER_SIZE);
		return first + read(data + BUFFER_SIZE, sample_count - BUFFER_SIZE);
	}

	Stream* tmp;
	while (numberOfActiveStreams < MAX_ACTIVE_STREAMS && newStreams.try_pop(tmp)) {
		activeStreams[numberOfActiveStreams++] = tmp;
	}
	while (streamsToStop.try_pop(tmp)) {
		auto it = std::find(activeStreams, activeStreams + numberOfActiveStreams, tmp);
		if (it != activeStreams + numberOfActiveStreams) {
			*it = activeStreams[--numberOfActiveStreams]; // move the last element to the one to be erased
			streamsToRemoveOnMainThread.push(tmp);
		}
	}

	std::fill(data, data + sample_count, 0.f);

	for (auto it = activeStreams; it != activeStreams + numberOfActiveStreams;) {
		auto& stream = *it;
		if (!stream) continue;

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
			streamsToRemoveOnMainThread.push(*it);
			*it = activeStreams[--numberOfActiveStreams]; // move the last element to the one to be erased
		} else {
			++it;
		}
	}

	played_.fetch_add(sample_count);

	return sample_count;
}

} // namespace

mixer_ptr make_mixer() {
	return std::make_shared<mixer_impl>();
}

} // namespace psemek::audio
