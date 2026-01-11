// Copyright 2018-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Video.hpp"

#include <stdexcept>

#ifdef JNGL_VIDEO

#include "../audio.hpp"
#include "../audio/constants.hpp"
#include "../audio/effect/pitch.hpp"
#include "../log.hpp"
#include "../main.hpp"
#include "../opengl.hpp"
#include "../theoraplay/theoraplay.h"
#include "Channel.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "screen.hpp"
#include "time.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <gsl/narrow>
#include <mutex>

namespace jngl {

class Video::Impl : public Stream {
public:
	explicit Impl(const std::string& filename)
	: decoder(THEORAPLAY_startDecodeFile((pathPrefix + filename).c_str(), BUFFER_SIZE,
	                                     THEORAPLAY_VIDFMT_IYUV)),
	  startTime(-getTime()) {
		if (!decoder) {
			throw std::runtime_error("Failed to start decoding " + filename + "!");
		}

		while (!video) {
			video = THEORAPLAY_getVideo(decoder);
		}
		timePerFrame = 1. / video->fps;
		assert(timePerFrame > 0);

		while (!audio) {
			audio = THEORAPLAY_getAudio(decoder);
		}
	}

	int getFrequency() const {
		return audio->freq;
	}

	void draw() {
		if (!started()) {
			const double timeBuffering = getTime() + startTime;
			if (timeBuffering > 5 || THEORAPLAY_availableVideo(decoder) >= BUFFER_SIZE ||
			    THEORAPLAY_threadDone(decoder)) {
				internal::debug("Buffering took {:.2f} seconds ({} frames).", timeBuffering,
				                THEORAPLAY_availableVideo(decoder));
				startTime = getTime();
			}
		}
		double now = getTime() - startTime;
		if (started() && !video) {
			video = THEORAPLAY_getVideo(decoder);
		}
		if (!shaderProgram || (video && static_cast<double>(video->playms) / 1000. <= now)) {
			if (started() && now - static_cast<double>(video->playms) / 1000. >= timePerFrame) {
				// Skip frames to catch up, but keep track of the last one in case we catch up to a
				// series of dupe frames, which means we'd have to draw that final frame and then
				// wait for more.
				std::unique_ptr<const THEORAPLAY_VideoFrame> last = std::move(video);
				while ((video = THEORAPLAY_getVideo(decoder)) != nullptr) {
					internal::warn("Skipped frame at {}ms.", last->playms);
					last = std::move(video);
					if (now - static_cast<double>(last->playms) / 1000. < timePerFrame) {
						break;
					}
				}

				if (!video) {
					video = std::move(last);
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
					})", Shader::Type::VERTEX, R"(
					attribute mediump vec2 position;
					attribute mediump vec2 inTexCoord;
					uniform mediump mat3 modelview;
					uniform mediump mat4 projection;
					varying mediump vec2 texCoord;

					void main() {
						vec3 tmp = modelview * vec3(position, 1);
						gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
						texCoord = inTexCoord;
					})");
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
					})", Shader::Type::FRAGMENT, R"(
					uniform sampler2D texY;
					uniform sampler2D texU;
					uniform sampler2D texV;

					varying mediump vec2 texCoord;

					void main() {
						lowp float y = texture2D(texY, texCoord).r;
						lowp float u = texture2D(texU, texCoord).r - 0.5;
						lowp float v = texture2D(texV, texCoord).r - 0.5;
						y = 1.1643 * (y - 0.0625);
						gl_FragColor = vec4(
							y + 1.5958 * v,
							y - 0.39173 * u - 0.81290 * v,
							y + 2.017 * u,
							1.0
						);
					})"
				);
				shaderProgram = std::make_unique<ShaderProgram>(vertexShader, fragmentShader);
				modelviewUniform = shaderProgram->getUniformLocation("modelview");
				const auto projectionUniform =
				    shaderProgram->getUniformLocation("projection");
				const auto tmp = shaderProgram->use();
				glUniform1i(shaderProgram->getUniformLocation("texU"), 1);
				glUniform1i(shaderProgram->getUniformLocation("texV"), 2);
				glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, opengl::projection.data);

				textureY = opengl::genAndBindTexture();
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, static_cast<GLsizei>(video->width),
				             static_cast<GLsizei>(video->height), 0, GL_RED, GL_UNSIGNED_BYTE,
				             nullptr);

				textureU = opengl::genAndBindTexture();
				// U and V components are a quarter of the size of the video:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, static_cast<GLsizei>(video->width / 2),
				             static_cast<GLsizei>(video->height / 2), 0, GL_RED, GL_UNSIGNED_BYTE,
				             nullptr);

				textureV = opengl::genAndBindTexture();
				// U and V components are a quarter of the size of the video:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, static_cast<GLsizei>(video->width / 2),
				             static_cast<GLsizei>(video->height / 2), 0, GL_RED, GL_UNSIGNED_BYTE,
				             nullptr);

				const auto preciseWidth = static_cast<float>(video->width * getScaleFactor());
				const auto preciseHeight = static_cast<float>(video->height * getScaleFactor());
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
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(video->width),
			                static_cast<GLsizei>(video->height), GL_RED, GL_UNSIGNED_BYTE,
			                video->pixels);

			assert(video->width % 2 == 0 && video->height % 2 == 0);
			glBindTexture(GL_TEXTURE_2D, textureU);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(video->width / 2),
			                static_cast<GLsizei>(video->height / 2), GL_RED, GL_UNSIGNED_BYTE,
			                video->pixels + static_cast<size_t>(video->width) * video->height);

			glBindTexture(GL_TEXTURE_2D, textureV);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(video->width / 2),
			                static_cast<GLsizei>(video->height / 2), GL_RED, GL_UNSIGNED_BYTE,
			                video->pixels + std::lround(1.25 * video->width * video->height));

			video = nullptr;
		}
		if (started()) {
			if (!audio) {
				audio = THEORAPLAY_getAudio(decoder);
			}
			while (audio) {
				queueAudio();
			}
		}
		if (shaderProgram) {
			auto _ = shaderProgram->use();
			glUniformMatrix3fv(modelviewUniform, 1, GL_FALSE, opengl::modelview.data);
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

	[[nodiscard]] bool isPlaying() const override {
		return THEORAPLAY_isDecoding(decoder);
	}

	~Impl() override {
		THEORAPLAY_stopDecode(decoder);
	}

	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;
	Impl(Impl&&) = delete;
	Impl& operator=(Impl&&) = delete;

	[[nodiscard]] int getWidth() const { return gsl::narrow<int>(video->width); }
	[[nodiscard]] int getHeight() const { return gsl::narrow<int>(video->height); }

	std::atomic_bool removeFromMixer{ false };

private:
	[[nodiscard]] bool started() const {
		return startTime > 0;
	}

	void rewind() override {
		assert(false);
	}

	void queueAudio() {
		{
			std::scoped_lock lock(audioBufferMutex);
			if (audio->channels == 1) {
				for (int i = 0; i < audio->frames; ++i) {
					audioBuffer.push_back(audio->samples[i]);
					audioBuffer.push_back(audio->samples[i]);
				}
			} else {
				assert(audio->channels == 2);
				audioBuffer.insert(audioBuffer.end(), audio->samples,
				                   audio->samples + static_cast<ptrdiff_t>(audio->frames * 2));
			}
		}
		audio = THEORAPLAY_getAudio(decoder);
	}

	std::size_t read(float * data, std::size_t sample_count) override {
		if (removeFromMixer) {
			return 0;
		}
		std::scoped_lock lock(audioBufferMutex);
		if (audioBuffer.size() < sample_count) {
			if (!isPlaying()) {
				return 0;
			}
			if (started()) {
				internal::warn("Audio buffer underrun!");
			}
			std::memset(data, 0, sample_count);
			return sample_count;
		}
		const auto begin = audioBuffer.begin();
		const auto end = begin + static_cast<long>(sample_count);
		std::copy(begin, end, data);
		audioBuffer.erase(begin, end);
		return sample_count;
	}

	std::mutex audioBufferMutex;
	std::vector<float> audioBuffer;

	constexpr static unsigned int BUFFER_SIZE = 200;

	THEORAPLAY_Decoder* decoder;
	std::unique_ptr<const THEORAPLAY_VideoFrame> video;
	std::unique_ptr<const THEORAPLAY_AudioPacket> audio;
	double startTime;
	double timePerFrame;

	std::unique_ptr<ShaderProgram> shaderProgram;
	int modelviewUniform = -1;
	GLuint textureY = 0; // luminance
	GLuint textureU = 0; // chrominance
	GLuint textureV = 0; // chrominance
	GLuint vao = 0;
	GLuint vertexBuffer = 0;
};

Video::Video(const std::string& filename) : impl(std::make_shared<Impl>(filename)) {
	if (impl->getFrequency() != audio::frequency) {
		Channel::main().add(
		    audio::pitch(impl, static_cast<float>(impl->getFrequency()) / audio::frequency));
	} else {
		Channel::main().add(impl);
	}
}

Video::~Video() {
	impl->removeFromMixer = true;
}

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
	return !impl->isPlaying();
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
