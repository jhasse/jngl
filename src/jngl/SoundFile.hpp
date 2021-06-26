// Copyright 2019-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::SoundFile class
/// @file
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace jngl {

class Sound;
struct SoundParams;

/// Sound loaded from an OGG file
class SoundFile {
public:
	/// Load an OGG file called \a filename
	explicit SoundFile(const std::string& filename);
	~SoundFile();
	SoundFile(const SoundFile&) = delete;
	SoundFile& operator=(const SoundFile&) = delete;
	SoundFile(SoundFile&&) noexcept;
	SoundFile& operator=(SoundFile&&) noexcept;

	/// Play the sound once. If called twice the sound would also play twice
	void play();

	/// Stop the last started sound
	void stop();

	/// Whether the sound is still playing at least once
	bool isPlaying();

	/// Play the sound in a loop. Can also be stopped using stop()
	void loop();

	/// Set pitch in (0.0f, ∞]. Default is 1.0f
	void setPitch(float);

	/// Set volume in [0, ∞]. Default is 1.0f
	void setVolume(float v);

private:
	std::shared_ptr<Sound> sound_;
	std::unique_ptr<SoundParams> params;
	std::vector<char> buffer_;
};

} // namespace jngl
