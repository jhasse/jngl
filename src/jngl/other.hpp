// Copyright 2014-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace jngl {

/// Returns true until the main window is closed or quit() has been called
bool running();

/// Updates the input state. App::mainLoop() calls this before Work::step()
void updateInput();

/// Swaps back and front buffer. App::mainLoop() calls this after Work::draw()
void swapBuffers();

/// Emit a quit event which will exit App::mainLoop() and set running() to false
void quit();

/// Undo quit() or ignore a quit event caused by the user closing the main window
void cancelQuit();

double getFPS();

/// How many times Work::step is called per second (default: 60)
unsigned int getStepsPerSecond();

/// How many times Work::step should be called per second (default: 60)
void setStepsPerSecond(unsigned int);

void setAntiAliasing(bool enabled);

bool getAntiAliasing();

void setVerticalSync(bool enabled);

bool getVerticalSync();

/// Sets the icon for the window (Desktop-only). Only works with PNG files for now.
void setIcon(const std::string& filename);

void setPrefix(const std::string& path);

std::string getPrefix();

void setConfigPath(const std::string& path);

/// Returns the directory where to store configuration files and save games.
/// - Windows: `%%AppData%/Display Name/`
/// - Linux: `~/.config/Display Name/`
/// - macOS: `~/Library/Application Support/Display Name/`
/// - Android/iOS: Data path provided by the OS
std::string getConfigPath();

/// Returns the directory of the currently running binary
std::string getBinaryPath();

void setArgs(std::vector<std::string>);

/// Returns the command line arguments passed to the executable.
std::vector<std::string> getArgs();

/// Returns a stringstream containing the whole file. This will read from the .apk on Android.
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

} // namespace jngl
