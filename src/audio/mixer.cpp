#include "mixer.hpp"

#include <algorithm>
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>

namespace psemek::audio
{

	namespace
	{

		struct mixer_impl final
			: mixer
			, std::enable_shared_from_this<mixer_impl>
		{
			channel_ptr add(stream_ptr stream) override;

			void remove(stream* stream) override {
				std::lock_guard lock(streamsToStopMutex);
				streamsToStop.push_back(stream);
			}

			std::size_t read(float * data, std::size_t sample_count) override;

			std::optional<std::size_t> length() const override
			{
				return std::nullopt;
			}

			std::size_t played() const override
			{
				return played_.load();
			}

		private:
			std::vector<channel_ptr> channels_;
			std::vector<channel_ptr> alive_channels_;

			std::vector<float> buffer_;

			std::mutex new_channels_mutex_;
			std::vector<channel_ptr> new_channels_;

			std::mutex streamsToStopMutex;
			std::vector<stream*> streamsToStop;

			std::atomic<std::size_t> played_{0};
		};

		channel_ptr mixer_impl::add(stream_ptr stream)
		{
			auto result = std::make_shared<channel>(std::move(stream));

			{
				std::lock_guard lock{new_channels_mutex_};
				new_channels_.push_back(result);
			}

			return result;
		}

		std::size_t mixer_impl::read(float * data, std::size_t sample_count)
		{
			{
				std::vector<channel_ptr> new_channels;
				{
					std::lock_guard lock{new_channels_mutex_};
					new_channels = std::move(new_channels_);
				}
				for (auto & ch : new_channels)
					channels_.push_back(std::move(ch));

				std::vector<stream*> streamsToStop;
				{
					std::lock_guard lock{streamsToStopMutex};
					streamsToStop = std::move(this->streamsToStop);
				}
				for (stream* const stream : streamsToStop) {
					const auto it = std::find_if(channels_.begin(), channels_.end(),
					[stream](const auto& channel) {
						return channel->hasStream(stream);
					});
					if (it != channels_.end()) {
						(*it)->stop();
					}
				}
			}

			std::fill(data, data + sample_count, 0.f);

			buffer_.resize(sample_count);

			for (auto & ch : channels_)
			{
				auto stream = ch->stream();
				if (!stream)
					continue;

				auto read = stream->read(buffer_.data(), sample_count);

				{
					auto begin = buffer_.data();
					auto end = begin + read;
					auto dst = data;
					for (; begin < end; )
						*dst++ += *begin++;
				}

				if (read < sample_count)
				{
					ch->stop();
					continue;
				}

				alive_channels_.push_back(std::move(ch));
			}

			std::swap(channels_, alive_channels_);
			alive_channels_.clear();

			played_.fetch_add(sample_count);

			return sample_count;
		}

	}

	mixer_ptr make_mixer()
	{
		return std::make_shared<mixer_impl>();
	}

}
