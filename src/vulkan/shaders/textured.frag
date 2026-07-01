#version 450
layout(set = 0, binding = 0) uniform sampler2D tex;
layout(push_constant) uniform Push {
	mat4 mvp;
	vec4 color;
} pc;
layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 outColor;
void main() {
	outColor = texture(tex, texCoord) * pc.color;
}
