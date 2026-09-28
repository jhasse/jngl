#version 450
layout(push_constant) uniform Push {
	mat4 mvp;
	vec4 color;
} pc;
layout(location = 0) out vec4 outColor;
void main() {
	outColor = pc.color;
}
