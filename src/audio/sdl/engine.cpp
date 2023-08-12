#include "../engine.hpp"

#include "../../jngl/debug.hpp"
#include "../constants.hpp"
#include "../Stream.hpp"

#include <SDL.h>

#include <atomic>
#include <cassert>
#include <cmath>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

namespace psemek::audio {

struct engine::Impl {
	explicit Impl(std::shared_ptr<Stream> output) {
		try {
			backend = std::make_unique<SdlImpl>(output);
		} catch (std::exception& e) {
			jngl::debugLn(e.what());
			backend = std::make_unique<DummyImpl>(std::move(output));
		}
	}
	struct Backend {
		virtual ~Backend() = default;
		virtual void setPause(bool) = 0;
	};
	std::unique_ptr<Backend> backend;

	struct DummyImpl : public Backend {
		explicit DummyImpl(std::shared_ptr<Stream> output)
		: output(std::move(output)), thread(
		                                 [this]()
		                                 {
			const int slowdown = 10;
			std::array<float, frequency / slowdown * 2 /* stereo */> buffer;
			while (!quit) {
				do {
					std::this_thread::sleep_for(std::chrono::milliseconds(1000 / slowdown));
				} while (pause && !quit);
				const auto read = this->output->read(buffer.data(), buffer.size());
				assert(read == buffer.size());
				// to debug sound on the terminal:
				// float average = 0;
				// for (float f : buffer) {
				// 	average += std::fabs(f / buffer.size());
				// }
				// if (average < 0.1f) {
				// 	jngl::debug(' ');
				// } else if (average < 0.2f) {
				// 	jngl::debug("▁");
				// } else if (average < 0.3f) {
				// 	jngl::debug("▂");
				// } else if (average < 0.4f) {
				// 	jngl::debug("▃");
				// } else if (average < 0.5f) {
				// 	jngl::debug("▄");
				// } else if (average < 0.6f) {
				// 	jngl::debug("▅");
				// } else if (average < 0.7f) {
				// 	jngl::debug("▆");
				// } else if (average < 0.8f) {
				// 	jngl::debug("▇");
				// } else {
				// 	jngl::debug("█");
				// }
			}
		  }) {
		}
		~DummyImpl() override {
			quit = true;
			thread.join();
		}
		void setPause(bool pause) override {
			this->pause = pause;
		}

		std::shared_ptr<Stream> output;
		std::atomic_bool pause{ false };
		std::atomic_bool quit{ false };
		std::thread thread;
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
			SDL_AudioSpec desired, obtained;
			desired.freq = frequency;
			desired.channels = 2;
			desired.format = AUDIO_S16SYS;
			desired.samples = 256;
			desired.callback = &callback;
			desired.userdata = this;
			if (device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0); device == 0) {
				throw std::runtime_error(SDL_GetError());
			}

			// log::info() << "Initialized audio: " << static_cast<int>(obtained.channels) << "
			// channels, " << obtained.freq << " Hz, " << obtained.samples << " samples";

			buffer.resize(obtained.samples * obtained.channels);
			SDL_PauseAudioDevice(device, 0);
		}
		~SdlImpl() override {
			SDL_CloseAudioDevice(device);
		}

		static void callback(void* userdata, std::uint8_t* dst_u8, int len) {
			static std::string const profiler_str = "audio";
			// prof::profiler prof(profiler_str);

			auto self = static_cast<SdlImpl*>(userdata);
			std::int16_t* dst = reinterpret_cast<std::int16_t*>(dst_u8);

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
} // namespace psemek::audio
