#version 330

layout (location = 0) in vec3 p;
layout (location = 1) in vec3 c;

smooth out vec3 pos;
smooth out vec3 color;

uniform mat4 t;
uniform mat4 t1;

float choose(int n, int k){
	float nCk = 1.0;
	for(int i = 1; i <= n-k; i++) {
		nCk *= float(i+k)/float(i);
	}
	return nCk;
}

float b(vec4 c, float t){
	float a = 0;
	const int deg = 3;
	for(int i = 0; i <= deg; i++) {
		a += choose(deg, i)*pow(t, i)*pow(1.0-t, deg-i)*c[i];
	}
	return a;
}

void main() {
	float x = (p.y+0.05)*10.0;
	mat4 t0 = x*t + (1-x)*t1;

	gl_Position.xyz = p;
	gl_Position.w = 1.0;
	gl_Position = gl_Position*t0;

	pos = gl_Position.xyz;
	color = c;
}
