#include "Game.h"

void renderObject(float2 pos, float2 dir, modId model, vertexObject * vOs, GLuint trans){
	float4x4 transform = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};

	transform[0][3] = round(pos[0]*500.0f)/500.0f;
	transform[1][3] = round(pos[1]*500.0f)/500.0f;
				
	transform[0][0] = dir[1];
	transform[0][1] = dir[0];
	transform[1][0] = -dir[0];
	transform[1][1] = dir[1];


	bindVO(vOs[model]);
	glUniformMatrix4fv(trans, 1, false, (float*)&transform[0][0]);
	glDrawElements(GL_TRIANGLES, vOs[model].data.indexSize, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void renderWorld(world w, vertexObject * vOs, GLuint trans){
	renderObject(sub(w.plr.r, w.cam.r), w.plr.dir, modId_player, vOs, trans);
	
	for(int i = 0; i < sizeof(w.bs)/sizeof(w.bs[0]); i++){
		if(w.bs[i].alive){
			renderObject(sub(w.bs[i].r, w.cam.r), scale(normalize(w.bs[i].v), 1.0), modId_bullet, vOs, trans);
		}
	}

	renderObject(w.mousePos, float2(0.0, 1.0), modId_crosshair, vOs, trans);
}

world createWorld(){//make constuctor?
	srand((unsigned) time(NULL));
	
	world w = {0};
	
	w.gun.fireWait = 0.0015f;
	w.gun.spread = 0.5f;
	w.gun.offset = float2(0.1f, 0.0f);

	w.plr.m = 1.0;
	return w;
}

void playerControl(world& w, float dt, float2 & plr_dv){
	float2 inDir = float2(
		-1.0f*float(input::pressed('A')) + 1.0f*float(input::pressed('D')),
		-1.0f*float(input::pressed('S')) + 1.0f*float(input::pressed('W'))
	);

	float speed = 2.0f;
	if(input::pressed(VK_SHIFT)){
		speed = 50.0f;
	}

	inDir = scale(normalize(inDir), speed);
	const float aMag = 200.0f;
	float2 velDif = sub(inDir, w.plr.v);
	if(dotMe(velDif) >= pow(aMag*dt, 2)){
		plr_dv = add(scale(normalize(velDif), aMag*dt), plr_dv);
		w.plr.v = add(w.plr.v, plr_dv);
	}
	else{
		plr_dv = sub(inDir, w.plr.v);		
		//w.plr.v = add(w.plr.v, plrAccel);
		w.plr.v = inDir;
	}
}
	
void playerMove(world& w, float dt, float2 & plr_dv){
	w.plr.r = add(scale(plr_dv, dt/2), add(w.plr.r, scale(w.plr.v, dt)));

	//w.cam.r = add(w.plr.r, scale(w.mousePos, 0.3f));
	w.plr.dir = normalize(w.mousePos);
}

//TODO: set accelerations first, then update velocities and positions, otherwise some objects are unsyncronized, which causes problems at low framerates or high speeds
//TODO: make behavior identical under dfferent frame rates
//BUG: weird gun knockback behavior when switching vsync on and off
void worldLoop(world & w, float dt){//make member function for convinience?
	w.mousePos = float2(1.0f/(1.0f*500.0f)*input::mouse[0], -1.0f/(1.0f*500.0f)*input::mouse[1]);
	float2 plr_dv = float2(0.0, 0.0);
	float playerTime = dt;

	float tBeforeFire = clamp(fmod(-w.gun.fireTimer, w.gun.fireWait), 0.0, dt);
	playerTime -= tBeforeFire;

	float test = tBeforeFire;

	playerControl(w, tBeforeFire, plr_dv);
	playerMove(w, tBeforeFire, plr_dv);
	
	if(w.gun.fireTimer >= 0.0){
		w.gun.fireTimer -= dt;
	}
	else {
		w.gun.fireTimer = 0.0f;
	}

	for(int i = 0; i < sizeof(w.bs)/sizeof(w.bs[0]); i++){
		auto a = w.bs[i];
		if(w.bs[i].alive){
			w.bs[i].r = add(w.bs[i].r, scale(w.bs[i].v, dt));
			if(dotMe(sub(w.bs[i].r, w.plr.r)) > 10.0){
				w.bs[i].alive = false;
			}
		}
	}

	if(input::pressed(VK_LBUTTON)){
		for(int i = 0; i < sizeof(w.bs)/sizeof(w.bs[0]) && w.gun.fireTimer <= 0.0; i++){
			if(!w.bs[i].alive){
				w.bs[i].alive = true;
				w.bs[i].r = add(add(w.plr.r, scale(perp(w.plr.dir), -w.gun.offset[1])), scale(w.plr.dir, w.gun.offset[0]));
				float2 randF2 = normalize(float2(2.0f*(random()-0.5f), 2.0f*(random()-0.5f)));
				float theta = atan2(w.plr.dir[0], w.plr.dir[1]);
				theta += w.gun.spread*2.0f*(random()-0.5f);//TODO: make gaussian distrobution
				float2 relV = scale(float2(sin(theta), cos(theta)), 4.0);
				w.bs[i].v = add(relV, w.plr.v);

				playerTime -= w.gun.fireWait;
				float timeAfter = w.gun.fireWait;
				if(playerTime < 0.0){
					timeAfter += playerTime;
					playerTime = 0.0;
				}
				test += timeAfter;
				playerControl(w, timeAfter, plr_dv);
				w.plr.v = sub(w.plr.v, scale(w.plr.dir, 1.0));//sub(w.plr.v, scale(relV, 0.152f));
				playerMove(w, timeAfter, plr_dv);
				
				//w.cam.v = add(w.cam.v, add(scale(randF2, 1.0f), scale(relV, -0.2f)));
		
				w.gun.fireTimer += w.gun.fireWait;
			}
		}
		if(w.gun.fireTimer <= 0.0){//the bullet limit has been reached
			w.gun.fireTimer = w.gun.fireWait;
		}
	}

	test += playerTime;
	playerControl(w, playerTime, plr_dv);
	playerMove(w, playerTime, plr_dv);

	if(input::pressed('L')){
		_asm nop;
	}

	{//x = e^(-gamma*t)*A*cos(omega*t + theta) //damped shm //consistaint under different frame rates
		const float omega = 100.0f;
		const float gamma = 20.0f;
		for(int i = 0; i < 2; i++){//having each axis "springed" separatly feels better than a single sprin attached to the center
			float x = w.cam.rr[i];
			float v = w.cam.v[i];
			float theta = atan2(-v-x*gamma, omega*x);
			float A = sqrt(pow(x, 2) + pow((v+x*gamma)/omega, 2));//by math, not by cons. of energy; A != the undampened amplitude
			x = exp(-gamma*dt)*A*cos(omega*dt+theta);
			v = -x*gamma -exp(-gamma*dt)*omega*A*sin(omega*dt+theta);
			w.cam.rr[i] = x;
			w.cam.v[i] = v;
		}
		
		w.cam.r = add(add(w.plr.r, scale(w.mousePos, 0.3f)), w.cam.rr);
	}
}