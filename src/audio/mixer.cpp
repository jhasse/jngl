#include "mixer.hpp"

#include "readerwriterqueue/readerwriterqueue.h"

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
			void add(stream_ptr stream) override;

			void remove(stream* stream) override {
                gc();
				streamsToStop.enqueue(stream);
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
            void gc() {
                stream* tmp;
                while (streamsToRemoveOnMainThread.try_dequeue(tmp)) {
                    auto it = std::find_if(streamsOnMainThread.begin(), streamsOnMainThread.end(),
                                           [tmp](const auto& stream) {
                        return stream.get() == tmp; });
                    assert(it != streamsOnMainThread.end());
                    streamsOnMainThread.erase(it);
                }
            }
            std::vector<std::shared_ptr<stream>> streamsOnMainThread;
            
            moodycamel::ReaderWriterQueue<stream*> streamsToRemoveOnMainThread;
            
            std::vector<stream*> activeStreams; //< only used on mixer thread

			std::vector<float> buffer_;

            moodycamel::ReaderWriterQueue<stream*> newStreams;

            moodycamel::ReaderWriterQueue<stream*> streamsToStop;

			std::atomic<std::size_t> played_{0};
		};

		void mixer_impl::add(stream_ptr stream)
		{
            gc();
			newStreams.enqueue(stream.get());
            streamsOnMainThread.emplace_back(std::move(stream));
		}

		std::size_t mixer_impl::read(float * data, std::size_t sample_count)
		{
            stream* tmp;
            while (newStreams.try_dequeue(tmp)) {
                activeStreams.emplace_back(tmp);
            }
            while (streamsToStop.try_dequeue(tmp)) {
                auto it = std::find(activeStreams.begin(), activeStreams.end(), tmp);
                assert(it != activeStreams.end());
                activeStreams.erase(it);
                streamsToRemoveOnMainThread.enqueue(tmp);
            }

			std::fill(data, data + sample_count, 0.f);

			buffer_.resize(sample_count);

            for (auto it = activeStreams.begin(); it != activeStreams.end();)
			{
				auto& stream = *it;
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
                    streamsToRemoveOnMainThread.enqueue(*it);
                    it = activeStreams.erase(it);
                } else {
                    ++it;
                }
			}

			played_.fetch_add(sample_count);

			return sample_count;
		}

	}

	mixer_ptr make_mixer()
	{
		return std::make_shared<mixer_impl>();
	}

}
