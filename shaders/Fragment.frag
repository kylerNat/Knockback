#version 330

smooth in vec3 pos;
smooth in vec3 color;

#define pi 3.1415926535897932384626433832795

void main() {
	vec2 a = floor(pos.xy*50000.0);
	float b = (3.0-cos(a.x*1.0*pi)*cos(a.y*1.0*pi))/4.0;
	gl_FragColor = vec4(vec3(1.0, 1.0, 1.0)*b, 1.0);
	if(pos == vec3(0.0)){
		gl_FragColor = vec4(color, 1.0);
	}
}