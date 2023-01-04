#pragma once

#include "stream.hpp"

#include <atomic>
#include <memory>

namespace psemek::audio
{

	struct channel
	{
		channel() = default;

		channel(stream_ptr stream)
			: stream_(std::move(stream))
		{}

		stream_ptr stream() const
		{
			return std::atomic_load(&stream_);
		}

		stream_ptr stream(stream_ptr new_stream)
		{
			return std::atomic_exchange(&stream_, std::move(new_stream));
		}

		stream_ptr stop()
		{
			return stream(nullptr);
		}

		bool is_stopped() const
		{
			return stream() != nullptr;
		}

	private:
		stream_ptr stream_;
	};

	using channel_ptr = std::shared_ptr<channel>;

}
