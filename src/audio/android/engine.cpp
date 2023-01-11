#include "../engine.hpp"
#include "../constants.hpp"
#include "../../jngl/debug.hpp"

#include <oboe/Oboe.h>

#include <cassert>
#include <mutex>
#include <atomic>
#include <vector>
#include <cmath>

namespace psemek::audio {

struct engine::impl {
	channel_ptr output;

	impl();
	~impl();

	class Callback : public oboe::AudioStreamCallback {
	public:
		Callback(impl& self) : self(self) {
		}

	private:
		oboe::DataCallbackResult onAudioReady(oboe::AudioStream*, void* data,
		                                      int32_t len) override {
			stream_ptr output = self.output->stream();
			float* dst = reinterpret_cast<float*>(data);
			if (output) {
				output->read(dst, len * 2);
			}
			return oboe::DataCallbackResult::Continue;
		}

		impl& self;
	};

	Callback callback;
	oboe::AudioStreamBuilder builder;
	std::shared_ptr<oboe::AudioStream> oboeStream;
};

engine::impl::impl()
: callback(*this)
{
	builder.setDirection(oboe::Direction::Output);
	builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
	builder.setSharingMode(oboe::SharingMode::Exclusive);
	builder.setFormat(oboe::AudioFormat::Float);
	builder.setChannelCount(oboe::ChannelCount::Stereo);
	builder.setSampleRate(frequency);
	// desired.samples = 256;
	builder.setCallback(&callback);
	const auto result = builder.openStream(oboeStream);
	if (result != oboe::Result::OK) {
		throw std::runtime_error(oboe::convertToText(result));
	}

	output = std::make_shared<channel>();
	oboeStream->requestStart();
}

engine::impl::~impl() {
}

engine::engine() : pimpl_(make_impl()) {
}

engine::~engine() = default;

channel_ptr engine::output() {
	return impl().output;
}

void engine::setPause(bool pause) {
	if (pause) {
		if (impl().oboeStream) {
			impl().oboeStream->close();
			impl().oboeStream.reset();
		}
	} else {
		if (!impl().oboeStream) {
			const auto result = impl().builder.openStream(impl().oboeStream);
			if (result == oboe::Result::OK) {
				impl().oboeStream->requestStart();
			} else {
				jngl::debugLn(std::string("WARNING: ") + oboe::convertToText(result));
			}
		}
	}
}
} // namespace psemek::audio
