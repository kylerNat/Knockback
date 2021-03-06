#version 330

#define pi 3.141592653589793238462643383279502884

smooth in vec2 pos;

uniform sampler2D tex;
uniform sampler2D wh;
uniform float time;
uniform mat4 trans_w;

const float res = 1000.0f;
const float tex_offset = 0.25f;
const float tex_scale = 0.5f;
const float n1 = 1.3330;

float rand(vec2 co){
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float wavy(vec2 c){
	//return 0.0;

	c += time*12.436;
	return sin(c.x*0.363)+sin(c.y*0.314)+sin(c.x*0.346)+sin(c.y*0.395);
	
	const float scale = 5.0f;
	/*if(mod(c.x, scale) < 2.0 || mod(c.x, scale) > scale-2.0 || mod(c.y, scale) < 2.0 || mod(c.y, scale) > scale-2.0){
		return -1.0f;
	}*/
	vec2 c_f = floor(c/scale)*scale;
	vec2 c_c = ceil(c/scale)*scale;
	return 
		10.0*(
			dot(vec2(c_c.x-c.x, c.x-c_f.x)
				*mat2(
					rand(c_f),                rand(vec2(c_c.x, c_f.y)),
					rand(vec2(c_f.x, c_c.y)), rand(c_c)
				)
				,vec2(c_c.y-c.y, c.y-c_f.y))
			/(scale*scale)
		);
}

vec4 tex2D(sampler2D tex, vec2 coord){
	return texture2D(tex, coord*tex_scale + tex_offset);
}

float beckmann(vec3 position, vec3 normal, vec3 lightIn, float m){
	if(m == 0.0){
		return 0.0;
	}

	float cosine = dot(normalize(normal), normalize(-normalize(position)+normalize(lightIn)) );//cosine between the normal and half vector
	float cosine2 = cosine*cosine;
	float sine2 = (1-cosine2);
	float tangent2 = sine2/cosine2;

	return exp(-tangent2/(m*m))/(pi*m*m*pow(cosine2, 2));
}

float reflectance(vec3 position, vec3 normal, vec3 lightIn){
	float n0 = 1.0;

	float cosineCamLight = dot(-normalize(position), lightIn);
	float cosineLightNorm = dot(normalize(normal), lightIn);
	float sine2 = (1-cosineLightNorm*cosineLightNorm);

	return (
		+0.5*cosineCamLight*pow(abs(
		(n0*cosineLightNorm-n1*sqrt(1-(n0/n1)*(n0/n1)*sine2))/
		(n0*cosineLightNorm+n1*sqrt(1-(n0/n1)*(n0/n1)*sine2))
		), 2)

		+0.5*cosineCamLight*pow(abs(
		(n1*cosineLightNorm-n0*sqrt(1-(n0/n1)*(n0/n1)*sine2))/
		(n1*cosineLightNorm+n0*sqrt(1-(n0/n1)*(n0/n1)*sine2))
		), 2)
	);
}

vec2 trans_coord(vec2 c){
	return (vec4(2.0*(c-0.5), 1.0, 1.0)*trans_w).xy*0.5*20.0/25.0+0.5;
}

void main(void) {
	vec2 p_t = gl_FragCoord.xy/res;
	vec2 norm = vec2(
 	      -texture(wh, trans_coord(p_t+vec2(-0.01,  0.01))).x +     texture(wh, trans_coord(p_t+vec2(0.01,  0.01))).x
      -2.0*texture(wh, trans_coord(p_t+vec2(-0.01,  0.0 ))).x + 2.0*texture(wh, trans_coord(p_t+vec2(0.01,  0.0 ))).x
	      -texture(wh, trans_coord(p_t+vec2(-0.01, -0.01))).x +     texture(wh, trans_coord(p_t+vec2(0.01, -0.01))).x,
		  
	      -texture(wh, trans_coord(p_t+vec2( 0.01, -0.01))).x +     texture(wh, trans_coord(p_t+vec2( 0.01, 0.01))).x
      -2.0*texture(wh, trans_coord(p_t+vec2( 0.0 , -0.01))).x + 2.0*texture(wh, trans_coord(p_t+vec2( 0.0 , 0.01))).x
	      -texture(wh, trans_coord(p_t+vec2(-0.01, -0.01))).x +     texture(wh, trans_coord(p_t+vec2(-0.01, 0.01))).x);
		  
	gl_FragColor.xyz = tex2D(tex, gl_FragCoord.xy/res).xyz;

	//refraction
	if(tex2D(tex, gl_FragCoord.xy/res).w >= 0.05f){
		norm = -0.75*vec2(sin(pos.y*20.0+time*5.0), sin(pos.x*20.0+time*5.0));
	}

	vec3 norm3 = 
		vec3(
			0.005*norm+0.000075*vec2(wavy(1000.0*pos.xy), wavy(1000.0*pos.yx)),
			-0.125f
		);

	float r = 1.0/n1;
	vec3 view = normalize(vec3(gl_FragCoord.xy/res - vec2(0.5, 0.5), 1.0));
	float c = dot(view, norm3);
	vec3 v_r = r*view + (r*c-sqrt(1.0-r*r*(1.0-c*c)))*norm3;

	vec2 texPos = gl_FragCoord.xy/res+0.1*v_r.xy/v_r.z*(1.0+texture(wh, trans_coord(p_t)).x);//-0.15*(0.005*norm+0.01*(gl_FragCoord.xy/res - vec2(0.5, 0.5))+0.00075*vec2(wavy(1000.0*pos.xy), wavy(1000.0*pos.yx)));//TODO: use actual refraction
	if(tex2D(tex, gl_FragCoord.xy/res).w <= 0.5f){
		if(true){//tex2D(tex, texPos).w <= 0.5f){
			gl_FragColor.xyz = tex2D(tex, texPos).xyz;
			gl_FragColor.xyz += 0.25*vec3(0.5);
			
			//gl_FragColor.xyz *= 0.75f;
			//gl_FragColor.xyz += 0.5*vec3(0.11, 0.42, 0.63);
		}
		gl_FragColor.xyz +=
			0.01*beckmann(
				view,
				norm3,
				vec3(1.0*(gl_FragCoord.xy/res - vec2(0.5, 0.5)), -0.5f),
				0.0125f
			);
		//reflection
		texPos = gl_FragCoord.xy/res+0.005*norm+0.05*(gl_FragCoord.xy/res - vec2(0.5, 0.5))+0.00075*vec2(wavy(1000.0*pos.xy), wavy(1000.0*pos.yx));
		if(tex2D(tex, texPos).w > 0.5f){
			gl_FragColor.xyz += tex2D(tex, texPos).xyz;//0.5*tex2D(tex, vec2(0.001*sin(gl_FragCoord.y*0.5), 0.001*sin(gl_FragCoord.x*0.5))+ gl_FragCoord.xy/res).xyz;
			//gl_FragColor.xyz *= 1.0-0.25*tex2D(tex, gl_FragCoord.xy/res+0.1*(gl_FragCoord.xy/res - vec2(0.5, 0.5))).w;
		}
		else {
			gl_FragColor.xyz += 0.25*vec3(0.5);
		}
		gl_FragColor.xyz *= vec3(0.5, 0.8, 2.0);

		if(tex2D(tex, gl_FragCoord.xy/res).w >= 0.125f){
			gl_FragColor.xyz *= 1.0+sin(time/0.05);
		}
	}
	//gl_FragColor.xyz = vec3(0.5);
	//gl_FragColor.xyz = 0.5*gl_FragColor.xyz+0.5*texture(wh, gl_FragCoord.xy/(res-20)).xxx;

	//gl_FragColor.xyz = 0.5*gl_FragColor.xyz-0.5*texture(wh, trans_coord(p_t)).xxx;

	//gl_FragColor.xyz = vec3(norm, 1.0);

	/*if(trans_coord(p_t).x < 0.0 || trans_coord(p_t).y < 0.0 || trans_coord(p_t).x > 1.0|| trans_coord(p_t).y > 1.0){
		gl_FragColor.xyz = vec3(1.0);
	}*/

	gl_FragColor.w = 1.0f;
	//gl_FragColor = tex2D(tex, gl_FragCoord.xy/res);
}
