// Copyright 2024-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Channel.hpp"

#include "../Sound.hpp"
#include "../audio.hpp"
#include "../audio/effect/pause.hpp"
#include "../audio/effect/volume.hpp"
#include "../audio/mixer.hpp"
#include "SoundFile.hpp"

#include <algorithm>

namespace jngl {

struct Channel::Impl {
	Mixer* mixer = nullptr;
	audio::VolumeControl* volumeControl = nullptr;
	std::shared_ptr<audio::pause_control> pauseControl;
	std::shared_ptr<uint8_t> pauseCount = std::make_shared<uint8_t>(0);

	/// All sounds currently playing (or finished) on this Channel
	std::vector<std::shared_ptr<Sound>> sounds;
};

Channel::Channel() : impl(std::make_unique<Impl>()) {
	auto mixer = std::make_shared<Mixer>();
	impl->mixer = mixer.get();
	auto volumeControl = std::make_shared<audio::VolumeControl>(mixer);
	impl->volumeControl = volumeControl.get();
	impl->pauseControl = audio::pause(std::move(volumeControl));
	Audio::handle().registerChannel(impl->pauseControl);
}

Channel::~Channel() {
	if (auto audio = Audio::handleIfAlive()) {
		audio->unregisterChannel(*impl->pauseControl);
	}
}

void Channel::play(const std::string& filename) {
	Audio::handle().getSoundFile(filename, std::launch::deferred)->play(*this);
}

void Channel::play(std::shared_ptr<Sound> sound) {
	impl->mixer->add(sound->getStream());
	impl->sounds.erase(std::remove_if(impl->sounds.begin(), impl->sounds.end(),
	                                  [](const auto& s) { return !s->isPlaying(); }),
	                   impl->sounds.end());
	impl->sounds.emplace_back(std::move(sound));
}

std::shared_ptr<SoundFile> Channel::loop(const std::string& filename) {
	auto tmp = Audio::handle().getSoundFile(filename, std::launch::deferred);
	tmp->loop(*this);
	return tmp;
}

void Channel::stop(const std::string& filename) {
	if (auto sound = Audio::handle().getSoundFileIfLoaded(filename)) {
		sound->stop(*this);
	}
}

void Channel::stop(const std::shared_ptr<Sound>& sound) {
	impl->mixer->remove(sound->getStream().get());
	if (auto i = std::find(impl->sounds.begin(), impl->sounds.end(), sound);
	    i != impl->sounds.end()) {
		impl->sounds.erase(i);
	}
}

void Channel::stopAll() {
	auto newMixer = std::make_shared<Mixer>();
	impl->volumeControl->replaceStream(newMixer);
	impl->mixer = newMixer.get();
	impl->sounds.clear();
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
