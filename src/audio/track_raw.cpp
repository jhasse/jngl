#include "Track.hpp"

#include "Stream.hpp"

#include <atomic>
#include <stdexcept>

namespace jngl {

namespace {

struct data_holder {
	std::vector<float> storage;
	gsl::span<float const> samples;

	explicit data_holder(gsl::span<float const> samples) : samples(samples) {
	}

	explicit data_holder(std::vector<float> storage)
	: storage(std::move(storage)), samples(this->storage) {
	}
};

struct raw_stream_impl : Stream {
	explicit raw_stream_impl(std::shared_ptr<data_holder> data_holder)
	: data_holder_(std::move(data_holder)) {
	}

	std::size_t read(float* data, std::size_t sample_count) override {
		auto played = played_.load();

		auto count = std::min(data_holder_->samples.size() - played, sample_count);
		std::copy(data_holder_->samples.begin() + played,
		          data_holder_->samples.begin() + played + count, data);
		played_.fetch_add(count);
		return count;
	}

	void rewind() override {
		played_ = 0;
	}

private:
	std::shared_ptr<data_holder> data_holder_;
	std::atomic<std::size_t> played_{ 0 };
};

struct raw_track_impl : Track {
	explicit raw_track_impl(std::shared_ptr<data_holder> data_holder)
	: data_holder_(std::move(data_holder)) {
	}

	std::shared_ptr<Stream> stream() const override {
		return std::make_shared<raw_stream_impl>(data_holder_);
	}

	std::optional<std::size_t> length() const override {
		return data_holder_->samples.size();
	}

private:
	std::shared_ptr<data_holder> data_holder_;
};

} // namespace

std::shared_ptr<Track> load_raw(gsl::span<float const> samples) {
	if ((samples.size() % 2) != 0) {
		throw std::runtime_error("bad sample count");
	}
	return std::make_shared<raw_track_impl>(std::make_shared<data_holder>(samples));
}

std::shared_ptr<Track> load_raw(std::vector<float> samples) {
	if ((samples.size() % 2) != 0) {
		throw std::runtime_error("bad sample count");
	}
	return std::make_shared<raw_track_impl>(std::make_shared<data_holder>(std::move(samples)));
}

} // namespace jngl
