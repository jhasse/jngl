#version 300 es
// See ShaderProgramTest.cpp, which only uses the padding to make the uniforms 256 bytes big
uniform sampler2D tex;
uniform lowp vec4 color;
uniform lowp vec4 padding[15];

in mediump vec2 texCoord;

out lowp vec4 outColor;

void main() {
	outColor = vec4(color.rgb, texture(tex, texCoord).a);
}
