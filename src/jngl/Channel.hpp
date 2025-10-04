// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Channel class
/// \file

#include "Finally.hpp"

#include <memory>
#include <string>

namespace jngl {

class SoundFile;
struct Stream;

/// An audio channel, different channels could be for example: "Music", "Speech" and "Sound Effects"
///
/// Example:
/// \code
/// struct Channels : public jngl::Singleton<Channels> {
///     jngl::Channel& main = jngl::Channel::main();
///     jngl::Channel music;
///     jngl::Channel speech;
/// };
///
/// // somewhere else:
/// Channels::handle().music.loop("background_music01.ogg");
/// \endcode
class Channel {
public:
	Channel();
	~Channel();

	/// Play OGG file on this channel
	void play(const std::string& filename);

	/// Play an OGG audio file on this channel in a loop
	///
	/// If it's already playing, this function won't play it twice.
	std::shared_ptr<SoundFile> loop(const std::string& filename);

	/// Stop OGG file playing on this channel
	void stop(const std::string& filename);

	/// Pauses the Channel; destroying the returned Finally object will resume again
	///
	/// Note that this doesn't change the status of jngl::isPlaying as that only depends on the
	/// status of the SoundFile.
	///
	/// Example where you want to pause audio in a menu:
	/// \code
	/// class PauseMenu : public jngl::Scene {
	///     jngl::Finally paused;
	///
	/// public:
	///     PauseMenu() : paused(jngl::Channel::main().pause()) {}
	///     void step() override { /* ... */ }
	///     void draw() const override { /* ... */ }
	/// };
	/// \endcode
	///
	/// Or if you want to toggle pausing audio using <kbd>Space</kbd>:
	/// \code
	/// class PauseExample : public jngl::Scene {
	///     std::optional<jngl::Finally> paused;
	///
	/// public:
	///     void step() override {
	///         if (jngl::keyPressed(jngl::key::Space)) {
	///             if (paused) {
	///                 paused = {}; // calls ~Finally() and resumes playback
	///             } else {
	///                 paused = jngl::Channel::main().pause();
	///             }
	///         }
	///     }
	///     void draw() const override { /* ... */ }
	/// };
	/// \endcode
	[[nodiscard]] Finally pause();

	/// Set volume of this channel in [0, ∞]. Default is 1.0f
	void setVolume(float volume);

	/// Returns the main Channel on which jngl::play, jngl::loop, jngl::stop operate
	static Channel& main();

	/// Internal function for now
	void add(std::shared_ptr<Stream>);
	/// Internal function for now
	void remove(const Stream*);

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
