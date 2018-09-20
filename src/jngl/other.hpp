// Copyright 2014-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>
#include <vector>

#include "dll.hpp"

namespace jngl {

bool JNGLDLL_API running();

void JNGLDLL_API updateInput();

void JNGLDLL_API swapBuffers();

void JNGLDLL_API quit();

void JNGLDLL_API cancelQuit();

void JNGLDLL_API setTitle(const std::string& title);

void JNGLDLL_API setBackgroundColor(unsigned char red, unsigned char green, unsigned char blue);

double JNGLDLL_API getFPS();

/// How many times Work::step is called per second (default: 60)
unsigned int JNGLDLL_API getStepsPerSecond();

void JNGLDLL_API setStepsPerSecond(unsigned int);

void JNGLDLL_API readPixel(int x, int y, unsigned char& red, unsigned char& green,
                           unsigned char& blue);

void JNGLDLL_API setAntiAliasing(bool enabled);

bool JNGLDLL_API getAntiAliasing();

void JNGLDLL_API setVerticalSync(bool enabled);

bool JNGLDLL_API getVerticalSync();

void JNGLDLL_API setIcon(const std::string& filename);

void JNGLDLL_API mainLoop();

void JNGLDLL_API setPrefix(const std::string& path);

std::string JNGLDLL_API getPrefix();

void JNGLDLL_API setConfigPath(const std::string& path);

std::string JNGLDLL_API getConfigPath();

// Returns the directory of the currently running binary
std::string JNGLDLL_API getBinaryPath();

void JNGLDLL_API setArgs(std::vector<std::string>);

/// Returns the command line arguments passed to the executable.
std::vector<std::string> JNGLDLL_API getArgs();

double JNGLDLL_API getTime();

void JNGLDLL_API sleep(int milliseconds);

/// Returns a stringstream containing the whole file. This will read from the .apk on Android.
std::stringstream JNGLDLL_API readAsset(const std::string& filename);

} // namespace jngl
