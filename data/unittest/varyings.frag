#version 300 es
// See varyings.vert
uniform sampler2D tex;

in mediump vec2 texCoord;
in lowp vec4 tint;

out lowp vec4 outColor;

void main() {
	outColor = vec4(tint.rgb, texture(tex, texCoord).a);
}
