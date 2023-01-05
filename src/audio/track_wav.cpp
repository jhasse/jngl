#include "track.hpp"
#include "duplicate.hpp"
#include "constants.hpp"
#include "../jngl/Finally.hpp"

#include <SDL.h>
#include <stdexcept>
#include <string>
#include <cassert>

namespace psemek::audio
{

	namespace
	{

		std::vector<float> convert_audio(SDL_AudioSpec const & spec, std::uint8_t * samples, std::size_t length)
		{
			if (spec.channels > 2)
				throw std::runtime_error("Can't convert audio with " + std::to_string(static_cast<int>(spec.channels)) + " channels");

			if (spec.freq != audio::frequency)
				throw std::runtime_error("Can't convert audio with frequency " + std::to_string(spec.freq));

			if (spec.format != AUDIO_S16SYS)
				throw std::runtime_error("Can't convert audio with format " + std::to_string(spec.format));

			auto p = reinterpret_cast<std::int16_t *>(samples);

			std::vector<float> result;

			if (spec.channels == 1)
			{
				result.resize(length);
				for (std::size_t i = 0; i < length / 2; ++i)
				{
					float v = (p[i] * 2.f + 1.f) / 65536.f;
					result[2 * i + 0] = v;
					result[2 * i + 1] = v;
				}
			}
			else
			{
				result.resize(length / 2);
				for (std::size_t i = 0; i < length / 2; ++i)
					result[i] = (p[i] * 2.f + 1.f) / 65536.f;
			}

			return result;
		}

	}

	track_ptr load_wav(std::span<char const> data)
	{
		SDL_AudioSpec spec;
		std::uint8_t * samples;
		std::uint32_t length;
		if (!SDL_LoadWAV_RW(SDL_RWFromConstMem(data.data(), data.size()), 1, &spec, &samples, &length)) {
			assert(false);
			// sdl2::fail("SDL_LoadWAV_RW failed:"); FIXME
		}

		jngl::Finally release_samples([samples]{ SDL_FreeWAV(samples); });
		return make_duplicator(make_recorder(convert_audio(spec, samples, length)));
	}

	track_ptr load_wav(std::vector<char> const & data)
	{
		return load_wav(std::span<char const>(data));
	}

}
