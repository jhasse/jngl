// Idea from http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer

#pragma once

#include <cstdio>

struct AAssetManager;

extern AAssetManager* android_asset_manager;

FILE* android_fopen(const char* fname, const char* mode);

#define fopen(name, mode) android_fopen(name, mode)
