#version 450
layout(location = 0) in vec2 position;
layout(location = 0) out vec2 vPosition;
layout(push_constant) uniform Push {
	mat4 mvp;
	vec4 color;
	vec2 size;
	vec4 cornerRadii;
} pc;
void main() {
	gl_Position = pc.mvp * vec4(position, 0.0, 1.0);
	vPosition = position;
}
