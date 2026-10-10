// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "SoundPanning.hpp"

#include "../Sound.hpp"
#include "Channel.hpp"

#include <utility>

namespace jngl {

SoundPanning::SoundPanning() = default;

SoundPanning::~SoundPanning() {
	stop();
}

SoundPanning::SoundPanning(SoundPanning&&) noexcept = default;

SoundPanning& SoundPanning::operator=(SoundPanning&& other) noexcept {
	if (this != &other) {
		stop();
		buffer = std::move(other.buffer);
		channel = other.channel;
		loop = other.loop;
		volume = other.volume;
		sound = std::move(other.sound);
	}
	return *this;
}

SoundPanning::SoundPanning(std::shared_ptr<std::vector<float>> buffer, Channel& channel, bool loop)
: buffer(std::move(buffer)), channel(&channel), loop(loop) {
}

void SoundPanning::setPanning(float panning) {
	if (buffer) {
		// First call: Now that we know the initial panning, we can start playing
		auto tmp = std::make_shared<Sound>(std::move(buffer));
		if (loop) {
			tmp->loop();
		}
		tmp->enablePanning();
		tmp->setPanning(panning);
		tmp->setVolume(volume);
		sound = tmp;
		channel->play(std::move(tmp));
	} else if (const auto sound_ = sound.lock()) {
		sound_->setPanning(panning);
	}
}

void SoundPanning::stop() {
	buffer.reset();
	// If the Channel has been destroyed, so has the Sound, so we won't access a dangling channel:
	if (const auto sound_ = sound.lock()) {
		channel->stop(sound_);
	}
}

void SoundPanning::setVolume(float volume) {
	this->volume = volume;
	if (const auto sound_ = sound.lock()) {
		sound_->setVolume(volume);
	}
}

bool SoundPanning::isPlaying() const {
	if (const auto sound_ = sound.lock()) {
		return sound_->isPlaying();
	}
	return false;
}

} // namespace jngl
