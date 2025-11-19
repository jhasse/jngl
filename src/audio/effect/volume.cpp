// Copyright 2023-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "volume.hpp"

namespace jngl::audio {

VolumeControl::VolumeControl(std::shared_ptr<Stream> stream)
: base_(1.f, 1.f, 0.f), stream_(std::move(stream)) {
}

float VolumeControl::gain() const {
	return base_.gain_left();
}
float VolumeControl::gain(float value) {
	base_.gain_left(value);
	return base_.gain_right(value);
}

float VolumeControl::smoothness() const {
	return base_.smoothness();
}
float VolumeControl::smoothness(float value) {
	return base_.smoothness(value);
}

void VolumeControl::rewind() {
	stream_->rewind();
}

bool VolumeControl::isPlaying() const {
	return stream_->isPlaying();
}

std::size_t VolumeControl::read(float* data, std::size_t sample_count) {
	decltype(stream_) tmp;
	{
#if !defined(__cpp_lib_atomic_shared_ptr) || __cpp_lib_atomic_shared_ptr < 201711L
		std::unique_lock lock(mutex);
#endif
		tmp = stream_; // the main thread may change stream_ in replaceStream while we're in read
	}
	auto result = tmp->read(data, sample_count);
	base_.apply(data, result);
	return result;
}

void VolumeControl::replaceStream(std::shared_ptr<Stream> stream) {
#if !defined(__cpp_lib_atomic_shared_ptr) || __cpp_lib_atomic_shared_ptr < 201711L
	std::unique_lock lock(mutex);
#endif
	stream_ = std::move(stream);
}

} // namespace jngl::audio
