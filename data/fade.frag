#version 300 es
uniform sampler2D tex;

in mediump vec2 texCoord;

out lowp vec4 outColor;

// The mesh's texture coordinates range from 0 to these values. We use them to
// normalize the position across the whole mesh for the edge fade.
const mediump vec2 meshSize = vec2(3.0, 2.4);

// Opaque region of the image we sample from (the center of the jngl logo),
// tiled across the mesh so there are no transparent pixels to begin with.
const mediump vec2 start = vec2(0.43, 0.1);
const mediump vec2 end = vec2(0.61, 0.9);

// How much of the mesh (relative to its size) the fade covers on each side.
const mediump float fadeWidth = 0.25;

void main() {
	lowp vec4 color = texture(tex, vec2(
		fract(texCoord.x) * (end.x - start.x) + start.x,
		fract(texCoord.y) * (end.y - start.y) + start.y
	));

	// Distance to the nearest edge of the mesh (0 at the edge, 0.5 in the
	// center).
	mediump vec2 pos = texCoord / meshSize;
	mediump vec2 distToEdge = min(pos, 1.0 - pos);

	// Fade from 0 at the edge to 1 once we're fadeWidth away from it.
	mediump float fade = smoothstep(0.0, fadeWidth, distToEdge.x) *
	                     smoothstep(0.0, fadeWidth, distToEdge.y);

	outColor = vec4(color.rgb, color.a * fade);
}
