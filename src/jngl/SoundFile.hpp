// Copyright 2019-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::SoundFile class
/// @file
#pragma once

#include <future>
#include <memory>
#include <string>
#include <vector>
#if defined(__has_include) && __has_include(<optional>)
#include <optional>
using std::optional;
#else
#include <experimental/optional>
using std::experimental::optional;
#endif

namespace jngl {

class Channel;
class Sound;
struct SoundParams;

/// Sound loaded from an OGG file
///
/// JNGL keeps a list of loaded sound files, so there's no need for you to use this class directly -
/// you can just use jngl::play.
class SoundFile {
public:
	/// Load an OGG file called \a filename
	///
	/// Loading can either happen on its own thread (std::launch::async) or the first time you
	/// try to play the file (std::launch::deferred).
	///
	/// \note
	/// If the file doesn't exist this will not throw, but calling SoundFile::play, SoundFile::loop
	/// or SoundFile::load will.
	explicit SoundFile(const std::string& filename, std::launch policy = std::launch::async);
	~SoundFile();
	SoundFile(const SoundFile&) = delete;
	SoundFile& operator=(const SoundFile&) = delete;
	SoundFile(SoundFile&&) noexcept;
	SoundFile& operator=(SoundFile&&) noexcept;

	/// Play the sound once. If called twice the sound would also play twice
	void play();
	void play(Channel&);

	/// Stop the last started sound
	void stop();
	void stop(Channel&);

	/// Whether the sound is still playing at least once
	bool isPlaying();

	/// Play the sound in a loop. Can also be stopped using stop()
	void loop();
	void loop(Channel&);

	/// Set volume in [0, ∞]. Default is 1.0f
	void setVolume(float v);

	/// Block until the sound file has been fully decompressed and loaded
	///
	/// \throws std::runtime_error File not found or decoding errors
	void load();

private:
	std::shared_ptr<Sound> sound_;
	std::vector<float> buffer_;
	long frequency = -1;
};

} // namespace jngl
