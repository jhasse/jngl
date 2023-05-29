// Copyright 2014-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Miscellaneous functions
/// @file
#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace jngl {

/// Returns true until the main window is closed or quit() has been called
bool running();

/// Updates the input state. App::mainLoop() calls this before Work::step()
///
/// Normally you shouldn't call this yourself at all, unless you want to implement your own game
/// loop.
void updateInput();

/// Swaps back and front buffer. App::mainLoop() calls this after Work::draw()
///
/// Also clears the back buffer using jngl::clearBackBuffer().
void swapBuffers();

/// Clears the back buffer and resets the ModelView matrix, see jngl::reset()
///
/// jngl::swapBuffers() calls this so there isn't any reason to call this manually most of the time.
void clearBackBuffer();

/// Some platforms (e.g. iOS) don't allow apps to quit themselves
///
/// If this returns false you should hide any "Quit Game" menu buttons.
bool canQuit();

/// Emit a quit event which will exit App::mainLoop() and set running() to false
///
/// If the window hasn't been created yet or the loop is already about to quit, this function does
/// nothing
void quit() noexcept;

/// Undo quit() or ignore a quit event caused by the user closing the main window
void cancelQuit();

/// Calculates FPS if called once per frame
///
/// \deprecated Not really intuitive or robust, use your own calculations instead
[[deprecated("Not really intuitive or robust, use your own calculations instead")]] double getFPS();

/// How many times Work::step is called per second (default: 60)
unsigned int getStepsPerSecond();

/// How many times Work::step should be called per second (default: 60)
void setStepsPerSecond(unsigned int);

/// Toggles MSAA
void setAntiAliasing(bool enabled);

/// Returns whether MSAA is enabled. If the device doesn't support it, it will always return false.
bool getAntiAliasing();

/// Toggles V-SYNC
void setVerticalSync(bool enabled);

/// Returns whether V-SYNC is enabled. Many devices always enable V-SYNC with no way to turn it off.
bool getVerticalSync();

/// Sets the icon for the window (Desktop-only)
void setIcon(const std::string& filename);

/// Sets a global prefix which will be prepended whenever images or sounds are loaded
void setPrefix(const std::string& path);

/// Returns the global prefix set by jngl::setPrefix
std::string getPrefix();

/// \deprecated Use jngl::writeConfig and jngl::readConfig instead.
void setConfigPath(const std::string& path);

/// \deprecated Use jngl::writeConfig and jngl::readConfig instead.
///
/// Returns the directory where to store configuration files and save games.
/// - Windows: `%%AppData%/Display Name/`
/// - Linux: `~/.config/Display Name/`
/// - macOS: `~/Library/Application Support/Display Name/`
/// - Android/iOS: Data path provided by the OS
[[deprecated("use jngl::writeConfig and jngl::readConfig instead")]] std::string getConfigPath();

/// Returns the directory of the currently running binary
std::string getBinaryPath();

/// Called by JNGL_MAIN_BEGIN to set command line arguments
void setArgs(std::vector<std::string>);

/// Returns the command line arguments passed to the executable
std::vector<std::string> getArgs();

/// Returns a stringstream containing the whole file. This will read from the .apk on Android
std::stringstream readAsset(const std::string& filename);

/// Read in a configuration value which has been saved under \a key
///
/// On most platforms this will read the contents of a file named \a key in getConfigPath(). On iOS
/// or tvOS it will use the key/value store provided by the OS.
///
/// \return Value saved by jngl::writeConfig. Empty string if an error occured or \a key doesn't
///         exist.
std::string readConfig(const std::string& key);

/// Write \a value into the persistent storage which can be read in again using
/// jngl::readConfig(\a key)
///
/// \throws std::ifstream::failure if the value couldn't be saved (e.g. jngl::getConfigPath() is
///                                read-only)
/// \throws std::runtime_error if \a key is invalid (it must be a relative file path)
void writeConfig(const std::string& key, const std::string& value);

/// Returns user preferred language, i.e. "en" for English, "de" for German
std::string getPreferredLanguage();

/// Opens a link (e.g. https://bixense.com) in the browser
void openURL(const std::string&);

} // namespace jngl
