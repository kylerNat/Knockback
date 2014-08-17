#version 330

layout (location = 0) in vec3 p;
layout (location = 1) in vec3 c;

smooth out vec3 pos;
smooth out vec3 color;

uniform mat4 t;

void main() {
	gl_Position.xyz = p;
	gl_Position.w = 1.0;
	gl_Position = gl_Position*t;
	gl_Position.xy *= 0.25;

	pos = p;
	if(abs(t[1][0]) < 2.0){
		pos = vec3(0.0, 0.0, 0.0);
	}
	color = c;
}