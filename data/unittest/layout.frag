#version 300 es
// See ShaderProgramTest.cpp: it's only black if strength and scale are where setUniform puts them
uniform sampler2D tex;
uniform lowp vec3 tint;
uniform lowp float strength;
uniform mediump mat2 unused;
uniform lowp float scale;

in mediump vec2 texCoord;

out lowp vec4 outColor;

void main() {
	outColor = vec4(vec3(1.0) - tint * strength * scale, texture(tex, texCoord).a);
}
