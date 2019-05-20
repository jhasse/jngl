// Copyright 2010-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <stdexcept>

class WeakLinkingError : public std::runtime_error {
public:
	WeakLinkingError(const std::string& text) : std::runtime_error(text) {
	}
};

extern "C" {
#include <dlfcn.h>
}
#include <functional>
#include <stdexcept>

typedef int ALint;
typedef unsigned int ALuint;
typedef int ALsizei;
typedef int ALenum;
typedef float ALfloat;
typedef void ALvoid;

constexpr auto AL_FALSE = 0;
constexpr auto AL_TRUE = 1;
constexpr auto AL_POSITION = 0x1004;
constexpr auto AL_LOOPING = 0x1007;
#define AL_BUFFER                                 0x1009
#define AL_SOURCE_STATE                           0x1010
#define AL_INITIAL                                0x1011
#define AL_PLAYING                                0x1012
#define AL_PAUSED                                 0x1013
#define AL_STOPPED                                0x1014
#define AL_BUFFERS_PROCESSED                      0x1016
#define AL_FORMAT_MONO8                           0x1100
#define AL_FORMAT_MONO16                          0x1101
#define AL_FORMAT_STEREO8                         0x1102
#define AL_FORMAT_STEREO16                        0x1103
#define AL_PITCH                                  0x1003
#define AL_GAIN                                   0x100A
#define AL_NO_ERROR                               0
#define AL_INVALID_NAME                           0xA001
#define AL_INVALID_ENUM                           0xA002
#define AL_INVALID_VALUE                          0xA003
#define AL_INVALID_OPERATION                      0xA004
#define AL_OUT_OF_MEMORY                          0xA005

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
typedef char ALCboolean;
typedef int ALCint;

template<typename T> class WeakFunction;
template<typename ReturnType, typename... Args>
class WeakFunction<ReturnType(Args...)> {
public:
	WeakFunction(const char* const name) : name_(name) {
	}
	ReturnType operator()(Args... args)
	{
		if (!function_) {
			Load();
		}
		return function_(args...);
	}
	void Load()
	{
		constexpr auto LIBNAME = "OpenAL32.dll";
		static void* handle = dlopen(LIBNAME, RTLD_LAZY);
		if(!handle)
		{
			throw WeakLinkingError(std::string("Could not open ") + LIBNAME + ".");
		}
		ReturnType (*symbol)(Args...);
		*reinterpret_cast<void**>(&symbol) = dlsym(handle, name_.c_str());
		function_ = symbol;
		if(!function_)
		{
			throw WeakLinkingError(std::string("Could not find ") + name_ + " in " + LIBNAME + ".");
		}
	}
private:
	std::function<ReturnType(Args...)> function_;
	const std::string name_;
};

extern WeakFunction<void(ALsizei, ALuint*)> alGenBuffers;
extern WeakFunction<void(ALsizei, ALuint*)> alGenSources;
extern WeakFunction<void(ALenum, ALfloat, ALfloat, ALfloat)> alListener3f;
extern WeakFunction<void(ALuint, ALenum, ALfloat)> alSourcef;
extern WeakFunction<void(ALuint, ALenum, ALfloat, ALfloat, ALfloat)> alSource3f;
extern WeakFunction<void(ALuint, ALenum, const ALvoid*, ALsizei, ALsizei)> alBufferData;
extern WeakFunction<void(ALuint, ALenum, ALint)> alSourcei;
extern WeakFunction<void(ALuint)> alSourcePlay;
extern WeakFunction<void(ALuint)> alSourceStop;
extern WeakFunction<void(ALuint, ALsizei, ALuint*)> alSourceUnqueueBuffers;
extern WeakFunction<void(ALsizei, const ALuint*)> alDeleteSources;
extern WeakFunction<void(ALsizei, const ALuint*)> alDeleteBuffers;
extern WeakFunction<void(ALuint, ALenum, ALint*)> alGetSourcei;
extern WeakFunction<ALenum()> alGetError;
extern WeakFunction<ALCdevice*(const char*)> alcOpenDevice;
extern WeakFunction<ALCcontext*(ALCdevice*, const ALCint*)> alcCreateContext;
extern WeakFunction<ALCboolean(ALCcontext*)> alcMakeContextCurrent;
extern WeakFunction<void(ALCcontext*)> alcDestroyContext;
extern WeakFunction<ALCboolean(ALCdevice*)> alcCloseDevice;

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
