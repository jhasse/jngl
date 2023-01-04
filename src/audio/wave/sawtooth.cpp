#include "wave/sawtooth.hpp"
#include "wave/generator.hpp"
#include "oscillator.hpp"
#include <psemek/log/log.hpp>

#include <cmath>

namespace psemek::audio
{

	stream_ptr sawtooth_wave(float frequency)
	{
		auto func = [o = oscillator{frequency}]() mutable {
			auto z = o.next();
			return (2.f / geom::pi) * (- std::atan2(z.real() + 1.f, z.imag()));
		};

		return make_generator(func);
	}

}
