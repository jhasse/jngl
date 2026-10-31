#version 300 es
// See ShaderProgramTest.cpp: declares its inputs and outputs in a different order than the
// built-in vertex shader and varyings.frag
in mediump vec2 inTexCoord;
in highp vec2 position;
uniform highp mat3 modelview;
uniform highp mat4 projection;
out lowp vec4 tint;
out mediump vec2 texCoord;

void main() {
	vec3 tmp = modelview * vec3(position, 1);
	gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
	texCoord = inTexCoord;
	tint = vec4(0.0, 0.0, 0.0, 1.0);
}
