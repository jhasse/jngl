// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Sound.hpp"

#include "audio.hpp"
#include "jngl/debug.hpp"
#include "audio/track.hpp"
#include "audio/effect/loop.hpp"
#include "audio/effect/volume.hpp"

#include <cassert>

using namespace psemek; // FIXME

namespace jngl {

struct Sound::Impl {
	std::shared_ptr<audio::track> track;
	std::shared_ptr<audio::stream> stream;
	std::shared_ptr<audio::volume_control> volumeControl;
};

Sound::Sound(std::vector<char>& bufferData) : impl(new Impl{ audio::load_ogg(bufferData) }) {
	impl->stream = impl->volumeControl = audio::volume(impl->track->stream());
}

Sound::~Sound() {
}

bool Sound::isPlaying() const {
	return true; // TODO
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

std::shared_ptr<audio::stream> Sound::getStream() {
	return impl->stream;
}

} // namespace jngl
