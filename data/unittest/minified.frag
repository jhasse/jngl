#version 300 es
precision mediump float;uniform sampler2D tex;uniform lowp vec4 color,weights[2];in mediump vec2 texCoord;out lowp vec4 outColor;void main(){lowp vec4 u=weights[1];outColor=vec4(vec3(1.)-color.xyz*u.xyz,texture(tex,texCoord).w);}
