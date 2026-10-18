// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::SoundPanning class
/// @file
#pragma once

#include <memory>
#include <vector>

namespace jngl {

class Channel;
class Sound;
class SoundFile;

/// A sound which can be positioned in the stereo field
///
/// Returned by SoundFile::playWithPanning and SoundFile::loopWithPanning. The sound only starts
/// playing once setPanning has been called for the first time, so that it never starts at the
/// wrong position.
///
/// The sound is tied to the lifetime of this object: Destroying it (or assigning another
/// SoundPanning to it) stops the sound. This makes it a good fit as a member of an entity in your
/// game world.
///
/// Example:
/// \code
/// class Enemy {
/// public:
///     Enemy() : engineSound(jngl::SoundFile::get("sfx/engine.ogg")->loopWithPanning()) {}
///
///     void step() {
///         // ... move the enemy ...
///         engineSound.setPanning(position.x / jngl::getScreenSize().x * 2);
///     }
///
/// private:
///     jngl::Vec2 position;
///     jngl::SoundPanning engineSound; // stops when the Enemy gets destroyed
/// };
/// \endcode
///
/// \note The Channel on which the sound plays must not be moved while this object is alive.
class SoundPanning {
public:
	/// Creates an empty object on which all methods do nothing
	SoundPanning();

	/// Stops the sound
	~SoundPanning();
	SoundPanning(const SoundPanning&) = delete;
	SoundPanning& operator=(const SoundPanning&) = delete;
	SoundPanning(SoundPanning&&) noexcept;

	/// Stops the sound this object has been playing and takes over the other one
	SoundPanning& operator=(SoundPanning&&) noexcept;

	/// Set the stereo position, the first call starts playing the sound
	///
	/// \param panning -1.0f = left, 0.0f = center, 1.0f = right. Values outside of [-1, 1] will be
	///                clamped.
	///
	/// Like with a real head, the channel further away from the sound source gets quieter (-10 dB
	/// at ±1) and slightly delayed. This can be called every frame, e.g. to follow a moving object.
	/// Changes are smoothed, so a fast moving sound will also get a slight Doppler effect.
	///
	/// Once the sound has finished playing (or has been stopped) this does nothing.
	void setPanning(float panning);

	/// Stops the sound. If it hasn't been started yet, it won't start anymore.
	void stop();

	/// Whether the sound has been started (using setPanning) and is still playing
	bool isPlaying() const;

	/// Set volume in [0, ∞]. Default is 1.0f
	///
	/// Can be called before the sound has been started.
	void setVolume(float volume);

private:
	friend class SoundFile;
	SoundPanning(std::shared_ptr<std::vector<float>> buffer, Channel&, bool loop);

	/// Set until the sound has been started
	std::shared_ptr<std::vector<float>> buffer;
	Channel* channel = nullptr;
	bool loop = false;
	float volume = 1.f;

	/// Set after the sound has been started, owned by the Channel
	std::weak_ptr<Sound> sound;
};

} // namespace jngl
