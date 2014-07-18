#version 330

#define pi 3.141592653589793238462643383279502884

smooth in vec2 pos;

uniform sampler2D tex;
uniform float time;

const float res = 1000.0f;
const float tex_offset = 0.25f;
const float tex_scale = 0.5f;

float rand(vec2 co){
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float wavy(vec2 c){
	return sin(c.x*0.363)+sin(c.y*0.314)+sin(c.x*0.346)+sin(c.y*0.395);
}

vec4 tex2D(sampler2D tex, vec2 coord){
	return texture2D(tex, coord*tex_scale + tex_offset);
}

float beckmann(vec3 position, vec3 normal,vec3 lightIn, float m){
	if(m == 0.0){
		return 0.0;
	}

	float cosine = dot(normalize(normal), normalize(-normalize(position)+normalize(lightIn)) );//cosine between the normal and half vector
	float cosine2 = cosine*cosine;
	float sine2 = (1-cosine2);
	float tangent2 = sine2/cosine2;

	return exp(-tangent2/(m*m))/(pi*m*m*pow(cosine2, 2));
}

void main(void) {
	gl_FragColor.xyz = tex2D(tex, gl_FragCoord.xy/res).xyz;
	vec2 texPos = gl_FragCoord.xy/res-0.15*(0.005*vec2(sin(pos.y*20.0+time*5.0), sin(pos.x*20.0+time*5.0))+0.01*(gl_FragCoord.xy/res - vec2(0.5, 0.5))+0.00075*vec2(wavy(1000.0*pos.xy+time), wavy(1000.0*pos.yx+time)));
	if(tex2D(tex, gl_FragCoord.xy/res).w <= 0.5f){
		if(tex2D(tex, texPos).w <= 0.5f){
			gl_FragColor.xyz = tex2D(tex, texPos).xyz;
			
			//gl_FragColor.xyz *= 0.75f;
			//gl_FragColor.xyz += 0.5*vec3(0.11, 0.42, 0.63);

			gl_FragColor.xyz +=
				0.01*beckmann(
					vec3(gl_FragCoord.xy/res - vec2(0.5, 0.5), 1.0f),
					vec3(
						0.005*vec2(sin(pos.y*20.0+time*5.0), sin(pos.x*20.0+time*5.0))+0.0005*vec2(wavy(1000.0*pos.xy+time), wavy(1000.0*pos.yx+time)),
						-0.125f
					),
					vec3(1.0*(gl_FragCoord.xy/res - vec2(0.5, 0.5)), -0.5f),
					0.125f
				);
		}
		texPos = gl_FragCoord.xy/res+0.005*vec2(sin(pos.y*20.0+time*5.0), sin(pos.x*20.0+time*5.0))+0.05*(gl_FragCoord.xy/res - vec2(0.5, 0.5))+0.00075*vec2(wavy(1000.0*pos.xy+time), wavy(1000.0*pos.yx+time));
		if(tex2D(tex, texPos).w > 0.5f){
			gl_FragColor.xyz += tex2D(tex, texPos).xyz;//0.5*tex2D(tex, vec2(0.001*sin(gl_FragCoord.y*0.5), 0.001*sin(gl_FragCoord.x*0.5))+ gl_FragCoord.xy/res).xyz;
			//gl_FragColor.xyz *= 1.0-0.25*tex2D(tex, gl_FragCoord.xy/res+0.1*(gl_FragCoord.xy/res - vec2(0.5, 0.5))).w;
		}
		else {
			gl_FragColor.xyz += 0.25*vec3(0.5);
		}
	}
	gl_FragColor.w = 1.0f;
	//gl_FragColor = tex2D(tex, gl_FragCoord.xy/res);
}