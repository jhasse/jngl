#include "stereo.hpp"

#include <vector>

namespace psemek::audio
{

	namespace
	{

		struct stereo_impl
			: stream
		{
			stereo_impl(stream_ptr left, stream_ptr right)
				: left_(std::move(left))
				, right_(std::move(right))
			{}

			std::optional<std::size_t> length() const override
			{
				auto left = left_->length();
				auto right = right_->length();

				if (left && right)
					return std::min(*left, *right);
				else if (left)
					return left;
				else
					return right;
			}

			std::size_t played() const override
			{
				return std::min(left_->played(), right_->played());
			}

			std::size_t read(float * data, std::size_t sample_count) override
			{
				if (buffer_.size() < sample_count)
					buffer_.resize(sample_count);

				auto right_result = right_->read(data, sample_count);
				auto left_result = left_->read(buffer_.data(), sample_count);

				auto result = std::min(right_result, left_result);

				{
					auto begin = buffer_.data();
					auto end = buffer_.data() + result;
					auto dst = data;
					for (; begin < end; begin += 2, dst += 2)
						*dst = *begin;
				}
				return result;
			}

		private:
			stream_ptr left_, right_;
			std::vector<float> buffer_;
		};

	}

	stream_ptr stereo(stream_ptr left, stream_ptr right)
	{
		return std::make_shared<stereo_impl>(std::move(left), std::move(right));
	}

}
