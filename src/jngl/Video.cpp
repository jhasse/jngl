// Copyright 2018-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Video.hpp"

#include <stdexcept>

#ifdef JNGL_VIDEO

#include "../../subprojects/theoraplay/theoraplay.h"
#include "../Sound.hpp"
#include "../audio.hpp"
#include "../main.hpp"
#include "../opengl.hpp"
#include "Shader.hpp"
#include "debug.hpp"
#include "screen.hpp"
#include "sound.hpp"
#include "time.hpp"

#include <boost/numeric/conversion/cast.hpp>
#include <deque>
#ifdef __APPLE__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

namespace jngl {

class Video::Impl {
public:
	explicit Impl(const std::string& filename)
	: decoder(THEORAPLAY_startDecodeFile((pathPrefix + filename).c_str(), BUFFER_SIZE,
	                                     THEORAPLAY_VIDFMT_IYUV)),
	  startTime(-jngl::getTime()) {
		if (!decoder) {
			throw std::runtime_error("Failed to start decoding " + filename + "!");
		}

		while (!video) {
			video = THEORAPLAY_getVideo(decoder);
		}
		timePerFrame = 1. / double(video->fps);
		assert(timePerFrame > 0);

		GetAudio();
		alGenSources(1, &source);
		checkAlError();
		alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
		checkAlError();
		alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
		checkAlError();

		while (!audio) {
			audio = THEORAPLAY_getAudio(decoder);
		}

		switch (audio->channels) {
			case 1:
				format = AL_FORMAT_MONO16;
				break;
			case 2:
				format = AL_FORMAT_STEREO16;
				break;
			default:
				throw std::runtime_error("Unsupported number of channels.");
		}
	}

	void draw() {
		if (!started()) {
			const double timeBuffering = jngl::getTime() + startTime;
			if (timeBuffering > 5 || THEORAPLAY_availableVideo(decoder) >= BUFFER_SIZE ||
			    THEORAPLAY_threadDone(decoder)) {
				jngl::debug("Buffering took ");
				jngl::debug(timeBuffering);
				jngl::debug(" seconds (");
				jngl::debug(THEORAPLAY_availableVideo(decoder));
				jngl::debugLn(" frames).");
				startTime = jngl::getTime();
			}
		}
		double now = jngl::getTime() - startTime;
		if (started() && !video) {
			video = THEORAPLAY_getVideo(decoder);
		}
		if (!shaderProgram or (video and double(video->playms) / 1000. <= now)) {
			if (started() and now - double(video->playms) / 1000. >= timePerFrame) {
				// Skip frames to catch up, but keep track of the last one in case we catch up to a
				// series of dupe frames, which means we'd have to draw that final frame and then
				// wait for more.
				const THEORAPLAY_VideoFrame* last = video;
				while ((video = THEORAPLAY_getVideo(decoder)) != nullptr) {
					jngl::debug("\x1b[33mskipped frame at ");
					jngl::debug(last->playms);
					jngl::debugLn("ms\x1b[0m");
					THEORAPLAY_freeVideo(last);
					last = video;
					if (now - double(video->playms) / 1000. < timePerFrame) { break; }
				}

				if (!video) {
					video = last;
				}
			}
			if (!shaderProgram) {
				Shader vertexShader(R"(#version 300 es
					in mediump vec2 position;
					in mediump vec2 inTexCoord;
					uniform mediump mat3 modelview;
					uniform mediump mat4 projection;
					out mediump vec2 texCoord;

					void main() {
						vec3 tmp = modelview * vec3(position, 1);
						gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
						texCoord = inTexCoord;
					})", Shader::Type::VERTEX
				);
				Shader fragmentShader(R"(#version 300 es
					uniform sampler2D texY;
					uniform sampler2D texU;
					uniform sampler2D texV;

					in mediump vec2 texCoord;

					out lowp vec4 outColor;

					void main() {
						lowp float y = texture(texY, texCoord).r;
						lowp float u = texture(texU, texCoord).r - 0.5;
						lowp float v = texture(texV, texCoord).r - 0.5;
						y = 1.1643 * (y - 0.0625);
						outColor = vec4(
							y + 1.5958 * v,
							y - 0.39173 * u - 0.81290 * v,
							y + 2.017 * u,
							1.0
						);
					})", Shader::Type::FRAGMENT
				);
				shaderProgram = std::make_unique<ShaderProgram>(vertexShader, fragmentShader);
				modelviewUniform = shaderProgram->getUniformLocation("modelview");
				const auto projectionUniform =
				    shaderProgram->getUniformLocation("projection");
				const auto tmp = shaderProgram->use();
				glUniform1i(shaderProgram->getUniformLocation("texU"), 1);
				glUniform1i(shaderProgram->getUniformLocation("texV"), 2);
				glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, &opengl::projection.a[0][0]);

				textureY = opengl::genAndBindTexture();
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, video->width, video->height, 0, GL_RED,
				             GL_UNSIGNED_BYTE, nullptr);

				textureU = opengl::genAndBindTexture();
				// U and V components are a quarter of the size of the video:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, video->width / 2, video->height / 2, 0,
				             GL_RED, GL_UNSIGNED_BYTE, nullptr);

				textureV = opengl::genAndBindTexture();
				// U and V components are a quarter of the size of the video:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, video->width / 2, video->height / 2, 0,
				             GL_RED, GL_UNSIGNED_BYTE, nullptr);

				const float preciseWidth = video->width * getScaleFactor();
				const float preciseHeight = video->height * getScaleFactor();
				const static GLfloat vertexes[] = {
					-preciseWidth / 2, -preciseHeight / 2,
					0, 0, // texture coordinates
					-preciseWidth / 2, preciseHeight / 2,
					0, 1, // texture coordinates
					preciseWidth / 2, preciseHeight / 2,
					1, 1, // texture coordinates
					preciseWidth / 2, -preciseHeight / 2,
					1, 0 // texture coordinates
				};
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);

				glGenBuffers(1, &vertexBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW);

				const GLint posAttrib = shaderProgram->getAttribLocation("position");
				glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
				glEnableVertexAttribArray(posAttrib);

				const GLint texCoordAttrib = shaderProgram->getAttribLocation("inTexCoord");
				glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
				                      reinterpret_cast<void*>(2 * sizeof(float))); // NOLINT
				glEnableVertexAttribArray(texCoordAttrib);
			}

			glBindTexture(GL_TEXTURE_2D, textureY);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, video->width, video->height, GL_RED,
			                GL_UNSIGNED_BYTE, video->pixels);

			assert(video->width % 2 == 0 && video->height % 2 == 0);
			glBindTexture(GL_TEXTURE_2D, textureU);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, video->width / 2, video->height / 2, GL_RED,
			                GL_UNSIGNED_BYTE, video->pixels + video->width * video->height);

			glBindTexture(GL_TEXTURE_2D, textureV);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, video->width / 2, video->height / 2, GL_RED,
			                GL_UNSIGNED_BYTE,
			                video->pixels + std::lround(1.25 * video->width * video->height));

			THEORAPLAY_freeVideo(video);
			video = nullptr;
		}
		if (started()) {
			if (!audio) {
				audio = THEORAPLAY_getAudio(decoder);
			}
			alSourcef(source, AL_GAIN, getVolume());
			while (audio) {
				ALint processed;
				alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
				checkAlError();

				assert(processed >= 0);

				if (processed > 0) {
					// Reuse a processed buffer
					alSourceUnqueueBuffers(source, 1, &buffers.front());
					checkAlError();

					queueAudio(buffers.front());
					buffers.pop_front();

					ALint state;
					alGetSourcei(source, AL_SOURCE_STATE, &state);
					checkAlError();
					if (state != AL_PLAYING) {
						while (true) {
							alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
							checkAlError();
							if (processed == 0) {
								break;
							}
							alSourceUnqueueBuffers(source, 1, &buffers.front());
							checkAlError();
							alDeleteBuffers(1, &buffers.front());
							checkAlError();
							buffers.pop_front();
						}

						alSourcePlay(source);
						checkAlError();
						jngl::debugLn("WARNING: Audio buffer underrun!");
					}
				} else {
					ALuint buffer;
					alGenBuffers(1, &buffer);
					checkAlError();
					queueAudio(buffer);
					if (buffers.size() == 1) {
						alSourcePlay(source);
						checkAlError();
					}
				}
			}
		}
		if (shaderProgram) {
			auto _ = shaderProgram->use();
			glUniformMatrix3fv(modelviewUniform, 1, GL_TRUE, &opengl::modelview.a[0][0]);
			glBindVertexArray(vao);

			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, textureU);

			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, textureV);

			glActiveTexture(GL_TEXTURE0); // Set this last so that it stays active after Video::draw
			glBindTexture(GL_TEXTURE_2D, textureY);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
	}

	[[nodiscard]] bool finished() const {
		return !THEORAPLAY_isDecoding(decoder);
	}

	~Impl() {
		alDeleteSources(1, &source);
		checkAlError();
		for (auto buffer : buffers) {
			alDeleteBuffers(1, &buffer);
			checkAlError();
		}
		THEORAPLAY_stopDecode(decoder);
	}

	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;
	Impl(Impl&&) = delete;
	Impl& operator=(Impl&&) = delete;

	[[nodiscard]] int getWidth() const { return boost::numeric_cast<int>(video->width); }
	[[nodiscard]] int getHeight() const { return boost::numeric_cast<int>(video->height); }

private:
	[[nodiscard]] bool started() const {
		return startTime > 0;
	}

	void queueAudio(ALuint buffer) {
		auto pcm = std::make_unique<int16_t[]>(audio->frames * audio->channels);
		for (int i = 0; i < audio->frames * audio->channels; ++i) {
			const float sample = std::clamp(audio->samples[i], -1.f, 1.f);
			pcm[i] = static_cast<int16_t>(sample * 32767.f);
		}

		alBufferData(buffer, format, pcm.get(), audio->frames * audio->channels * sizeof(int16_t),
		             audio->freq);
		checkAlError();
		alSourceQueueBuffers(source, 1, &buffer);
		checkAlError();

		buffers.push_back(buffer);

		THEORAPLAY_freeAudio(audio);
		audio = THEORAPLAY_getAudio(decoder);
	}

	constexpr static unsigned int BUFFER_SIZE = 200;

	std::unique_ptr<jngl::Sound> sound;
	THEORAPLAY_Decoder* decoder;
	const THEORAPLAY_VideoFrame* video = nullptr;
	const THEORAPLAY_AudioPacket* audio = nullptr;
	double startTime;
	double timePerFrame;
	std::deque<ALuint> buffers;
	ALuint source = 0;
	ALenum format;

	std::unique_ptr<jngl::ShaderProgram> shaderProgram;
	int modelviewUniform = -1;
	GLuint textureY = 0; // luminance
	GLuint textureU = 0; // chrominance
	GLuint textureV = 0; // chrominance
	GLuint vao = 0;
	GLuint vertexBuffer = 0;
};

Video::Video(const std::string& filename) : impl(std::make_unique<Impl>(filename)) {
}

Video::~Video() = default;

void Video::draw() const {
	impl->draw();
}

int Video::getWidth() const {
	return impl->getWidth();
}

int Video::getHeight() const {
	return impl->getHeight();
}

bool Video::finished() const {
	return impl->finished();
}

} // namespace jngl

#else

namespace jngl {

class Video::Impl {
};

Video::Video(const std::string&) {
	throw std::runtime_error("JNGL compiled without video support!");
}

Video::~Video() = default;

void Video::draw() const {
}

int Video::getWidth() const { return -1; } // NOLINT
int Video::getHeight() const { return -1; } // NOLINT

bool Video::finished() const { return true; } // NOLINT

} // namespace jngl

#endif
