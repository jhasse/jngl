#include "volume.hpp"
#include "volume_base.hpp"

namespace psemek::audio {

namespace {

struct volume_control_impl : volume_control {
	volume_control_impl(stream_ptr stream, float gain, float smoothness)
	: base_(gain, gain, smoothness), stream_(std::move(stream)) {
	}

	float gain() const override {
		return base_.gain_left();
	}
	float gain(float value) override {
		base_.gain_left(value);
		return base_.gain_right(value);
	}

	float smoothness() const override {
		return base_.smoothness();
	}
	float smoothness(float value) override {
		return base_.smoothness(value);
	}

	std::optional<std::size_t> length() const override {
		return stream_->length();
	}

	std::size_t played() const override {
		return stream_->played();
	}

	std::size_t read(float* data, std::size_t sample_count) override {
		auto result = stream_->read(data, sample_count);
		base_.apply(data, result);
		return result;
	}

private:
	volume_base base_;
	stream_ptr stream_;
};

struct volume_control_stereo_impl : volume_control_stereo {
	volume_control_stereo_impl(stream_ptr stream, float gain_left, float gain_right,
	                           float smoothness)
	: base_(gain_left, gain_right, smoothness), stream_(std::move(stream)) {
	}

	float gain_left() const override {
		return base_.gain_left();
	}
	float gain_right() const override {
		return base_.gain_right();
	}
	float gain_left(float value) override {
		return base_.gain_left(value);
	}
	float gain_right(float value) override {
		return base_.gain_right(value);
	}

	float smoothness() const override {
		return base_.smoothness();
	}
	float smoothness(float value) override {
		return base_.smoothness(value);
	}

	std::optional<std::size_t> length() const override {
		return stream_->length();
	}

	std::size_t played() const override {
		return stream_->played();
	}

	std::size_t read(float* data, std::size_t sample_count) override {
		auto result = stream_->read(data, sample_count);
		base_.apply(data, result);
		return result;
	}

private:
	volume_base base_;
	stream_ptr stream_;
};

} // namespace

std::shared_ptr<volume_control> volume(stream_ptr stream, float gain, float smoothness) {
	return std::make_shared<volume_control_impl>(std::move(stream), gain, smoothness);
}

std::shared_ptr<volume_control_stereo> volume_stereo(stream_ptr stream, float gain_left,
                                                     float gain_right, float smoothness) {
	return std::make_shared<volume_control_stereo_impl>(std::move(stream), gain_left, gain_right,
	                                                    smoothness);
}

} // namespace psemek::audio
