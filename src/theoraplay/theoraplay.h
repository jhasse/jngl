/**
 * TheoraPlay; multithreaded Ogg Theora/Ogg Vorbis decoding.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file was originally written by Ryan C. Gordon.
 */

#pragma once

#include <cstdint>
#include <memory>

struct THEORAPLAY_Io
{
    long (*read)(THEORAPLAY_Io *io, void *buf, long buflen);
    void (*close)(THEORAPLAY_Io *io);
    void *userdata;
};

struct THEORAPLAY_Decoder;

/* YV12 is YCrCb, not YCbCr; that's what SDL uses for YV12 overlays. */
enum THEORAPLAY_VideoFormat : uint8_t {
	THEORAPLAY_VIDFMT_YV12, /* NTSC colorspace, planar YCrCb 4:2:0 */
	THEORAPLAY_VIDFMT_IYUV, /* NTSC colorspace, planar YCbCr 4:2:0 */
	THEORAPLAY_VIDFMT_RGB,  /* 24 bits packed pixel RGB */
	THEORAPLAY_VIDFMT_RGBA  /* 32 bits packed pixel RGBA (full alpha). */
};

struct THEORAPLAY_VideoFrame {
	unsigned int playms;
	double fps;
	unsigned int width;
	unsigned int height;
	THEORAPLAY_VideoFormat format;
	unsigned char* pixels;
	std::unique_ptr<THEORAPLAY_VideoFrame> next;
};

struct THEORAPLAY_AudioPacket {
	~THEORAPLAY_AudioPacket() noexcept;

	unsigned int playms; /* playback start time in milliseconds. */
	int channels;
	int freq;
	int frames;
	float* samples; /* frames * channels float32 samples. */
	std::unique_ptr<THEORAPLAY_AudioPacket> next;
};

THEORAPLAY_Decoder* THEORAPLAY_startDecodeFile(const char* fname, unsigned int maxframes,
                                               THEORAPLAY_VideoFormat vidfmt);
THEORAPLAY_Decoder* THEORAPLAY_startDecode(THEORAPLAY_Io* io, unsigned int maxframes,
                                           THEORAPLAY_VideoFormat vidfmt);
void THEORAPLAY_stopDecode(THEORAPLAY_Decoder*);

bool THEORAPLAY_isDecoding(THEORAPLAY_Decoder*);
bool THEORAPLAY_decodingError(THEORAPLAY_Decoder*);
bool THEORAPLAY_isInitialized(THEORAPLAY_Decoder*);
bool THEORAPLAY_hasVideoStream(THEORAPLAY_Decoder*);
bool THEORAPLAY_hasAudioStream(THEORAPLAY_Decoder*);
unsigned int THEORAPLAY_availableVideo(THEORAPLAY_Decoder*);
unsigned int THEORAPLAY_availableAudio(THEORAPLAY_Decoder*);

std::unique_ptr<const THEORAPLAY_AudioPacket> THEORAPLAY_getAudio(THEORAPLAY_Decoder*);

std::unique_ptr<const THEORAPLAY_VideoFrame> THEORAPLAY_getVideo(THEORAPLAY_Decoder*);

/// is the background thread done decoding? (returns true before THEORAPLAY_isDecoding)
bool THEORAPLAY_threadDone(THEORAPLAY_Decoder*);
