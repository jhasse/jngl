#include "wave/karplus_strong.hpp"
#include "constants.hpp"
#include <psemek/random/generator.hpp>
#include <psemek/random/uniform.hpp>

#include <vector>
#include <atomic>

namespace psemek::audio
{

	namespace
	{

		struct karplus_strong_impl
			: stream
		{
			karplus_strong_impl(float frequency)
				: buffer_(2 * std::round(audio::frequency / frequency))
			{
				random::generator rng{0x4b0a763ef6573bf2ull, 0};
				for (std::size_t i = 0; i < buffer_.size(); i += 2)
				{
					float v = random::uniform(rng, -1.f, 1.f);
					buffer_[i + 0] = v;
					buffer_[i + 1] = v;
				}
			}

			std::optional<std::size_t> length() const override
			{
				return std::nullopt;
			}

			std::size_t read(float * data, std::size_t sample_count) override
			{
				std::size_t const size = buffer_.size();

				for (std::size_t i = 0; i < sample_count; i += 2)
				{
					buffer_[buffer_pos_ + 0] = (buffer_[buffer_pos_ + 0] + buffer_[(buffer_pos_ + 2) % size + 0]) / 2.f;
					buffer_[buffer_pos_ + 1] = (buffer_[buffer_pos_ + 1] + buffer_[(buffer_pos_ + 2) % size + 1]) / 2.f;

					data[i + 0] = buffer_[buffer_pos_ + 0];
					data[i + 1] = buffer_[buffer_pos_ + 1];

					buffer_pos_ += 2;
					buffer_pos_ %= size;
				}

				return sample_count;
			}

			std::size_t played() const override
			{
				return played_.load();
			}

		private:
			std::vector<float> buffer_;
			std::size_t buffer_pos_{0};

			std::atomic<std::size_t> played_;
		};

	}

	stream_ptr karplus_strong(float frequency)
	{
		return std::make_shared<karplus_strong_impl>(frequency);
	}

}
