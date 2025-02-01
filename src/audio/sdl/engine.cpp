// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#include "../engine.hpp"

#include "../../jngl/other.hpp"
#include "../../log.hpp"
#include "../Stream.hpp"
#include "../constants.hpp"

#include <SDL3/SDL.h>

#include <cassert>
#include <vector>

namespace jngl::audio {

struct engine::Impl {
	explicit Impl(std::shared_ptr<Stream> output) {
		try {
			backend = std::make_unique<SdlImpl>(output);
		} catch (std::exception& e) {
			internal::warn(e.what());
			backend = std::make_unique<DummyImpl>(std::move(output));
		}
	}
	struct Backend {
		virtual ~Backend() = default;
		virtual void setPause(bool) = 0;
		virtual void step() {}
	};
	std::unique_ptr<Backend> backend;

	struct DummyImpl : public Backend {
		explicit DummyImpl(std::shared_ptr<Stream> output)
		: output(std::move(output)) {
		}
		void setPause(bool pause) override {
			this->pause = pause;
		}
		void step() override {
			if (pause)  { return; }
			size_t size = frequency * 2 / getStepsPerSecond();
			auto buffer = std::make_unique<float[]>(size);
			[[maybe_unused]] const auto read = output->read(buffer.get(), size);
			assert(read == size);
			// to debug sound on the terminal:
			// float average = 0;
			// for (size_t i = 0; i < size; ++i) {
			// 	average += std::fabs(buffer[i] / static_cast<float>(size));
			// }
			// if (average < 0.1f) {
			// 	debug(' ');
			// } else if (average < 0.2f) {
			// 	debug("▁");
			// } else if (average < 0.3f) {
			// 	debug("▂");
			// } else if (average < 0.4f) {
			// 	debug("▃");
			// } else if (average < 0.5f) {
			// 	debug("▄");
			// } else if (average < 0.6f) {
			// 	debug("▅");
			// } else if (average < 0.7f) {
			// 	debug("▆");
			// } else if (average < 0.8f) {
			// 	debug("▇");
			// } else {
			// 	debug("█");
			// }
		}

		std::shared_ptr<Stream> output;
		bool pause{ false };
	};

	struct SdlImpl : public Backend {
		std::shared_ptr<void> sdl_init;

		SDL_AudioStream* device = nullptr;

		std::vector<float> buffer;

		std::shared_ptr<Stream> output;

		explicit SdlImpl(std::shared_ptr<Stream> output) : output(std::move(output)) {
			if (!SDL_Init(SDL_INIT_AUDIO)) {
				throw std::runtime_error(SDL_GetError());
			}
			SDL_AudioSpec spec;
			SDL_zero(spec);
			spec.freq = frequency;
			spec.channels = 2;
			spec.format = SDL_AUDIO_F32;
			if (device = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec,
			                                       &callback, this);
			    !device) {
				throw std::runtime_error(SDL_GetError());
			}

			internal::debug("Initialized audio: {} channels, {} Hz, {} samples", spec.channels,
			                spec.freq, spec.freq);

			SDL_ResumeAudioStreamDevice(device);
		}
		~SdlImpl() override {
			SDL_DestroyAudioStream(device);
		}

		static void callback(void* userdata, SDL_AudioStream* stream, int additionalAmount, int totalAmount) {
			auto self = static_cast<SdlImpl*>(userdata);

			self->buffer.resize(additionalAmount);
			std::size_t const size = additionalAmount;
			std::size_t read = 0;
			read = self->output->read(self->buffer.data(), size);
			std::fill(self->buffer.data() + read, self->buffer.data() + size, 0.f);

			SDL_PutAudioStreamData(stream, self->buffer.data(), additionalAmount * sizeof(float));
		}

		void setPause(bool pause) override {
			if (pause) {
				SDL_PauseAudioStreamDevice(device);
			} else {
				SDL_ResumeAudioStreamDevice(device);
			}
		}
	};
};

engine::engine(std::shared_ptr<Stream> output) : impl(std::make_unique<Impl>(std::move(output))) {
}

engine::~engine() = default;

void engine::setPause(bool pause) {
	impl->backend->setPause(pause);
}

void engine::step() {
	impl->backend->step();
}

} // namespace jngl::audio
