#version 300 es
// See ShaderProgramTest.cpp: declares its sampler twice, of which the preprocessor keeps one
#ifdef GL_ES
uniform mediump sampler2D tex;
#else
uniform sampler2D tex;
#endif

in mediump vec2 texCoord;

out lowp vec4 outColor;

void main() {
	outColor = vec4(0.0, 0.0, 0.0, texture(tex, texCoord).a);
}
