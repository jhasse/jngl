#version 300 es
// See ShaderProgramTest.cpp: big enough for the uniforms to need more than 256 bytes
uniform sampler2D tex;
uniform lowp vec4 colors[32];
uniform int index;

in mediump vec2 texCoord;

out lowp vec4 outColor;

void main() {
	outColor = vec4(colors[index].rgb, texture(tex, texCoord).a);
}
