// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Sound.hpp"

#include "audio.hpp"
#include "audio/constants.hpp"
#include "audio/effect/loop.hpp"
#include "audio/effect/pitch.hpp"
#include "audio/effect/volume.hpp"
#include "audio/Track.hpp"
#include "jngl/debug.hpp"

#include <cassert>

namespace jngl {

struct Sound::Impl {
	std::shared_ptr<Track> track;
	std::shared_ptr<Stream> stream;
	std::shared_ptr<audio::volume_control> volumeControl;
};

Sound::Sound(std::vector<float>& bufferData, long frequency)
: impl(new Impl{ load_raw(bufferData) }) {
	auto stream = impl->track->stream();
	if (frequency != jngl::audio::frequency) {
		stream =
		    audio::pitch(std::move(stream), static_cast<float>(frequency) / jngl::audio::frequency);
	}
	impl->stream = impl->volumeControl = audio::volume(std::move(stream));
}

Sound::~Sound() {
}

bool Sound::isPlaying() const {
	return impl->stream->isPlaying();
}

bool Sound::isLooping() const {
	return impl->stream != impl->volumeControl;
}

void Sound::loop() {
	assert(!isLooping());
	impl->stream = audio::loop(impl->volumeControl);
}

bool Sound::isStopped() const {
	return false; // TODO
}

void Sound::SetPitch(float p) {
	// TODO
}

void Sound::setVolume(float v) {
	impl->volumeControl->gain(v);
}

std::shared_ptr<Stream> Sound::getStream() {
	return impl->stream;
}

} // namespace jngl
