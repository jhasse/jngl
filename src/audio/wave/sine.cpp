#include "wave/sine.hpp"
#include "wave/generator.hpp"
#include "oscillator.hpp"

#include <cmath>
#include <complex>

namespace psemek::audio
{

	stream_ptr sine_wave(float frequency)
	{
		auto func = [o = oscillator{frequency}]() mutable {
			return o.next().imag();
		};

		return make_generator(func);
	}

}
