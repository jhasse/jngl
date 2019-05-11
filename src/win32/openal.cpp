// Copyright 2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "openal.hpp"

WeakFunction<void(ALsizei, ALuint*)> alGenBuffers("alGenBuffers");
WeakFunction<void(ALsizei, ALuint*)> alGenSources("alGenSources");
WeakFunction<void(ALenum, ALfloat, ALfloat, ALfloat)> alListener3f("alListener3f");
WeakFunction<void(ALuint, ALenum, ALfloat)> alSourcef("alSourcef");
WeakFunction<void(ALuint, ALenum, ALfloat, ALfloat, ALfloat)> alSource3f("alSource3f");
WeakFunction<void(ALuint, ALenum, const ALvoid*, ALsizei, ALsizei)> alBufferData("alBufferData");
WeakFunction<void(ALuint, ALenum, ALint)> alSourcei("alSourcei");
WeakFunction<void(ALuint)> alSourcePlay("alSourcePlay");
WeakFunction<void(ALuint)> alSourceStop("alSourceStop");
WeakFunction<void(ALuint, ALsizei, ALuint*)> alSourceUnqueueBuffers("alSourceUnqueueBuffers");
WeakFunction<void(ALsizei, const ALuint*)> alDeleteSources("alDeleteSources");
WeakFunction<void(ALsizei, const ALuint*)> alDeleteBuffers("alDeleteBuffers");
WeakFunction<void(ALuint, ALenum, ALint*)> alGetSourcei("alGetSourcei");
WeakFunction<ALenum()> alGetError("alGetError");
WeakFunction<ALCdevice*(const char*)> alcOpenDevice("alcOpenDevice");
WeakFunction<ALCcontext*(ALCdevice*, const ALCint*)> alcCreateContext("alcCreateContext");
WeakFunction<ALCboolean(ALCcontext*)> alcMakeContextCurrent("alcMakeContextCurrent");
WeakFunction<void(ALCcontext*)> alcDestroyContext("alcDestroyContext");
WeakFunction<ALCboolean(ALCdevice*)> alcCloseDevice("alcCloseDevice");
