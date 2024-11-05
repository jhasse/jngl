// Copyright 2019-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Sound.hpp"

#include "audio.hpp"
#include "audio/Track.hpp"
#include "audio/effect/loop.hpp"
#include "audio/effect/volume.hpp"

#include <cassert>

namespace jngl {

struct Sound::Impl {
	std::shared_ptr<PlayingTrack> track;
	std::shared_ptr<Stream> stream;
	std::shared_ptr<audio::volume_control> volumeControl;
	std::shared_ptr<std::vector<float>> buffer; //< we might outlive our SoundFile
};

Sound::Sound(std::shared_ptr<std::vector<float>> bufferData)
: impl(new Impl{ std::make_shared<PlayingTrack>(*bufferData), {}, {}, std::move(bufferData) }) {
	impl->stream = impl->volumeControl = audio::volume(impl->track);
}

Sound::~Sound() = default;

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

void Sound::setVolume(float v) {
	impl->volumeControl->gain(v);
}

std::shared_ptr<Stream> Sound::getStream() {
	return impl->stream;
}

float Sound::progress() const {
	return impl->track->progress();
}

} // namespace jngl
