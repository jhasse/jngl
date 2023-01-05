#include "track.hpp"
#include "resampler.hpp"
#include "constants.hpp"

#include <vorbis/vorbisfile.h>

#include <atomic>
#include <cassert>
#include <cstring>
#include <stdexcept>

namespace psemek::audio {

namespace {

struct ogg_data_holder {
	std::vector<char> storage;
	std::span<char const> data;
	int32_t pos = 0;

	ogg_data_holder(std::vector<char> storage) : storage(std::move(storage)), data(this->storage) {
	}

	ogg_data_holder(std::span<char const> data) : data(data) {
	}
};

static size_t byteRead(void* ptr, size_t size, size_t nmemb, void* datasource) {
	ogg_data_holder& data = *reinterpret_cast<ogg_data_holder*>(datasource);
	size_t bytesToRead = size * nmemb;
	if (data.pos + bytesToRead > data.data.size()) {
		bytesToRead = data.data.size() - data.pos;
	}
	std::memcpy(ptr, data.data.data() + data.pos, bytesToRead);
	data.pos += bytesToRead;
	return bytesToRead;
}

static int bytesSeek(void* datasource, ogg_int64_t offset, int whence) {
	ogg_data_holder& data = *reinterpret_cast<ogg_data_holder*>(datasource);
	switch (whence) {
	case SEEK_CUR:
		data.pos += offset;
		break;
	case SEEK_END:
		data.pos = data.data.size() - offset;
		break;
	case SEEK_SET:
		data.pos = offset;
		break;
	default:
		return -1;
	}
	if (data.pos < 0) {
		data.pos = 0;
		return -1;
	}
	if (data.pos > data.data.size()) {
		data.pos = data.data.size();
		return -1;
	}
	return 0;
}

static int bytesClose(void*) {
	return 0;
}

static long bytesTell(void* datasource) {
	return reinterpret_cast<ogg_data_holder*>(datasource)->pos;
}

struct ogg_stream_impl : stream {
	ogg_stream_impl(std::shared_ptr<ogg_data_holder> data)
	: data_(std::move(data)), source_buffer_(1152 * 200) // TODO: how big has this to be for ogg?
	{
		if (ov_open_callbacks(this->data_.get(), &oggFile, nullptr, 0,
		                      ov_callbacks{ byteRead, bytesSeek, bytesClose, bytesTell }) != 0) {
			throw std::runtime_error("Could not open OGG file.");
		}

		const vorbis_info* const pInfo = ov_info(&oggFile, -1);
		mono_ = pInfo->channels == 1;
		resampler_.ratio(static_cast<float>(frequency) / pInfo->rate);

		// TODO: free pInfo?
	}

	std::optional<std::size_t> length() const override {
		return played_.load();
	}

	std::size_t read(float* data, std::size_t sample_count) override {
		auto input = reinterpret_cast<std::uint8_t const*>(data_->data.data());

		std::size_t result = 0;
		while (result < sample_count) {
			if (resampler_pos_ < resampler_.result().size()) {
				std::size_t size =
				    std::min(resampler_.result().size() - resampler_pos_, sample_count - result);
				std::copy(resampler_.result().data() + resampler_pos_,
				          resampler_.result().data() + resampler_pos_ + size, data + result);
				resampler_pos_ += size;
				result += size;
			} else {

				const int endian = 0;                 // 0 for Little-Endian, 1 for Big-Endian
				float** buffer = nullptr;
				auto samples_read = ov_read_float(&oggFile, &buffer, 1024, &bitStream);
				assert(samples_read >= 0);
				if (samples_read == 0) {
					break;
				}
				resampler_pos_ = 0;
				read_bytes_ += samples_read;

				for (std::size_t i = samples_read; i > 0;) {
					i -= 1;
					auto tmp = buffer[0][i];
					source_buffer_[i * 2 + 0] = tmp;
					source_buffer_[i * 2 + 1] = buffer[mono_ ? 0 : 1][i];
				}

				resampler_.feed(
				    { source_buffer_.data(), source_buffer_.data() + samples_read * 2 });
			}
		}

		played_.fetch_add(result);

		return result;
	}

	std::size_t played() const override {
		return played_.load();
	}

private:
	std::shared_ptr<ogg_data_holder> data_;
	std::size_t read_bytes_{ 0 };


	OggVorbis_File oggFile;
	std::vector<float> source_buffer_;
	bool mono_;
	int bitStream;

	resampler resampler_;
	std::size_t resampler_pos_{ 0 };

	std::atomic<std::size_t> played_{ 0 };
};

struct ogg_track_impl : track {
	ogg_track_impl(std::shared_ptr<ogg_data_holder> data) : data_(std::move(data)) {
	}

	stream_ptr stream() const override {
		return std::make_shared<ogg_stream_impl>(data_);
	}

	std::optional<std::size_t> length() const override {
		return std::nullopt;
	}

private:
	std::shared_ptr<ogg_data_holder> data_;
};

} // namespace

track_ptr load_ogg(std::span<char const> data) {
	return std::make_shared<ogg_track_impl>(std::make_shared<ogg_data_holder>(data));
}

track_ptr load_ogg(std::vector<char> data) {
	return std::make_shared<ogg_track_impl>(std::make_shared<ogg_data_holder>(std::move(data)));
}

} // namespace psemek::audio
