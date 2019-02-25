#version 300 es
uniform sampler2D tex;

in mediump vec2 texCoord;

out lowp vec4 outColor;

void main() {
	const mediump vec2 start = vec2(0.43, 0.1);
	const mediump vec2 end = vec2(0.61, 0.9);
	outColor = texture(tex, vec2(
		fract(texCoord.x) * (end.x - start.x) + start.x,
		fract(texCoord.y) * (end.y - start.y) + start.y
	));
}
