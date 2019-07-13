// Copyright 2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Sound.hpp"

#include "SoundParams.hpp"
#include "jngl/debug.hpp"

namespace jngl {

struct Sound::Impl {
	ALuint buffer = 0;
	ALuint source = 0;
};

float Sound::masterVolume = 1.0f;

void checkAlError() {
	switch (alGetError()) {
		case AL_NO_ERROR:
			break;
		case AL_INVALID_NAME:
			debugLn("Invalid name paramater passed to AL call.");
			break;
		case AL_INVALID_ENUM:
			debugLn("Invalid enum parameter passed to AL call.");
			break;
		case AL_INVALID_VALUE:
			debugLn("Invalid value parameter passed to AL call.");
			break;
		case AL_INVALID_OPERATION:
			debugLn("Illegal AL call.");
			break;
		case AL_OUT_OF_MEMORY:
			debugLn("Not enough memory.");
			break;
		default:
			debugLn("Unknown OpenAL error.");
	}
}

Sound::Sound(const SoundParams& params, std::vector<char>& bufferData)
: impl(std::make_unique<Impl>()) {
	alGenBuffers(1, &impl->buffer);
	checkAlError();
	alGenSources(1, &impl->source);
	checkAlError();
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	checkAlError();
	alSource3f(impl->source, AL_POSITION, 0.0f, 0.0f, 0.0f);
	checkAlError();
	alBufferData(impl->buffer, params.format, &bufferData[0],
	             static_cast<ALsizei>(bufferData.size()), params.freq);
	checkAlError();
	alSourcei(impl->source, AL_BUFFER, impl->buffer);
	checkAlError();
	alSourcePlay(impl->source);
	checkAlError();
	setVolume(masterVolume);
}

Sound::~Sound() {
	debug("freeing sound buffer ... ");
	alSourceStop(impl->source);
	checkAlError();
	ALint processedBuffers = 0;
	alGetSourcei(impl->source, AL_BUFFERS_PROCESSED, &processedBuffers);
	checkAlError();
	if (processedBuffers == 1) {
		alSourceUnqueueBuffers(impl->source, processedBuffers, &impl->buffer);
		checkAlError();
	}
	alDeleteSources(1, &impl->source);
	checkAlError();
	alDeleteBuffers(1, &impl->buffer);
	checkAlError();
	debug("OK\n");
}

bool Sound::IsPlaying() {
	ALint state;
	alGetSourcei(impl->source, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}

void Sound::loop() {
	alSourcei(impl->source, AL_LOOPING, AL_TRUE);
}

bool Sound::isStopped() const {
	ALint state;
	alGetSourcei(impl->source, AL_SOURCE_STATE, &state);
	return state == AL_STOPPED;
}

void Sound::SetPitch(float p) {
	alSourcef(impl->source, AL_PITCH, p);
}

void Sound::setVolume(float v) {
	alSourcef(impl->source, AL_GAIN, v);
}

} // namespace jngl