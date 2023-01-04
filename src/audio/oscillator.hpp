#pragma once

#include "constants.hpp"
#include <psemek/geom/constants.hpp>

#include <complex>

namespace psemek::audio
{

	struct oscillator
	{
		oscillator(float f)
		{
			frequency(f);
		}

		void frequency(float f)
		{
			m_ = std::exp(std::complex<float>{0.f, 2.f * geom::pi * f * inv_frequency});
		}

		std::complex<float> phase() const
		{
			return phase_;
		}

		std::complex<float> next()
		{
			phase_ *= m_;
			phase_ /= std::abs(phase_);
			return phase_;
		}

	private:
		std::complex<float> phase_{1.f, 0.f};
		std::complex<float> m_;
	};

}
