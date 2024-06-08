// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Channel.hpp"

#include "../audio/effect/pause.hpp"
#include "../audio/effect/volume.hpp"
#include "../audio/mixer.hpp"
#include "../audio.hpp"
#include "SoundFile.hpp"

namespace jngl {

struct Channel::Impl {
	Mixer* mixer = nullptr;
	audio::volume_control* volumeControl = nullptr;
	std::shared_ptr<audio::pause_control> pauseControl;
	std::shared_ptr<uint8_t> pauseCount = std::make_shared<uint8_t>(0);
};

Channel::Channel() : impl(std::make_unique<Impl>()) {
	auto mixer = std::make_shared<Mixer>();
	impl->mixer = mixer.get();
	auto volumeControl = audio::volume(mixer);
	impl->volumeControl = volumeControl.get();
	impl->pauseControl = audio::pause(std::move(volumeControl));
	Audio::handle().registerChannel(impl->pauseControl);
}

Channel::~Channel() {
	Audio::handle().unregisterChannel(*impl->pauseControl);
}

// defined in SoundFile.cpp
std::shared_ptr<SoundFile> getSoundFile(const std::string& filename, std::launch policy);

void Channel::play(const std::string& filename) {
	getSoundFile(filename, std::launch::deferred)->play(*this);
}

std::shared_ptr<SoundFile> Channel::loop(const std::string& filename) {
	auto tmp = getSoundFile(filename, std::launch::deferred);
	tmp->loop(*this);
	return tmp;
}

void Channel::stop(const std::string& filename) {
	getSoundFile(filename, std::launch::async)->stop(*this);
}

Finally Channel::pause() {
	if (*impl->pauseCount == 0) {
		impl->pauseControl->pause();
	}
	++(*impl->pauseCount);
	return Finally{ [this, weak = std::weak_ptr(impl->pauseCount)]() {
		if (auto count = weak.lock()) {
			--(*count);
			if (*count == 0) {
				impl->pauseControl->resume();
			}
		}
	} };
}

void Channel::setVolume(float volume) {
	impl->volumeControl->gain(volume);
}

Channel& Channel::main() {
	return Audio::handle().getMainChannel();
}

void Channel::add(std::shared_ptr<Stream> stream) {
	impl->mixer->add(std::move(stream));
}

void Channel::remove(const Stream* stream) {
	impl->mixer->remove(stream);
}

} // namespace jngl
