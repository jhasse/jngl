#version 300 es

uniform sampler2D tex;

in mediump vec2 texCoord;

out lowp vec4 outColor;

// 0 - no blur
// 1 - 3x3 blur
// 2 - 5x5 blur
// 3 - 7x7 blur
// ...
const int size = 3;

void main() {
	mediump float stepSizeX = 1. / 600.; // This should be 1/width
	mediump float stepSizeY = 1. / 300.; // and 1/height
	lowp vec4 sum = vec4(0.0);
	for (int x = -size; x <= size; x++) {
		for (int y = -size; y <= size; y++) {
			sum += texture(tex, vec2(texCoord.x + float(x) * stepSizeX,
			                         texCoord.y + float(y) * stepSizeY)) /
			       float((2 * size + 1) * (2 * size + 1));
		}
	}
	outColor = sum;
}
