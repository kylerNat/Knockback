#version 330

smooth in vec3 pos;
smooth in vec3 color;

#define pi 3.1415926535897932384626433832795

void main() {
	gl_FragColor = vec4(color, 1.0);
}