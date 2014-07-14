#version 330

uniform sampler2D tex;

void main(void) {
	gl_FragColor.xyz = texture2D(tex, gl_FragCoord.xy/1000.0f).xyz;
	if(texture2D(tex, gl_FragCoord.xy/1000.0f).w <= 0.5f){
		gl_FragColor.xyz += 0.5*texture2D(tex, gl_FragCoord.xy/1000.0f+vec2(0.01*sin(gl_FragCoord.y*0.1), 0.01*sin(gl_FragCoord.x*0.1))+0.1*(gl_FragCoord.xy/1000.0f - vec2(0.5, 0.5))).xyz;//0.5*texture2D(tex, vec2(0.001*sin(gl_FragCoord.y*0.5), 0.001*sin(gl_FragCoord.x*0.5))+ gl_FragCoord.xy/1000.0f).xyz;
	}
	gl_FragColor.w = 1.0f;
}