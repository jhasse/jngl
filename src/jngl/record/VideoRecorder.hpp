// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "../job.hpp"

namespace jngl {

/** Records video and audio output to a file
 *
 * VideoRecorder is a background job that captures the rendered frames and audio samples to create
 * a video file.
 *
 * The recorder captures:
 * - Video: Frames are captured at the current window resolution and frame rate
 *   (jngl::getStepsPerSecond())
 * - Audio: Stereo audio at 44.1 kHz sample rate
 *
 * \note Recording requires JNGL to be compiled with JNGL_RECORD enabled and FFmpeg libraries
 * available. Pass `-DJNGL_RECORD=1` to CMake during configuration. On macOS, install FFmpeg
 * with `brew install ffmpeg`.
 *
 * Example usage:
 * \code
 * #include <jngl.hpp>
 * #include <jngl/init.hpp>
 * #include <jngl/record/VideoRecorder.hpp>
 *
 * jngl::AppParameters jnglInit() {
 *     jngl::AppParameters params;
 *     params.start = []() {
 *         // Start recording to output.mkv
 *         jngl::addJob<jngl::VideoRecorder>("output.mkv");
 *         // Your scene setup here...
 *     };
 *     return params;
 * }
 * \endcode
 *
 * The VideoRecorder will automatically capture frames and audio as your application runs. Remove
 * the job or let it go out of scope to finalize and close the video file.
 *
 * \warning During recording, the game will run at a slower wall-clock speed to ensure no frames
 * are dropped. The video encoding is synchronous and the application waits for each frame to be
 * encoded before proceeding. This means the game runs slower in real-time but produces a smooth
 * video at the target frame rate. Audio playback is muted during recording since the game is not
 * running at normal speed.
 *
 * The video container is chosen based on the file extension you provide. For example:
 * - `.mkv`: Matroska container
 * - `.mp4`: MP4 container
 *
 * By default, lossy codecs are used (H.264 for video, AAC for audio) to produce reasonably sized
 * files. If you want lossless quality, pass true for the \a lossless parameter when creating the
 * VideoRecorder.
 *
 * If you chose to use lossless, you would probably convert the resulting .mkv file to a compressed
 * format later so you can upload it to Google Play or the Apple App Store. Use ffmpeg on the
 * command line for that:
 *
 * \code
 * ffmpeg -i foo.mkv \
 *        -c:v libx264 \
 *        -profile:v high \
 *        -level 4.2 \
 *        -pix_fmt yuv420p \
 *        -r 30 \
 *        -c:a aac \
 *        -b:a 160k \
 *        -movflags +faststart \
 *        output.mp4
 * \endcode
 *
 * This command converts foo.mkv to output.mp4 using H.264 video codec and AAC audio codec.
 */
class VideoRecorder : public jngl::Job {
public:
	/// Creates a new video recorder that writes to the specified file (saved in your data/
	/// directory).
	///
	/// Initializes video and audio encoding with the following common settings:
	/// - Frame rate: Current application frame rate (jngl::getStepsPerSecond())
	/// - Resolution: Current window dimensions (jngl::getWindowWidth() x jngl::getWindowHeight())
	///
	/// \param filename Path to the output video file. Use appropriate file extension (e.g., .mkv,
	/// .mp4) to select the container format.
	/// \param lossless If true, uses lossless codecs (FFV1 for video, FLAC for audio). If false,
	/// uses lossy codecs (H.264 for video, AAC for audio). Default is false.
	/// \throws std::runtime_error if video encoding setup fails (e.g., codecs not found, file
	/// cannot be opened)
	explicit VideoRecorder(std::string_view filename, bool lossless = false);

	/// Finalizes and closes the video file
	///
	/// Flushes any remaining frames and audio data, writes the container trailer, and releases
	/// all encoding resources.
	~VideoRecorder() override;

	VideoRecorder(const VideoRecorder&) = delete;
	VideoRecorder& operator=(const VideoRecorder&) = delete;
	VideoRecorder(VideoRecorder&&) = delete;
	VideoRecorder& operator=(VideoRecorder&&) = delete;

	/// Assumes 44.1 kHz, stereo audio input and getting called exactly getStepsPerSecond() times
	/// per second
	void fillAudioBuffer(std::unique_ptr<float[]> samples);

	void step() override;
	void draw() const override;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
