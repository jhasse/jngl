// Copyright 2023-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#include "../engine.hpp"

#include "../../jngl/other.hpp"
#include "../../jngl/record/VideoRecorder.hpp"
#include "../../log.hpp"
#include "../Stream.hpp"
#include "../constants.hpp"

#include <SDL.h>

#include <cassert>
#include <vector>

namespace jngl::audio {

struct engine::Impl {
	explicit Impl(std::shared_ptr<Stream> output) {
#ifdef JNGL_RECORD
		if (auto videoRecorder = getJob<VideoRecorder>()) {
			internal::debug("Using dummy audio backend because VideoRecorder is active.");
			backend = std::make_unique<DummyImpl>(std::move(output), std::move(videoRecorder));
			return;
		}
#endif
		try {
			backend = std::make_unique<SdlImpl>(output);
		} catch (std::exception& e) {
			internal::warn(e.what());
			backend = std::make_unique<DummyImpl>(std::move(output), nullptr);
		}
	}
	struct Backend {
		virtual ~Backend() = default;
		virtual void setPause(bool) = 0;
		virtual void step() {}
	};
	std::unique_ptr<Backend> backend;

	struct DummyImpl : public Backend {
		explicit DummyImpl(std::shared_ptr<Stream> output,
		                   const std::shared_ptr<VideoRecorder>& videoRecorder)
		: output(std::move(output)), videoRecorder(videoRecorder) {
		}
		void setPause(bool pause) override {
			this->pause = pause;
		}
		void step() override {
			if (pause) {
				return;
			}
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
			if (auto videoRecorderShared = videoRecorder.lock()) {
				videoRecorderShared->fillAudioBuffer(std::move(buffer));
			}
		}

		std::shared_ptr<Stream> output;
		bool pause{ false };

	private:
		std::weak_ptr<VideoRecorder> videoRecorder;
	};

	struct SdlImpl : public Backend {
		std::shared_ptr<void> sdl_init;

		SDL_AudioDeviceID device;

		std::vector<float> buffer;

		std::shared_ptr<Stream> output;

		explicit SdlImpl(std::shared_ptr<Stream> output) : output(std::move(output)) {
			if (SDL_Init(SDL_INIT_AUDIO) < 0) {
				throw std::runtime_error(SDL_GetError());
			}
			SDL_AudioSpec desired;
			SDL_AudioSpec obtained;
			desired.freq = frequency;
			desired.channels = 2;
			desired.format = AUDIO_S16SYS;
#ifdef __EMSCRIPTEN__
			desired.samples = 2048;
#else
			desired.samples = 256;
#endif
			desired.callback = &callback;
			desired.userdata = this;
			if (device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0); device == 0) {
				throw std::runtime_error(SDL_GetError());
			}

			internal::debug("Initialized audio: {} channels, {} Hz, {} samples",
			                static_cast<int>(obtained.channels), obtained.freq, obtained.samples);

			buffer.resize(static_cast<ssize_t>(obtained.samples) * obtained.channels);
			SDL_PauseAudioDevice(device, 0);
		}
		~SdlImpl() override {
			SDL_CloseAudioDevice(device);
		}

		static void callback(void* userdata, std::uint8_t* dst_u8, int len) {
			static std::string const profiler_str = "audio";
			// prof::profiler prof(profiler_str);

			auto self = static_cast<SdlImpl*>(userdata);
			auto* dst = reinterpret_cast<std::int16_t*>(dst_u8); // NOLINT

			std::size_t const size = len / 2;
			std::size_t read = 0;
			read = self->output->read(self->buffer.data(), size);
			std::fill(self->buffer.data() + read, self->buffer.data() + size, 0.f);

			for (auto s : self->buffer) {
				*dst++ = static_cast<std::int16_t>(
				    std::max(std::min((65535.f * s - 1.f) / 2.f, 32767.f), -32768.f));
			}
		}

		void setPause(bool pause) override {
			SDL_PauseAudioDevice(device, pause ? 1 : 0);
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
#ifdef JNGL_RECORD
	if (auto videoRecorder = getJob<VideoRecorder>()) {
		if (dynamic_cast<Impl::SdlImpl*>(impl->backend.get())) {
			// Recording started, swap the backend:
			if (!impl->backend || dynamic_cast<Impl::DummyImpl*>(impl->backend.get()) == nullptr) {
				internal::debug(
				    "Switching to dummy audio backend because VideoRecorder is active.");
				impl->backend = std::make_unique<Impl::DummyImpl>(
				    dynamic_cast<Impl::SdlImpl&>(*impl->backend).output, videoRecorder);
			}
		}
	} else {
		// Recording stopped, swap the backend:
		if (dynamic_cast<Impl::DummyImpl*>(impl->backend.get()) != nullptr) {
			internal::debug("Switching to SDL audio backend because VideoRecorder is inactive.");
			impl->backend = std::make_unique<Impl::SdlImpl>(
			    dynamic_cast<Impl::DummyImpl&>(*impl->backend).output);
		}
	}
#endif
	impl->backend->step();
}

} // namespace jngl::audio
