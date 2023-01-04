#include "wave/triangle.hpp"
#include "wave/generator.hpp"
#include "oscillator.hpp"

#include <cmath>

namespace psemek::audio
{

	stream_ptr triangle_wave(float frequency)
	{
		auto func = [o = oscillator{frequency}]() mutable {
			auto const z = o.next();
			float const t = 0.5f * std::atan2(z.imag(), z.real()) / (geom::pi) + 0.5f;
			return 4.f * std::abs(t - std::floor(t + 0.5f)) - 1.f;
		};

		return make_generator(func);
	}

}
