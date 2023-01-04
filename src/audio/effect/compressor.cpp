#include "compressor.hpp"
#include "../smooth.hpp"
/*
#include <psemek/geom/math.hpp>
#include <psemek/geom/interval.hpp>
#include <psemek/geom/contains.hpp>

namespace psemek::audio
{

	namespace
	{

		struct compressor_impl
			: stream
		{
			compressor_impl(stream_ptr stream, float volume_threshold, float strength, float envelope_attack, float envelope_release, float knee)
				: stream_(std::move(stream))
				, volume_threshold_log_(std::log(volume_threshold))
				, strength_(strength)
				, envelope_attack_multiplier_(smoothness_to_multiplier(envelope_attack))
				, envelope_release_multiplier_(smoothness_to_multiplier(envelope_release))
				, knee_range_(geom::expand(geom::interval<float>::singleton(volume_threshold_log_), std::log(knee) * 0.5f))
			{}

			std::optional<std::size_t> length() const override
			{
				return stream_->length();
			}

			std::size_t read(float * data, std::size_t sample_count) override
			{
				auto result = stream_->read(data, sample_count);

				for (std::size_t i = 0; i < result; i += 2)
				{
					float v = std::max(std::abs(data[i]), std::abs(data[i + 1]));

					float multiplier = (v > envelope_) ? envelope_attack_multiplier_ : envelope_release_multiplier_;

					envelope_ += (v - envelope_) * multiplier;

					float strength = strength_;

					float envelope_log = std::log(envelope_);

					if (geom::contains(knee_range_, envelope_log))
						strength *= geom::unlerp(knee_range_, envelope_log);

					float log_gain = strength * std::min(0.f, volume_threshold_log_ - envelope_log);

					float gain = std::exp(log_gain);

					data[i + 0] *= gain;
					data[i + 1] *= gain;
				}

				return result;
			}

			std::size_t played() const override
			{
				return stream_->played();
			}

		private:
			stream_ptr stream_;
			float volume_threshold_log_;
			float strength_;
			float envelope_attack_multiplier_;
			float envelope_release_multiplier_;
			geom::interval<float> knee_range_;
			float envelope_ = 0.f;
		};

	}

	stream_ptr compressor(stream_ptr stream, float volume_threshold, float strength, float envelope_attack, float envelope_release, float knee)
	{
		return std::make_shared<compressor_impl>(std::move(stream), volume_threshold, strength, envelope_attack, envelope_release, knee);
	}

	stream_ptr limiter(stream_ptr stream, float volume_threshold, float envelope_attack, float envelope_release, float knee)
	{
		return compressor(std::move(stream), volume_threshold, 1.f, envelope_attack, envelope_release, knee);
	}

}
*/
