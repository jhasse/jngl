// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Video.hpp"

#ifdef JNGL_VIDEO

#include "../audio.hpp"
#include "../../subprojects/theoraplay/theoraplay.h"
#include "debug.hpp"
#include "time.hpp"

#include <deque>
#include <thread>
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

namespace jngl {

class Video::Impl {
public:
	Impl(const std::string& filename)
	: decoder(THEORAPLAY_startDecodeFile(filename.c_str(), 60, THEORAPLAY_VIDFMT_RGBA)),
	  startTime(jngl::getTime()) {
		if (!decoder) {
			throw std::runtime_error("Failed to start decoding " + filename + "!");
		}

		while (!video) {
			video = THEORAPLAY_getVideo(decoder);
		}
		timePerFrame = 1. / double(video->fps);
		assert(timePerFrame > 0);

		GetAudio();
		alGenSources(1, &source);
		checkAlError();
		alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
		checkAlError();
		alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
		checkAlError();

		while (!audio) {
			audio = THEORAPLAY_getAudio(decoder);
		}

		switch (audio->channels) {
			case 1:
				format = AL_FORMAT_MONO16;
				break;
			case 2:
				format = AL_FORMAT_STEREO16;
				break;
			default:
				throw std::runtime_error("Unsupported number of channels.");
		}
	}

	void draw() {
		double now = jngl::getTime() - startTime;
		if (!video) {
			video = THEORAPLAY_getVideo(decoder);
		}
		if (video and double(video->playms) / 1000. <= now) {
			if (now - double(video->playms) / 1000. >= timePerFrame) {
				// Skip frames to catch up, but keep track of the last one in case we catch up to a
				// series of dupe frames, which means we'd have to draw that final frame and then
				// wait for more.
				const THEORAPLAY_VideoFrame* last = video;
				while ((video = THEORAPLAY_getVideo(decoder)) != nullptr) {
					THEORAPLAY_freeVideo(last);
					last = video;
					jngl::debugLn("skipped frame!");
					if (now - double(video->playms) / 1000. < timePerFrame) { break; }
				}

				if (!video) {
					video = last;
				}
			}
			if (!sprite) {
				sprite = std::make_unique<jngl::Sprite>(video->pixels, video->width, video->height);
			} else {
				sprite->setBytes(video->pixels);
			}
			THEORAPLAY_freeVideo(video);
			video = nullptr;
		}
		if (!audio) {
			audio = THEORAPLAY_getAudio(decoder);
		}
		while (audio) {
			ALint processed;
			alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
			checkAlError();

			assert(processed >= 0);

			if (processed > 0) {
				// Reuse a processed buffer
				alSourceUnqueueBuffers(source, 1, &buffers.front());
				checkAlError();

				queueAudio(buffers.front());
				buffers.pop_front();

				ALint state;
				alGetSourcei(source, AL_SOURCE_STATE, &state);
				if (state != AL_PLAYING) {

					while (true) {
						alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
						checkAlError();
						if (processed == 0) {
							break;
						}
						alSourceUnqueueBuffers(source, 1, &buffers.front());
						checkAlError();
						alDeleteBuffers(1, &buffers.front());
						buffers.pop_front();
					}

					alSourcePlay(source);
					checkAlError();
					jngl::debugLn("WARNING: Audio buffer underrun!");
				}
			} else {
				ALuint buffer;
				alGenBuffers(1, &buffer);
				checkAlError();
				queueAudio(buffer);
				if (buffers.size() == 1) {
					alSourcePlay(source);
					checkAlError();
				}
			}
		}
		if (sprite) {
			sprite->draw();
		}
	}

	~Impl() {
		alSourceStop(source);
		checkAlError();
		for (auto buffer : buffers) {
			alSourceUnqueueBuffers(source, 1, &buffer);
			checkAlError();
			alDeleteBuffers(1, &buffer);
			checkAlError();
		}
		alDeleteSources(1, &source);
		checkAlError();
	}

private:
	void queueAudio(ALuint buffer) {
		auto pcm = std::make_unique<int16_t[]>(audio->frames * audio->channels);
		for (size_t i = 0; i < audio->frames * audio->channels; ++i) {
			const float sample = std::clamp(audio->samples[i], -1.f, 1.f);
			pcm[i] = sample * 32767.f;
		}

		alBufferData(buffer, format, pcm.get(), audio->frames * audio->channels * sizeof(int16_t),
		             audio->freq);
		checkAlError();
		alSourceQueueBuffers(source, 1, &buffer);
		checkAlError();

		buffers.push_back(buffer);

		THEORAPLAY_freeAudio(audio);
		audio = THEORAPLAY_getAudio(decoder);
	}

	std::unique_ptr<jngl::Sprite> sprite;
	std::unique_ptr<jngl::Sound> sound;
	THEORAPLAY_Decoder* decoder;
	const THEORAPLAY_VideoFrame* video = nullptr;
	const THEORAPLAY_AudioPacket* audio = nullptr;
	double startTime;
	double timePerFrame;
	std::deque<ALuint> buffers;
	ALuint source = 0;
	ALenum format;
};

Video::Video(const std::string& filename) : impl(std::make_unique<Impl>(filename)) {
}

Video::~Video() = default;

void Video::draw() const {
	impl->draw();
}

} // namespace jngl

#else

namespace jngl {

class Video::Impl {
};

Video::Video(const std::string&) {
	throw std::runtime_error("JNGL compiled without video support!");
}

Video::~Video() = default;

void Video::draw() const {
}

} // namespace jngl

#endif
