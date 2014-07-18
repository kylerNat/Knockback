#version 330

layout (location = 0) in vec2 p;

smooth out vec2 pos;

uniform vec2 center;

void main() {
	pos = p+center;
	gl_Position = vec4(p, 0.0, 1.0);
}