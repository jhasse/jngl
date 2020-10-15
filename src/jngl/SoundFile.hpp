// Copyright 2019-2020 Jan Niklas Hasse <jhasse@bixense.com>
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
	explicit SoundFile(const std::string& filename);
	~SoundFile() = default;
	SoundFile(const SoundFile&) = delete;
	SoundFile& operator=(const SoundFile&) = delete;
	SoundFile(SoundFile&&) = default;
	SoundFile& operator=(SoundFile&&) = default;
	void play();
	void stop();
	bool isPlaying();
	void loop();
	void setPitch(float p);
	void setVolume(float v);

private:
	std::shared_ptr<Sound> sound_;
	std::unique_ptr<SoundParams> params;
	std::vector<char> buffer_;
};

} // namespace jngl
