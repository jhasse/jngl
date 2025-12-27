// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "VideoRecorder.hpp"

#ifdef JNGL_RECORD
#include "../../log.hpp"
#include "../../opengl.hpp"
#include "../Finally.hpp"
#include "../other.hpp"
#include "../window.hpp"
#include "../work.hpp"

#include <cassert>
#include <cstddef>
#include <thread>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace jngl {

struct VideoRecorder::Impl {
	AVFormatContext* formatContext = nullptr;
	AVStream* stream = nullptr;
	AVCodecContext* codecContext = nullptr;
	AVFrame* frame = nullptr;
	AVStream* audioStream = nullptr;
	AVCodecContext* audioCodecContext = nullptr;
	AVFrame* audioFrame = nullptr;
	int64_t audioPts = 0;
	std::unique_ptr<uint8_t[]> backBuffer;
	std::unique_ptr<uint8_t[]> pixelBuffer;
	std::unique_ptr<std::thread> workerThread;
	std::unique_ptr<float[]> frameAudioSamples;
	std::unique_ptr<int16_t[]> audioBuffer;
	int audioBufferPos = 0;

	explicit Impl(std::string_view filename) {
		avformat_network_init();
		avformat_alloc_output_context2(&formatContext, nullptr, nullptr,
		                               std::string(filename).c_str());
		if (!formatContext) {
			throw std::runtime_error("Failed to allocate format context.");
		}

		const AVCodec* const codec = avcodec_find_encoder_by_name("ffv1");
		if (!codec) {
			throw std::runtime_error("FFV1 codec not found.");
		}

		stream = avformat_new_stream(formatContext, codec);
		if (!stream) {
			throw std::runtime_error("Failed to create stream.");
		}

		codecContext = avcodec_alloc_context3(codec);
		codecContext->width = getWindowWidth();
		codecContext->height = getWindowHeight();
		codecContext->time_base = { .num = 1, .den = static_cast<int>(getStepsPerSecond()) };
		codecContext->framerate = { .num = static_cast<int>(getStepsPerSecond()), .den = 1 };
		codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
		codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		AVDictionary* codecOpts = nullptr;
		Finally freeCodecOpts{ [&]() { av_dict_free(&codecOpts); } };
		av_dict_set(&codecOpts, "level", "3", 0); // FFV1 level
		av_dict_set(&codecOpts, "coder", "1", 0); // range coder
		if (avcodec_open2(codecContext, codec, &codecOpts) < 0) {
			throw std::runtime_error("Failed to open codec.");
		}

		avcodec_parameters_from_context(stream->codecpar, codecContext);

		// Setup audio stream
		const AVCodec* const audioCodec = avcodec_find_encoder(AV_CODEC_ID_FLAC);
		if (!audioCodec) {
			throw std::runtime_error("FLAC codec not found.");
		}

		audioStream = avformat_new_stream(formatContext, audioCodec);
		if (!audioStream) {
			throw std::runtime_error("Failed to create audio stream.");
		}

		audioCodecContext = avcodec_alloc_context3(audioCodec);
		audioCodecContext->sample_fmt = AV_SAMPLE_FMT_S16;
		audioCodecContext->sample_rate = 44100;
		audioCodecContext->ch_layout = AV_CHANNEL_LAYOUT_STEREO;
		audioCodecContext->time_base = { .num = 1, .den = 44100 };
		audioCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		if (avcodec_open2(audioCodecContext, audioCodec, nullptr) < 0) {
			throw std::runtime_error("Failed to open audio codec.");
		}

		avcodec_parameters_from_context(audioStream->codecpar, audioCodecContext);

		audioFrame = av_frame_alloc();
		audioFrame->format = audioCodecContext->sample_fmt;
		audioFrame->ch_layout = audioCodecContext->ch_layout;
		audioFrame->sample_rate = audioCodecContext->sample_rate;
		audioFrame->nb_samples = audioCodecContext->frame_size;
		if (av_frame_get_buffer(audioFrame, 0) < 0) {
			throw std::runtime_error("Failed to allocate audio frame buffer.");
		}

		audioBuffer =
		    std::make_unique<int16_t[]>(static_cast<long>(audioCodecContext->frame_size) * 2);

		assert(!(formatContext->oformat->flags & AVFMT_NOFILE));
		if (avio_open(&formatContext->pb, std::string(filename).c_str(), AVIO_FLAG_WRITE) < 0) {
			throw std::runtime_error("Failed to open output file.");
		}

		if (avformat_write_header(formatContext, nullptr) < 0) {
			throw std::runtime_error("Failed to write header.");
		}

		frame = av_frame_alloc();
		frame->format = codecContext->pix_fmt;
		frame->width = codecContext->width;
		frame->height = codecContext->height;
		if (av_frame_get_buffer(frame, 0) < 0) {
			throw std::runtime_error("Failed to allocate frame buffer.");
		}

		backBuffer = std::make_unique<uint8_t[]>(3 * static_cast<size_t>(codecContext->width) *
		                                         static_cast<size_t>(codecContext->height));
		pixelBuffer = std::make_unique<uint8_t[]>(3 * static_cast<size_t>(codecContext->width) *
		                                          static_cast<size_t>(codecContext->height));
	}
	~Impl() {
		// Flush audio encoder
		if (audioCodecContext) {
			int ret = avcodec_send_frame(audioCodecContext, nullptr);
			while (ret >= 0) {
				AVPacket* pkt = av_packet_alloc();
				ret = avcodec_receive_packet(audioCodecContext, pkt);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					av_packet_free(&pkt);
					break;
				}
				if (ret < 0) {
					internal::error("Error flushing audio packet: {}", ret);
					av_packet_free(&pkt);
					break;
				}
				pkt->stream_index = audioStream->index;
				av_interleaved_write_frame(formatContext, pkt);
				av_packet_unref(pkt);
				av_packet_free(&pkt);
			}
		}
		// Flush video encoder
		int ret = avcodec_send_frame(codecContext, nullptr);
		while (ret >= 0) {
			AVPacket* pkt = av_packet_alloc();
			ret = avcodec_receive_packet(codecContext, pkt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
				av_packet_free(&pkt);
				break;
			}
			if (ret < 0) {
				internal::error("Error flushing packet: {}", ret);
				av_packet_free(&pkt);
				break;
			}
			pkt->stream_index = stream->index;
			av_interleaved_write_frame(formatContext, pkt);
			av_packet_unref(pkt);
			av_packet_free(&pkt);
		}
		if (audioFrame) {
			av_frame_free(&audioFrame);
		}
		if (audioCodecContext) {
			avcodec_free_context(&audioCodecContext);
		}
		if (frame) {
			av_frame_free(&frame);
		}
		if (codecContext) {
			avcodec_free_context(&codecContext);
		}
		if (formatContext) {
			av_write_trailer(formatContext);
			avio_close(formatContext->pb);
			avformat_free_context(formatContext);
		}
	}
};

VideoRecorder::VideoRecorder(std::string_view filename) : impl(std::make_unique<Impl>(filename)) {
}

VideoRecorder::~VideoRecorder() {
	if (impl->workerThread) {
		impl->workerThread->join();
	}
}

void VideoRecorder::fillAudioBuffer(std::unique_ptr<float[]> samples) {
	impl->frameAudioSamples = std::move(samples);
}

void VideoRecorder::step() {
	resetFrameLimiter();
}

void VideoRecorder::draw() const {
	// FIXME: This is broken when letter-boxing is used
	glReadPixels(0, 0, impl->codecContext->width, impl->codecContext->height, GL_RGB,
	             GL_UNSIGNED_BYTE, impl->backBuffer.get());
	if (impl->workerThread) {
		impl->workerThread->join();
	}
	std::swap(impl->pixelBuffer, impl->backBuffer);
	impl->workerThread = std::make_unique<std::thread>([this]() {
		// Convert RGB to YUV420P
		SwsContext* swsCtx =
		    sws_getContext(impl->codecContext->width, impl->codecContext->height, AV_PIX_FMT_RGB24,
		                   impl->codecContext->width, impl->codecContext->height,
		                   AV_PIX_FMT_YUV420P, SWS_BILINEAR, nullptr, nullptr, nullptr);
		Finally freeSwsCtx{ [&]() { sws_freeContext(swsCtx); } };

		// Flip image vertically: point to last row and use negative linesize
		const int linesize = 3 * impl->codecContext->width;
		const uint8_t* lastRow =
		    impl->pixelBuffer.get() +
		    static_cast<ptrdiff_t>(linesize * (impl->codecContext->height - 1));
		const uint8_t* inData[1] = { lastRow };
		int inLinesize[1] = { -linesize };
		sws_scale(swsCtx, inData, inLinesize, 0, impl->codecContext->height, impl->frame->data,
		          impl->frame->linesize);

		static int64_t pts_counter = 0;
		impl->frame->pts = pts_counter++;

		AVPacket* pkt = av_packet_alloc();
		if (avcodec_send_frame(impl->codecContext, impl->frame) < 0) {
			av_packet_free(&pkt);
			throw std::runtime_error("Error sending frame to encoder.");
		}
		while (avcodec_receive_packet(impl->codecContext, pkt) == 0) {
			av_packet_rescale_ts(pkt, impl->codecContext->time_base, impl->stream->time_base);
			pkt->stream_index = impl->stream->index;
			if (av_interleaved_write_frame(impl->formatContext, pkt) < 0) {
				av_packet_free(&pkt);
				throw std::runtime_error("Error writing frame to output file.");
			}
			av_packet_unref(pkt);
		}
		av_packet_free(&pkt);

		// Encode audio
		if (impl->frameAudioSamples) {
			const int samplesPerFrame = 44100 / static_cast<int>(getStepsPerSecond());
			const int frameSize = impl->audioCodecContext->frame_size;

			// Copy new audio samples to buffer (stereo = 2 samples per frame)
			for (int i = 0; i < samplesPerFrame * 2; ++i) {
				float sample = std::max(-1.0f, std::min(1.0f, impl->frameAudioSamples[i]));
				impl->audioBuffer[impl->audioBufferPos++] = static_cast<int16_t>(sample * 32767.0f);
			}

			// Send complete frames
			while (impl->audioBufferPos >= frameSize * 2) {
				// Copy buffer data to frame
				auto* frameData = reinterpret_cast<int16_t*>(impl->audioFrame->data[0]); // NOLINT
				for (int i = 0; i < frameSize * 2; ++i) {
					frameData[i] = impl->audioBuffer[i];
				}

				// Shift remaining samples
				for (int i = frameSize * 2; i < impl->audioBufferPos; ++i) {
					impl->audioBuffer[i - frameSize * 2] = impl->audioBuffer[i];
				}
				impl->audioBufferPos -= frameSize * 2;

				impl->audioFrame->nb_samples = frameSize;
				impl->audioFrame->pts = impl->audioPts;
				impl->audioPts += frameSize;

				AVPacket* audioPkt = av_packet_alloc();
				if (avcodec_send_frame(impl->audioCodecContext, impl->audioFrame) < 0) {
					av_packet_free(&audioPkt);
					throw std::runtime_error("Error sending audio frame to encoder.");
				}
				while (avcodec_receive_packet(impl->audioCodecContext, audioPkt) == 0) {
					av_packet_rescale_ts(audioPkt, impl->audioCodecContext->time_base,
					                     impl->audioStream->time_base);
					audioPkt->stream_index = impl->audioStream->index;
					if (av_interleaved_write_frame(impl->formatContext, audioPkt) < 0) {
						av_packet_free(&audioPkt);
						throw std::runtime_error("Error writing audio frame to output file.");
					}
					av_packet_unref(audioPkt);
				}
				av_packet_free(&audioPkt);
			}
		}
	});
}

} // namespace jngl
#else
namespace jngl {

struct VideoRecorder::Impl {};

VideoRecorder::VideoRecorder(std::string_view) {
	throw std::runtime_error("VideoRecorder is not available because JNGL_RECORD is not defined. "
	                         "Pass -DJNGL_RECORD=1 to CMake to enable it.");
}
VideoRecorder::~VideoRecorder() = default;
void VideoRecorder::fillAudioBuffer(std::unique_ptr<float[]>) {
}
void VideoRecorder::step() {
}
void VideoRecorder::draw() const {
}
} // namespace jngl
#endif
