/**
 * TheoraPlay; multithreaded Ogg Theora/Ogg Vorbis decoding.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file was originally written by Ryan C. Gordon.
 */

// I wrote this with a lot of peeking at the Theora example code in
//  libtheora-1.1.1/examples/player_example.c, but this is all my own
//  code.

#include "../../src/log.hpp"

#include <cassert>
#include <cstring>

#include <mutex>
#include <thread>

#include "theora/theoradec.h"
#include "theoraplay.h"
#include "vorbis/codec.h"

#ifdef ANDROID
#include "../../src/android/fopen.hpp"
#endif

#include <atomic>

using VideoFrame = THEORAPLAY_VideoFrame;
using AudioPacket = THEORAPLAY_AudioPacket;

// !!! FIXME: these all count on the pixel format being TH_PF_420 for now.

void ConvertVideoFrame420ToIYUV(const th_info* tinfo, const th_ycbcr_buffer ycbcr, uint8_t* dst) {
	const int p0 = 0;
	const int p1 = 1;
	const int p2 = 2;
	const uint32_t w = tinfo->pic_width;
	const uint32_t h = tinfo->pic_height;
	const uint32_t yoff = (tinfo->pic_x & ~1) + ycbcr[0].stride * (tinfo->pic_y & ~1);
	const uint32_t uvoff = (tinfo->pic_x / 2) + (ycbcr[1].stride) * (tinfo->pic_y / 2);
    const unsigned char *p0data = ycbcr[p0].data + yoff;
	const uint32_t p0stride = ycbcr[p0].stride;
    const unsigned char *p1data = ycbcr[p1].data + uvoff;
	const uint32_t p1stride = ycbcr[p1].stride;
    const unsigned char *p2data = ycbcr[p2].data + uvoff;
	const uint32_t p2stride = ycbcr[p2].stride;

	for (uint32_t i = 0; i < h; ++i, dst += w) {
		memcpy(dst, p0data + static_cast<ptrdiff_t>(p0stride * i), w);
	}
	for (uint32_t i = 0; i < (h / 2); ++i, dst += w / 2) {
		memcpy(dst, p1data + static_cast<ptrdiff_t>(p1stride * i), w / 2);
	}
	for (uint32_t i = 0; i < (h / 2); ++i, dst += w / 2) {
		memcpy(dst, p2data + static_cast<ptrdiff_t>(p2stride * i), w / 2);
	}
}

struct THEORAPLAY_Decoder {
    // Thread wrangling...
	bool thread_created = false;
    std::mutex lock;
	std::atomic_bool halt = false;
	bool thread_done = false;
    std::thread worker;

    // API state...
	THEORAPLAY_Io* io = nullptr;
	unsigned int maxframes = 0; // Max video frames to buffer.
	std::atomic_bool prepped = false;
	std::atomic_uint videocount = 0; // currently buffered frames.
	std::atomic_uint audioms = 0;    // currently buffered audio samples.
	std::atomic_bool hasvideo = false;
	std::atomic_bool hasaudio = false;
	std::atomic_bool decode_error = false;

	THEORAPLAY_VideoFormat vidfmt = THEORAPLAY_VIDFMT_IYUV;

	std::unique_ptr<VideoFrame> videolist;
	VideoFrame* videolisttail = nullptr;

	std::unique_ptr<AudioPacket> audiolist;
	AudioPacket* audiolisttail = nullptr;

	std::unique_ptr<uint8_t[]> ringBuffer;
};

namespace {
int FeedMoreOggData(THEORAPLAY_Io* io, ogg_sync_state* sync) {
	long buflen = 4096;
    char *buffer = ogg_sync_buffer(sync, buflen);
	if (buffer == nullptr) {
		return -1;
	}
	buflen = io->read(io, buffer, buflen);
	if (buflen <= 0) {
		return 0;
	}
	return (ogg_sync_wrote(sync, buflen) == 0) ? 1 : -1;
}

// RAII wrappers for Ogg/Vorbis/Theora resources
struct OggSyncGuard {
	ogg_sync_state sync{};
	OggSyncGuard() { ogg_sync_init(&sync); }
	~OggSyncGuard() { ogg_sync_clear(&sync); }
};

struct VorbisInfoGuard {
	vorbis_info info{};
	VorbisInfoGuard() { vorbis_info_init(&info); }
	~VorbisInfoGuard() { vorbis_info_clear(&info); }
};

struct VorbisCommentGuard {
	vorbis_comment comment{};
	VorbisCommentGuard() { vorbis_comment_init(&comment); }
	~VorbisCommentGuard() { vorbis_comment_clear(&comment); }
};

struct TheoraInfoGuard {
	th_info info{};
	TheoraInfoGuard() { th_info_init(&info); }
	~TheoraInfoGuard() { th_info_clear(&info); }
};

struct TheoraCommentGuard {
	th_comment comment{};
	TheoraCommentGuard() { th_comment_init(&comment); }
	~TheoraCommentGuard() { th_comment_clear(&comment); }
};

struct OggStreamGuard {
	ogg_stream_state stream{};
	bool initialized = false;
	void init(const ogg_stream_state& src) {
		memcpy(&stream, &src, sizeof(stream));
		initialized = true;
	}
	~OggStreamGuard() {
		if (initialized) {
			ogg_stream_clear(&stream);
		}
	}
};

struct VorbisDspGuard {
	vorbis_dsp_state dsp{};
	bool initialized = false;
	bool init(vorbis_info* vinfo) {
		initialized = (vorbis_synthesis_init(&dsp, vinfo) == 0);
		return initialized;
	}
	~VorbisDspGuard() {
		if (initialized) {
			vorbis_dsp_clear(&dsp);
		}
	}
};

struct VorbisBlockGuard {
	vorbis_block block{};
	bool initialized = false;
	bool init(vorbis_dsp_state* vdsp) {
		initialized = (vorbis_block_init(vdsp, &block) == 0);
		return initialized;
	}
	~VorbisBlockGuard() {
		if (initialized) {
			vorbis_block_clear(&block);
		}
	}
};

struct TheoraSetupGuard {
	th_setup_info* setup = nullptr;
	~TheoraSetupGuard() {
		if (setup) {
			th_setup_free(setup);
		}
	}
};

struct TheoraDecGuard {
	th_dec_ctx* dec = nullptr;
	~TheoraDecGuard() {
		if (dec) {
			th_decode_free(dec);
		}
	}
};

struct IoCloseGuard {
	THEORAPLAY_Io* io;
	explicit IoCloseGuard(THEORAPLAY_Io* io) : io(io) {}
	~IoCloseGuard() { io->close(io); }
};

// This massive function is where all the effort happens.
void WorkerThread(THEORAPLAY_Decoder* const ctx) {
	struct ThreadDoneGuard {
		THEORAPLAY_Decoder* ctx;
		explicit ThreadDoneGuard(THEORAPLAY_Decoder* c) : ctx(c) {}
		~ThreadDoneGuard() { ctx->thread_done = true; }
	} threadDoneGuard(ctx);

	IoCloseGuard ioGuard(ctx->io);
	OggSyncGuard syncGuard;
	VorbisInfoGuard vinfoGuard;
	VorbisCommentGuard vcommentGuard;
	TheoraCommentGuard tcommentGuard;
	TheoraInfoGuard tinfoGuard;
	OggStreamGuard vstreamGuard;
	OggStreamGuard tstreamGuard;
	VorbisDspGuard vdspGuard;
	VorbisBlockGuard vblockGuard;
	TheoraSetupGuard tsetupGuard;
	TheoraDecGuard tdecGuard;

	ogg_sync_state& sync = syncGuard.sync;
	vorbis_info& vinfo = vinfoGuard.info;
	vorbis_comment& vcomment = vcommentGuard.comment;
	th_comment& tcomment = tcommentGuard.comment;
	th_info& tinfo = tinfoGuard.info;

	unsigned long audioframes = 0;
	double fps = 0.0;
	bool was_error = true; // resets to false at the end.

	ogg_packet packet{};
	ogg_page page{};
	int vpackets = 0;
	int tpackets = 0;

	// make sure we initialized the stream before using pagein, but the stream
	//  will know to ignore pages that aren't meant for it, so pass to both.
	const auto queueOggPage = [&]() {
		if (tpackets != 0) {
			ogg_stream_pagein(&tstreamGuard.stream, &page);
		}
		if (vpackets != 0) {
			ogg_stream_pagein(&vstreamGuard.stream, &page);
		}
	};
	size_t ringBufferPos = 0;
	size_t ringBufferSize = 0;

	bool bos = true;
	while (!ctx->halt && bos) {
		if (FeedMoreOggData(ctx->io, &sync) <= 0) {
			ctx->decode_error = !ctx->halt;
			return;
		}
		// parse out the initial header.
		while ((!ctx->halt) && (ogg_sync_pageout(&sync, &page) > 0)) {
			ogg_stream_state test;

			if (ogg_page_bos(&page) == 0) { // not a header.
				queueOggPage();
				bos = false;
				break;
			}

			ogg_stream_init(&test, ogg_page_serialno(&page));
			ogg_stream_pagein(&test, &page);
			ogg_stream_packetout(&test, &packet);

			if (tpackets == 0 &&
			    (th_decode_headerin(&tinfo, &tcomment, &tsetupGuard.setup, &packet) >= 0)) {
				tstreamGuard.init(test);
				tpackets = 1;
			} else if (vpackets == 0 &&
			           (vorbis_synthesis_headerin(&vinfo, &vcomment, &packet) >= 0)) {
				vstreamGuard.init(test);
				vpackets = 1;
			} else {
				// whatever it is, we don't care about it
				ogg_stream_clear(&test);
			}
		}
	}

	// no audio OR video?
	if (ctx->halt || (vpackets == 0 && tpackets == 0)) {
		ctx->decode_error = !ctx->halt;
		return;
	}

	// apparently there are two more theora and two more vorbis headers next.
	while ((!ctx->halt) &&
	       ((tpackets != 0 && (tpackets < 3)) || (vpackets != 0 && (vpackets < 3)))) {
		while (!ctx->halt && tpackets != 0 && (tpackets < 3)) {
			if (ogg_stream_packetout(&tstreamGuard.stream, &packet) != 1) {
				break; // get more data?
			}
			if (th_decode_headerin(&tinfo, &tcomment, &tsetupGuard.setup, &packet) == 0) {
				ctx->decode_error = !ctx->halt;
				return;
			}
			tpackets++;
		}

		while (!ctx->halt && vpackets != 0 && (vpackets < 3)) {
			if (ogg_stream_packetout(&vstreamGuard.stream, &packet) != 1) {
				break; // get more data?
			}
			if (vorbis_synthesis_headerin(&vinfo, &vcomment, &packet) != 0) {
				ctx->decode_error = !ctx->halt;
				return;
			}
			vpackets++;
		}

		// get another page, try again?
		if (ogg_sync_pageout(&sync, &page) > 0) {
			queueOggPage();
		} else if (FeedMoreOggData(ctx->io, &sync) <= 0) {
			ctx->decode_error = !ctx->halt;
			return;
		}
	}

	// okay, now we have our streams, ready to set up decoding.
	if (!ctx->halt && tpackets != 0) {
		// th_decode_alloc() docs say to check for insanely large frames yourself.
		if ((tinfo.frame_width > 99999) || (tinfo.frame_height > 99999)) {
			ctx->decode_error = !ctx->halt;
			return;
		}
		// We treat "unspecified" as NTSC. *shrug*
		if ((tinfo.colorspace != TH_CS_UNSPECIFIED) &&
		    (tinfo.colorspace != TH_CS_ITU_REC_470M) &&
		    (tinfo.colorspace != TH_CS_ITU_REC_470BG)) {
			assert(0 && "Unsupported colorspace."); // !!! FIXME
			ctx->decode_error = !ctx->halt;
			return;
		}

		if (tinfo.pixel_fmt != TH_PF_420) {
			assert(0); // !!! FIXME
			ctx->decode_error = !ctx->halt;
			return;
		}

		if (tinfo.fps_denominator != 0) {
			fps = static_cast<double>(tinfo.fps_numerator) /
			      static_cast<double>(tinfo.fps_denominator);
		}
		tdecGuard.dec = th_decode_alloc(&tinfo, tsetupGuard.setup);
		if (!tdecGuard.dec) {
			ctx->decode_error = !ctx->halt;
			return;
		}

		// Set decoder to maximum post-processing level.
		//  Theoretically we could try dropping this level if we're not keeping up.
		int pp_level_max = 0;
		// !!! FIXME: maybe an API to set this?
		// th_decode_ctl(tdecGuard.dec, TH_DECCTL_GET_PPLEVEL_MAX, &pp_level_max,
		// sizeof(pp_level_max));
		th_decode_ctl(tdecGuard.dec, TH_DECCTL_SET_PPLEVEL, &pp_level_max, sizeof(pp_level_max));
	}

	// Done with this now.
	if (tsetupGuard.setup != nullptr) {
		th_setup_free(tsetupGuard.setup);
		tsetupGuard.setup = nullptr;
	}

	if (!ctx->halt && vpackets != 0) {
		if (!vdspGuard.init(&vinfo)) {
			ctx->decode_error = !ctx->halt;
			return;
		}
		if (!vblockGuard.init(&vdspGuard.dsp)) {
			ctx->decode_error = !ctx->halt;
			return;
		}
	}

	// Now we can start the actual decoding!
	// Note that audio and video don't _HAVE_ to start simultaneously.

	ctx->lock.lock();
	ctx->prepped = true;
	ctx->hasvideo = (tpackets != 0);
	ctx->hasaudio = (vpackets != 0);
	ctx->lock.unlock();

	bool eos = false; // end of stream flag.
	while (!ctx->halt && !eos) {
		bool need_pages = false; // need more Ogg pages?
		bool saw_video_frame = false;

		// Try to read as much audio as we can at once. We limit the outer
		//  loop to one video frame and as much audio as we can eat.
		while (!ctx->halt && vpackets != 0) {
			float** pcm = nullptr;
			const int frames = vorbis_synthesis_pcmout(&vdspGuard.dsp, &pcm);
			if (frames > 0) {
				const int channels = vinfo.channels;
				auto item = std::make_unique<AudioPacket>();
				item->playms = static_cast<unsigned int>(
				    ((static_cast<double>(audioframes)) / (static_cast<double>(vinfo.rate))) *
				    1000.0);
				item->channels = channels;
				item->freq = static_cast<int>(vinfo.rate);
				item->frames = frames;
				item->samples = new float[static_cast<size_t>(frames) * channels]; // NOLINT
				item->next = nullptr;

				// I bet this beats the crap out of the CPU cache...
				float* samples = item->samples;
				for (int frameidx = 0; frameidx < frames; frameidx++) {
					for (int chanidx = 0; chanidx < channels; chanidx++) {
						*(samples++) = pcm[chanidx][frameidx];
					}
				}

				vorbis_synthesis_read(&vdspGuard.dsp, frames); // we ate everything.
				audioframes += frames;

				// printf("Decoded %d frames of audio.\n", (int) frames);
				ctx->lock.lock();
				ctx->audioms += item->playms;
				AudioPacket* const tail = item.get();
				if (ctx->audiolisttail) {
					assert(ctx->audiolist);
					ctx->audiolisttail->next = std::move(item);
				} else {
					assert(!ctx->audiolist);
					ctx->audiolist = std::move(item);
				}
				ctx->audiolisttail = tail;
				ctx->lock.unlock();
			} else { // no audio available left in current packet?
				// try to feed another packet to the Vorbis stream...
				if (ogg_stream_packetout(&vstreamGuard.stream, &packet) <= 0) {
					if (tpackets == 0) {
						need_pages = true; // no video, get more pages now.
					}
					break; // we'll get more pages when the video catches up.
				}
				if (vorbis_synthesis(&vblockGuard.block, &packet) == 0) {
					vorbis_synthesis_blockin(&vdspGuard.dsp, &vblockGuard.block);
				}
			}
		}

		if (!ctx->halt && tpackets != 0) {
			// Theora, according to example_player.c, is
			//  "one [packet] in, one [frame] out."
			if (ogg_stream_packetout(&tstreamGuard.stream, &packet) <= 0) {
				need_pages = true;
			} else {
				ogg_int64_t granulepos = 0;

				// you have to guide the Theora decoder to get meaningful timestamps, apparently.
				// :/
				if (packet.granulepos >= 0) {
					th_decode_ctl(tdecGuard.dec, TH_DECCTL_SET_GRANPOS, &packet.granulepos,
					              sizeof(packet.granulepos));
				}
				if (th_decode_packetin(tdecGuard.dec, &packet, &granulepos) == 0) // new frame!
				{
					th_ycbcr_buffer ycbcr;
					if (th_decode_ycbcr_out(tdecGuard.dec, ycbcr) == 0) {
						const double videotime = th_granule_time(tdecGuard.dec, granulepos);
						auto item = std::make_unique<VideoFrame>();
						if (item == nullptr) {
							ctx->decode_error = !ctx->halt;
							return;
						}
						item->playms = static_cast<unsigned int>(videotime * 1000.0);
						item->fps = fps;
						item->width = tinfo.pic_width;
						item->height = tinfo.pic_height;
						item->format = ctx->vidfmt;

						if (!ctx->ringBuffer) {
							while (true) {
								ringBufferSize = static_cast<size_t>(ctx->maxframes + 1) *
								                 item->width * item->height * 2;
								jngl::internal::debug("Allocating {} MB for video ring buffer.",
								                      ringBufferSize / 1024 / 1024);
								try {
									ctx->ringBuffer = std::make_unique<uint8_t[]>(ringBufferSize);
									break;
								} catch (std::bad_alloc&) {
									ctx->maxframes /= 2;
									if (ctx->maxframes < 1) {
										throw;
									}
								}
							}
						}
						item->pixels = &ctx->ringBuffer[ringBufferPos];
						ringBufferPos +=
						    static_cast<size_t>(item->width) * item->height * 2;
						assert(ringBufferPos <= ringBufferSize); // width and height mustn't change
						if (ringBufferPos == ringBufferSize) {
							ringBufferPos = 0;
						}

						ConvertVideoFrame420ToIYUV(&tinfo, ycbcr, item->pixels);
						item->next = nullptr;

						if (item->pixels == nullptr) {
							ctx->decode_error = !ctx->halt;
							return;
						}

						// printf("Decoded another video frame.\n");
						ctx->lock.lock();
						auto itemPtr = item.get();
						if (ctx->videolisttail) {
							assert(ctx->videolist);
							ctx->videolisttail->next = std::move(item);
						} else {
							assert(!ctx->videolist);
							ctx->videolist = std::move(item);
						}
						ctx->videolisttail = itemPtr;
						ctx->videocount++;
						ctx->lock.unlock();

						saw_video_frame = true;
					}
				}
			}
		}

		if (!ctx->halt && need_pages) {
			const int rc = FeedMoreOggData(ctx->io, &sync);
			if (rc == 0) {
				eos = true; // end of stream
			} else if (rc < 0) {
				ctx->decode_error = !ctx->halt;
				return;
			} else {
				while (!ctx->halt && (ogg_sync_pageout(&sync, &page) > 0)) {
					queueOggPage();
				}
			}
		}

		// Sleep the process until we have space for more frames.
		if (saw_video_frame) {
			bool go_on = !ctx->halt;
			// printf("Sleeping.\n");
			while (go_on) {
				// !!! FIXME: This is stupid. I should use a semaphore for this.
				ctx->lock.lock();
				go_on = !ctx->halt && (ctx->videocount >= ctx->maxframes);
				ctx->lock.unlock();
				if (go_on) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
			}
		}
	}

	was_error = false;
	ctx->decode_error = (!ctx->halt && was_error);
}

void* WorkerThreadEntry(void* _this) {
	WorkerThread(static_cast<THEORAPLAY_Decoder*>(_this));
    return nullptr;
}

long IoFopenRead(THEORAPLAY_Io* io, void* buf, long buflen) {
	auto* f = static_cast<FILE*>(io->userdata);
	const size_t br = fread(buf, 1, buflen, f);
	if ((br == 0) && ferror(f) != 0) {
		return -1;
	}
	return static_cast<long>(br);
}

void IoFopenClose(THEORAPLAY_Io* io) {
	auto* f = static_cast<FILE*>(io->userdata);
	fclose(f);
	delete io; // NOLINT
}
} // namespace

THEORAPLAY_Decoder* THEORAPLAY_startDecodeFile(const char* fname, const unsigned int maxframes,
                                               THEORAPLAY_VideoFormat vidfmt) {
	auto* io = new THEORAPLAY_Io; // NOLINT
	FILE* f = fopen(fname, "rb");
	if (f == nullptr) {
		delete io;
		return nullptr;
	}

	io->read = IoFopenRead;
	io->close = IoFopenClose;
	io->userdata = f;
	return THEORAPLAY_startDecode(io, maxframes, vidfmt);
}

THEORAPLAY_Decoder *THEORAPLAY_startDecode(THEORAPLAY_Io *io,
                                           const unsigned int maxframes,
                                           THEORAPLAY_VideoFormat vidfmt)
{
	const auto ctx = new THEORAPLAY_Decoder;

	ctx->maxframes = maxframes;
    ctx->vidfmt = vidfmt;
    ctx->io = io;

    try {
        ctx->worker = std::thread(WorkerThreadEntry, ctx);
        ctx->thread_created = true;
    } catch (std::system_error&) {
        io->close(io);
        delete ctx;
        return nullptr;
	}
	return static_cast<THEORAPLAY_Decoder*>(ctx);
}

void THEORAPLAY_stopDecode(THEORAPLAY_Decoder* const ctx) {
	if (!ctx) {
		return;
	}
	if (ctx->thread_created) {
		ctx->halt = true;
		ctx->worker.join();
	}
	delete ctx;
}

bool THEORAPLAY_isDecoding(THEORAPLAY_Decoder* const ctx) {
	bool retval = false;
    if (ctx)
    {
        ctx->lock.lock();
		retval = (ctx != nullptr &&
		          (ctx->audiolist || ctx->videolist || (ctx->thread_created && !ctx->thread_done)));
		ctx->lock.unlock();
    }
    return retval;
}

bool THEORAPLAY_isInitialized(THEORAPLAY_Decoder* const decoder) {
	if (!decoder) {
		return false;
	}
	std::scoped_lock guard(decoder->lock);
	return decoder->prepped;
}

bool THEORAPLAY_hasVideoStream(THEORAPLAY_Decoder* const decoder) {
	if (!decoder) {
		return false;
	}
	std::scoped_lock guard(decoder->lock);
	return decoder->hasvideo;
}

bool THEORAPLAY_hasAudioStream(THEORAPLAY_Decoder* const decoder) {
	if (!decoder) {
		return false;
	}
	std::scoped_lock guard(decoder->lock);
	return decoder->hasaudio;
}

unsigned int THEORAPLAY_availableVideo(THEORAPLAY_Decoder* decoder) {
	if (!decoder) {
		return 0;
	}
	std::scoped_lock guard(decoder->lock);
	return decoder->videocount;
}

unsigned int THEORAPLAY_availableAudio(THEORAPLAY_Decoder* decoder) {
	if (!decoder) {
		return 0;
	}
	std::scoped_lock guard(decoder->lock);
	return decoder->audioms;
}

bool THEORAPLAY_decodingError(THEORAPLAY_Decoder* const decoder) {
	if (!decoder) {
		return false;
	}
	std::scoped_lock guard(decoder->lock);
	return decoder->decode_error;
}

std::unique_ptr<const THEORAPLAY_AudioPacket> THEORAPLAY_getAudio(THEORAPLAY_Decoder* const ctx) {
	std::unique_ptr<AudioPacket> retval;

	ctx->lock.lock();
	retval = std::move(ctx->audiolist);
	if (retval)
    {
        ctx->audioms -= retval->playms;
		ctx->audiolist = std::move(retval->next);
		retval->next = nullptr;
		if (ctx->audiolist == nullptr) {
			ctx->audiolisttail = nullptr;
		}
	}
    ctx->lock.unlock();

    return retval;
}

THEORAPLAY_AudioPacket::~THEORAPLAY_AudioPacket() noexcept {
	delete[] samples; // NOLINT
}

std::unique_ptr<const THEORAPLAY_VideoFrame> THEORAPLAY_getVideo(THEORAPLAY_Decoder* const ctx) {
    ctx->lock.lock();
	std::unique_ptr<VideoFrame> retval = std::move(ctx->videolist);
	if (retval) {
		ctx->videolist = std::move(retval->next);
		if (ctx->videolist == nullptr) {
			ctx->videolisttail = nullptr;
		}
		assert(ctx->videocount > 0);
        ctx->videocount--;
	}
	ctx->lock.unlock();

	return retval;
}

void THEORAPLAY_freeVideo(const THEORAPLAY_VideoFrame* item) {
	if (item != nullptr) {
		assert(item->next == nullptr);
		delete item;
	}
}

bool THEORAPLAY_threadDone(THEORAPLAY_Decoder* const ctx) {
	if (!ctx) {
		return true;
	}
	std::scoped_lock lock(ctx->lock);
	return ctx->thread_done;
}
