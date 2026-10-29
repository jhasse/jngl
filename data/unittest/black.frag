#version 300 es
uniform sampler2D tex;

in mediump vec2 texCoord;

out lowp vec4 outColor;

void main() {
	outColor = vec4(0.0, 0.0, 0.0, texture(tex, texCoord).a);
}
