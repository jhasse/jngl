#version 450
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 inTexCoord;
layout(push_constant) uniform Push {
	mat4 mvp;
	vec4 color;
} pc;
layout(location = 0) out vec2 texCoord;
void main() {
	gl_Position = pc.mvp * vec4(position, 0.0, 1.0);
	texCoord = inTexCoord;
}
