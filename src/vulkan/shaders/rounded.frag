#version 450
layout(location = 0) in vec2 vPosition;
layout(push_constant) uniform Push {
	mat4 mvp;
	vec4 color;
	vec2 size;
	vec4 cornerRadii;
} pc;
layout(location = 0) out vec4 outColor;

float sdRoundBox(vec2 p, vec2 b, vec4 r) {
	r.xy = (p.x > 0.0) ? r.xy : r.zw;
	r.x = (p.y > 0.0) ? r.y : r.x;
	vec2 q = abs(p) - b + r.x;
	return min(max(q.x, q.y), 0.0) + length(max(q, vec2(0.0))) - r.x;
}

void main() {
	vec2 p = vPosition * pc.size;
	vec2 halfSize = pc.size * 0.5;
	vec4 r = vec4(pc.cornerRadii.y, pc.cornerRadii.w, pc.cornerRadii.x, pc.cornerRadii.z);
	float d = sdRoundBox(p, halfSize, r);
	float alpha = 1.0 - smoothstep(-0.5, 0.5, d);
	outColor = vec4(pc.color.rgb, pc.color.a * alpha);
}
