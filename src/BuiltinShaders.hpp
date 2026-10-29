// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// The GLSL of JNGL's built-in shaders, for all backends.
///
/// The OpenGL backend compiles them at runtime (see ShaderCache), falling back to the GLES 2
/// versions where "#version 300 es" isn't supported. For the Vulkan and Direct3D 12 backends,
/// jngl-shaderc compiles them at build time like a game's shaders, see src/shaderc/main.cpp.
/// @file
#pragma once

namespace jngl::builtin_shaders {

/// Single-color shapes, e.g. jngl::drawRect
inline constexpr const char* coloredVertex = R"(#version 300 es
	in mediump vec2 position;
	uniform highp mat3 modelview;
	uniform mediump mat4 projection;

	void main() {
		vec3 tmp = modelview * vec3(position, 1);
		gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
	})";

inline constexpr const char* coloredVertexGles2 = R"(#version 100
	attribute mediump vec2 position;
	uniform highp mat3 modelview;
	uniform mediump mat4 projection;

	void main() {
		vec3 tmp = modelview * vec3(position, 1);
		gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
	})";

inline constexpr const char* coloredFragment = R"(#version 300 es
	uniform lowp vec4 color;
	out lowp vec4 outColor;

	void main() {
		outColor = color;
	})";

inline constexpr const char* coloredFragmentGles2 = R"(#version 100
	uniform lowp vec4 color;

	void main() {
		gl_FragColor = color;
	})";

/// See jngl::drawRoundedRect
inline constexpr const char* roundedVertex = R"(#version 300 es
	in mediump vec2 position;
	uniform highp mat3 modelview;
	uniform mediump mat4 projection;
	out mediump vec2 vPosition;

	void main() {
		vec3 tmp = modelview * vec3(position, 1);
		gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
		vPosition = position;
	})";

inline constexpr const char* roundedVertexGles2 = R"(#version 100
	attribute mediump vec2 position;
	uniform highp mat3 modelview;
	uniform mediump mat4 projection;
	varying mediump vec2 vPosition;

	void main() {
		vec3 tmp = modelview * vec3(position, 1);
		gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
		vPosition = position;
	})";

inline constexpr const char* roundedFragment = R"(#version 300 es
	uniform lowp vec4 color;
	uniform mediump vec2 size;
	uniform mediump vec4 cornerRadii;
	in mediump vec2 vPosition;
	out lowp vec4 outColor;

	mediump float sdRoundBox(mediump vec2 p, mediump vec2 b, mediump vec4 r) {
		r.xy = (p.x > 0.0) ? r.xy : r.zw;
		r.x  = (p.y > 0.0) ? r.y  : r.x;
		mediump vec2 q = abs(p) - b + r.x;
		return min(max(q.x, q.y), 0.0) + length(max(q, vec2(0.0))) - r.x;
	}

	void main() {
		mediump vec2 p = vPosition * size;
		mediump vec2 halfSize = size * 0.5;
		mediump vec4 r = vec4(cornerRadii.y, cornerRadii.w, cornerRadii.x, cornerRadii.z);
		mediump float d = sdRoundBox(p, halfSize, r);
		mediump float alpha = 1.0 - smoothstep(-0.5, 0.5, d);
		outColor = vec4(color.rgb, color.a * alpha);
	})";

inline constexpr const char* roundedFragmentGles2 = R"(#version 100
	uniform lowp vec4 color;
	uniform mediump vec2 size;
	uniform mediump vec4 cornerRadii;
	varying mediump vec2 vPosition;

	mediump float sdRoundBox(mediump vec2 p, mediump vec2 b, mediump vec4 r) {
		r.xy = (p.x > 0.0) ? r.xy : r.zw;
		r.x  = (p.y > 0.0) ? r.y  : r.x;
		mediump vec2 q = abs(p) - b + r.x;
		return min(max(q.x, q.y), 0.0) + length(max(q, vec2(0.0))) - r.x;
	}

	void main() {
		mediump vec2 p = vPosition * size;
		mediump vec2 halfSize = size * 0.5;
		mediump vec4 r = vec4(cornerRadii.y, cornerRadii.w, cornerRadii.x, cornerRadii.z);
		mediump float d = sdRoundBox(p, halfSize, r);
		mediump float alpha = 1.0 - smoothstep(-0.5, 0.5, d);
		gl_FragColor = vec4(color.rgb, color.a * alpha);
	})";

/// Sprites, text and so on, also of ShaderPrograms that use Sprite::vertexShader()
inline constexpr const char* texturedVertex = R"(#version 300 es
	in mediump vec2 position;
	in mediump vec2 inTexCoord;
	uniform highp mat3 modelview;
	uniform mediump mat4 projection;
	out mediump vec2 texCoord;

	void main() {
		vec3 tmp = modelview * vec3(position, 1);
		gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
		texCoord = inTexCoord;
	})";

inline constexpr const char* texturedVertexGles2 = R"(#version 100
	attribute mediump vec2 position;
	attribute mediump vec2 inTexCoord;
	uniform highp mat3 modelview;
	uniform mediump mat4 projection;
	varying mediump vec2 texCoord;

	void main() {
		vec3 tmp = modelview * vec3(position, 1);
		gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
		texCoord = inTexCoord;
	})";

/// Each channel of the texture mixed towards lerpColor (and an alpha of 0) by lerpFactors
inline constexpr const char* texturedFragment = R"(#version 300 es
	uniform sampler2D tex;
	uniform lowp vec3 lerpColor;
	uniform lowp vec4 lerpFactors;

	in mediump vec2 texCoord;

	out lowp vec4 outColor;

	void main() {
		outColor = mix(texture(tex, texCoord), vec4(lerpColor, 0.0), lerpFactors);
	})";

inline constexpr const char* texturedFragmentGles2 = R"(#version 100
	uniform sampler2D tex;
	uniform lowp vec3 lerpColor;
	uniform lowp vec4 lerpFactors;

	varying mediump vec2 texCoord;

	void main() {
		gl_FragColor = mix(texture2D(tex, texCoord), vec4(lerpColor, 0.0), lerpFactors);
	})";

/// jngl::Video's frames on the GPU backends, whose ShaderPrograms sample a single texture: each of
/// its texels has the Y, U and V of its pixel in R, G and B. Used with texturedVertex.
inline constexpr const char* videoFragment = R"(#version 300 es
	uniform sampler2D tex;

	in mediump vec2 texCoord;

	out lowp vec4 outColor;

	void main() {
		lowp vec3 yuv = texture(tex, texCoord).rgb;
		lowp float y = 1.1643 * (yuv.r - 0.0625);
		lowp float u = yuv.g - 0.5;
		lowp float v = yuv.b - 0.5;
		outColor = vec4(
			y + 1.5958 * v,
			y - 0.39173 * u - 0.81290 * v,
			y + 2.017 * u,
			1.0
		);
	})";

} // namespace jngl::builtin_shaders
