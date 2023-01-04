#pragma once

#include "stream.hpp"
#include "constants.hpp"

#include <atomic>

namespace psemek::audio
{

	template <typename Func>
	struct generator_stream
		: stream
	{
		generator_stream(Func func)
			: func_(std::move(func))
		{}

		std::optional<std::size_t> length() const override
		{
			return std::nullopt;
		}

		std::size_t played() const override
		{
			return played_.load();
		}

		std::size_t read(float * data, std::size_t sample_count) override
		{
			auto end = data + sample_count;
			for (auto p = data; p != end;)
			{
				float v = func_();
				*p++ = v;
				*p++ = v;
			}
			played_.fetch_add(sample_count);
			return sample_count;
		}

	private:
		Func func_;
		std::atomic<std::size_t> played_{0};
	};

	template <typename Func>
	auto make_generator(Func && func)
	{
		return std::make_shared<generator_stream<std::decay_t<Func>>>(std::forward<Func>(func));
	}

}
