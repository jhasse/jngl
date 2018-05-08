#version 130

uniform sampler2D tex;

// 0 - no blur
// 1 - 3x3 blur
// 2 - 5x5 blur
// 3 - 7x7 blur
// ...
const int size = 3;

void main() {
	float stepSizeX = 1.0 / textureSize(tex, 0).x;
	float stepSizeY = 1.0 / textureSize(tex, 0).y;
	vec4 sum = vec4(0.0);
	for (int x = -size; x <= size; x++) {
		for (int y = -size; y <= size; y++) {
			sum += texture(tex, vec2(gl_TexCoord[0].x + x * stepSizeX,
			                         gl_TexCoord[0].y + y * stepSizeY)) /
			       ((2 * size + 1) * (2 * size + 1));
		}
	}
	gl_FragColor = sum;
}
