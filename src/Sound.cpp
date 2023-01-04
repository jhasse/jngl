// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Sound.hpp"

#include "audio.hpp"
#include "SoundParams.hpp"
#include "jngl/debug.hpp"
#include "audio/track.hpp"

using namespace psemek; // FIXME

namespace jngl {

struct Sound::Impl {
	std::shared_ptr<audio::track> track;
};

float Sound::masterVolume = 1.0f;

Sound::Sound(std::vector<char>& bufferData) : impl(new Impl{ audio::load_ogg(bufferData) }) {
}

Sound::~Sound() {
}

bool Sound::isPlaying() const {
	return true; // TODO
}

void Sound::loop() {
	// TODO
}

bool Sound::isStopped() const {
	return false; // TODO
}

void Sound::SetPitch(float p) {
	// TODO
}

void Sound::setVolume(float v) {
	// TODO
}

std::shared_ptr<audio::stream> Sound::getStream() {
	return impl->track->stream();
}

} // namespace jngl
