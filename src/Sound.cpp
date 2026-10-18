// Copyright 2019-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Sound.hpp"

#include "audio.hpp"
#include "audio/Track.hpp"
#include "audio/effect/PanningControl.hpp"
#include "audio/effect/loop.hpp"
#include "audio/effect/volume.hpp"

#include <cassert>

namespace jngl {

struct Sound::Impl {
	std::shared_ptr<PlayingTrack> track;
	std::shared_ptr<Stream> stream;
	std::shared_ptr<audio::VolumeControl> volumeControl;
	std::shared_ptr<audio::PanningControl> panningControl;
	std::shared_ptr<std::vector<float>> buffer; //< we might outlive our SoundFile
	bool looping = false;
};

Sound::Sound(std::shared_ptr<std::vector<float>> bufferData)
: impl(new Impl{ .track = std::make_shared<PlayingTrack>(*bufferData),
                 .stream = {},
                 .volumeControl = {},
                 .panningControl = {},
                 .buffer = std::move(bufferData),
                 .looping = false }) {
	impl->stream = impl->volumeControl = std::make_shared<audio::VolumeControl>(impl->track);
}

Sound::~Sound() = default;

bool Sound::isPlaying() const {
	return impl->stream->isPlaying();
}

bool Sound::isLooping() const {
	return impl->looping;
}

void Sound::loop() {
	assert(!isLooping());
	assert(!impl->panningControl); // otherwise the panning's tail would cause gaps when looping
	impl->stream = audio::loop(impl->stream);
	impl->looping = true;
}

void Sound::setVolume(float v) {
	impl->volumeControl->gain(v);
}

void Sound::enablePanning() {
	assert(!impl->panningControl);
	impl->stream = impl->panningControl = std::make_shared<audio::PanningControl>(impl->stream);
}

void Sound::setPanning(float panning) {
	assert(impl->panningControl);
	impl->panningControl->setPanning(panning);
}

std::shared_ptr<Stream> Sound::getStream() {
	return impl->stream;
}

float Sound::progress() const {
	return impl->track->progress();
}

std::span<float> Sound::getSamples() {
	return impl->buffer ? std::span(*impl->buffer) : std::span<float>{};
}

} // namespace jngl
