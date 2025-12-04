// Copyright 2023-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "../Stream.hpp"
#include "volume_base.hpp"

#include <memory>
#if defined(__cpp_lib_atomic_shared_ptr) && __cpp_lib_atomic_shared_ptr >= 201711L
#include <atomic>
#else
#include <mutex>
#endif
namespace jngl::audio {

class VolumeControl : public Stream {
public:
	explicit VolumeControl(std::shared_ptr<Stream> stream);

	float gain() const;
	float gain(float value);

	float smoothness() const;
	float smoothness(float value);

	void rewind() override;

	bool isPlaying() const override;

	std::size_t read(float* data, std::size_t sample_count) override;

	void replaceStream(std::shared_ptr<Stream>);

private:
	volume_base base_;
#if defined(__cpp_lib_atomic_shared_ptr) && __cpp_lib_atomic_shared_ptr >= 201711L
	std::atomic<std::shared_ptr<Stream>> stream_;
#else
	std::mutex mutex;
	std::shared_ptr<Stream> stream_;
#endif
};

} // namespace jngl::audio
