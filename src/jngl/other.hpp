/*
Copyright 2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once


#include <string>
#include <vector>
#include <stdexcept>

#include "dll.hpp"

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif

namespace jngl {
    bool JNGLDLL_API running();

    void JNGLDLL_API updateInput();

    void JNGLDLL_API swapBuffers();

    void JNGLDLL_API quit();

    void JNGLDLL_API cancelQuit();

    void JNGLDLL_API drawRect(double xposition, double yposition, double width, double height);

    void JNGLDLL_API setTitle(const std::string& title);

    void JNGLDLL_API setBackgroundColor(unsigned char red,
                                        unsigned char green,
                                        unsigned char blue);

    double JNGLDLL_API getFPS();

    void JNGLDLL_API errorMessage(const std::string& text);

    bool JNGLDLL_API getFullscreen();

    void JNGLDLL_API readPixel(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue);

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

    std::vector<std::string> JNGLDLL_API getArgs();

    double JNGLDLL_API getTime();

    void JNGLDLL_API sleep(int milliseconds);
}

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
