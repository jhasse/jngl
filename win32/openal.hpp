/*
Copyright 2010 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
*/

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

#define AL_POSITION                               0x1004
#define AL_BUFFER                                 0x1009
#define AL_SOURCE_STATE                           0x1010
#define AL_INITIAL                                0x1011
#define AL_PLAYING                                0x1012
#define AL_PAUSED                                 0x1013
#define AL_STOPPED                                0x1014
#define AL_FORMAT_MONO8                           0x1100
#define AL_FORMAT_MONO16                          0x1101
#define AL_FORMAT_STEREO8                         0x1102
#define AL_FORMAT_STEREO16                        0x1103
#define AL_PITCH                                  0x1003

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
typedef char ALCboolean;
typedef int ALCint;

class WeakLinkingError : public std::runtime_error {
public:
	WeakLinkingError(const std::string& text) : std::runtime_error(text)
	{
	}
};

template<typename T> class WeakFunction;
template<typename ReturnType, typename... Args>
class WeakFunction<ReturnType(Args...)> {
public:
	WeakFunction(const char* const name, const char* const libName) : name_(name), libName_(libName)
	{
	}
	ReturnType operator()(Args... args)
	{
		static bool loaded = false;
		if(!loaded)
		{
			Load();
		}
		return function_(args...);
	}
	void Load()
	{
		void* handle = dlopen(libName_.c_str(), RTLD_LAZY);
		if(!handle)
		{
			throw WeakLinkingError(std::string("Could not open ") + libName_ + ".");
		}
		ReturnType (*symbol)(Args...);
		*reinterpret_cast<void**>(&symbol) = dlsym(handle, name_.c_str());
		function_ = symbol;
		if(!function_)
		{
			throw WeakLinkingError(std::string("Could not find ") + name_ + " in " + libName_ + ".");
		}
	}
private:
	std::function<ReturnType(Args...)> function_;
	const std::string name_;
	const std::string libName_;
};

WeakFunction<void(ALsizei, ALuint*)> alGenBuffers("alGenBuffers", "OpenAL32.dll");
WeakFunction<void(ALsizei, ALuint*)> alGenSources("alGenSources", "OpenAL32.dll");
WeakFunction<void(ALenum, ALfloat, ALfloat, ALfloat)> alListener3f("alListener3f", "OpenAL32.dll");
WeakFunction<void(ALuint, ALenum, ALfloat)> alSourcef("alSourcef", "OpenAL32.dll");
WeakFunction<void(ALuint, ALenum, ALfloat, ALfloat, ALfloat)> alSource3f("alSource3f", "OpenAL32.dll");
WeakFunction<void(ALuint, ALenum, const ALvoid*, ALsizei, ALsizei)> alBufferData("alBufferData", "OpenAL32.dll");
WeakFunction<void(ALuint, ALenum, ALint)> alSourcei("alSourcei", "OpenAL32.dll");
WeakFunction<void(ALuint)> alSourcePlay("alSourcePlay", "OpenAL32.dll");
WeakFunction<void(ALuint)> alSourceStop("alSourceStop", "OpenAL32.dll");
WeakFunction<void(ALuint, ALsizei, ALuint*)> alSourceUnqueueBuffers("alSourceUnqueueBuffers", "OpenAL32.dll");
WeakFunction<void(ALsizei, const ALuint*)> alDeleteSources("alDeleteSources", "OpenAL32.dll");
WeakFunction<void(ALsizei, const ALuint*)> alDeleteBuffers("alDeleteBuffers", "OpenAL32.dll");
WeakFunction<void(ALuint,  ALenum, ALint*)> alGetSourcei("alGetSourcei", "OpenAL32.dll");

WeakFunction<ALCdevice*(const char*)> alcOpenDevice("alcOpenDevice", "OpenAL32.dll");
WeakFunction<ALCcontext*(ALCdevice*, const ALCint*)> alcCreateContext("alcCreateContext", "OpenAL32.dll");
WeakFunction<ALCboolean(ALCcontext*)> alcMakeContextCurrent("alcMakeContextCurrent", "OpenAL32.dll");
WeakFunction<void(ALCcontext*)> alcDestroyContext("alcDestroyContext", "OpenAL32.dll");
WeakFunction<ALCboolean(ALCdevice*)> alcCloseDevice("alcCloseDevice", "OpenAL32.dll");

#include <vorbis/codec.h>

typedef struct {
  size_t (*read_func)  (void *ptr, size_t size, size_t nmemb, void *datasource);
  int    (*seek_func)  (void *datasource, ogg_int64_t offset, int whence);
  int    (*close_func) (void *datasource);
  long   (*tell_func)  (void *datasource);
} ov_callbacks;

typedef struct OggVorbis_File {
  void            *datasource; /* Pointer to a FILE *, etc. */
  int              seekable;
  ogg_int64_t      offset;
  ogg_int64_t      end;
  ogg_sync_state   oy;
  int              links;
  ogg_int64_t     *offsets;
  ogg_int64_t     *dataoffsets;
  long            *serialnos;
  ogg_int64_t     *pcmlengths;
  vorbis_info     *vi;
  vorbis_comment  *vc;
  ogg_int64_t      pcm_offset;
  int              ready_state;
  long             current_serialno;
  int              current_link;
  double           bittrack;
  double           samptrack;
  ogg_stream_state os;
  vorbis_dsp_state vd;
  vorbis_block     vb;
  ov_callbacks callbacks;
} OggVorbis_File;

WeakFunction<int(FILE*, OggVorbis_File*, char*, long)> ov_open("ov_open", "vorbisfile.dll");
WeakFunction<vorbis_info*(OggVorbis_File*, int)> ov_info("ov_info", "vorbisfile.dll");
WeakFunction<int(OggVorbis_File*)> ov_clear("ov_clear", "vorbisfile.dll");
WeakFunction<long(OggVorbis_File*, char*, int, int, int, int, int*)> ov_read("ov_read", "vorbisfile.dll");
