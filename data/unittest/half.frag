#version 300 es
uniform sampler2D tex;

in mediump vec2 texCoord;

out mediump vec4 outColor;

void main() {
	mediump vec4 color = texture(tex, texCoord);
	outColor = vec4(color.rgb * 0.5, color.a);
}
