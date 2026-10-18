#version 450
layout(set = 0, binding = 0) uniform sampler2D tex;
layout(push_constant) uniform Push {
	mat4 mvp;
	vec4 lerpColor;
	vec4 lerpFactors;
} pc;
layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 outColor;
void main() {
	outColor = mix(texture(tex, texCoord), vec4(pc.lerpColor.rgb, 0.0), pc.lerpFactors);
}
