#include "engine.hpp"
#include "constants.hpp"

#include <SDL.h>

#include <cassert>
#include <mutex>
#include <atomic>
#include <vector>
#include <optional>
#include <cmath>

namespace psemek::audio
{

	struct engine::impl
	{
		std::shared_ptr<void> sdl_init;

		SDL_AudioDeviceID device;

		std::vector<float> buffer;
		bool thread_registered = false;

		channel_ptr output;

		impl();
		~impl();

		static void callback(void * userdata, std::uint8_t * stream, int len);
	};

	engine::impl::impl()
		// : sdl_init(sdl2::init(SDL_INIT_AUDIO)) FIXME
	{
		SDL_AudioSpec desired, obtained;
		desired.freq = frequency;
		desired.channels = 2;
		desired.format = AUDIO_S16SYS;
		desired.samples = 256;
		desired.callback = &callback;
		desired.userdata = this;
		if (device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0); device == 0) {
			assert(false);
			// sdl2::fail("SDL_OpenAudioDevice failed:"); TODO
		}

		// log::info() << "Initialized audio: " << static_cast<int>(obtained.channels) << " channels, " << obtained.freq << " Hz, " << obtained.samples << " samples";

		buffer.resize(obtained.samples * obtained.channels);
		output = std::make_shared<channel>();
		SDL_PauseAudioDevice(device, 0);
	}

	engine::impl::~impl()
	{
		SDL_CloseAudioDevice(device);
	}

	void engine::impl::callback(void * userdata, std::uint8_t * dst_u8, int len)
	{
		static std::string const profiler_str = "audio";
		// prof::profiler prof(profiler_str);

		auto self = static_cast<impl *>(userdata);
		stream_ptr output = self->output->stream();
		std::int16_t * dst = reinterpret_cast<std::int16_t *>(dst_u8);

		if (!self->thread_registered)
		{
			// log::register_thread("audio");
			self->thread_registered = true;
		}

		std::size_t const size = len / 2;
		std::size_t read = 0;
		if (output)
			read = output->read(self->buffer.data(), size);
		std::fill(self->buffer.data() + read, self->buffer.data() + size, 0.f);

		for (auto s : self->buffer)
			*dst++ = static_cast<std::int16_t>(std::max(std::min((65535.f * s - 1.f) / 2.f, 32767.f), -32768.f));
	}

	engine::engine()
		: pimpl_(make_impl())
	{}

	engine::~engine() = default;

	channel_ptr engine::output()
	{
		return impl().output;
	}

}
