#include "Game.h"
/*TODO:
	multiple weapons
	levels
	enemies
*/

int map_s(int id, int x, int y){
	x += 5;
	y += 5;
	if(x < 0 || y < 0 || x >= sizeof(maps[0])/sizeof(maps[0][0])|| y >= sizeof(maps[0][0])/sizeof(maps[0][0][0])){
		return 0;
	}
	return maps[id][y][x];
}

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
	renderObject(sub(w.plr.r, w.cam.r), w.plr.dir, modId_player, renderArgs);
	
	for(int i = 0; i < sizeof(w.bs)/sizeof(w.bs[0]); i++){
		if(w.bs[i].alive){
			renderObject(sub(w.bs[i].r, w.cam.r), scale(normalize(w.bs[i].v), 1.0), modId_bullet, renderArgs);
		}
	}

	float2 r_t = complexx(scale(sub(w.plr.r, w.m.r), 1.0f/block_scale), float2(cos(w.m.theta), sin(w.m.theta)));//transform the particle from the world space to map space
	int x_p = (int)round(r_t[0]);
	int y_p = (int)round(r_t[1]);

	renderObject(sub(add(w.m.r, complexx(float2(x_p*block_scale, y_p*block_scale), float2(cos(w.m.theta), -sin(w.m.theta)))), w.cam.r), float2(sin(w.m.theta), cos(w.m.theta)), modId_crosshair, renderArgs);

	for(int x = 0; x < sizeof(maps[0])/sizeof(maps[0][0]); x++){
		for(int y = 0; y < sizeof(maps[0][0])/sizeof(maps[0][0][0]); y++){
			x -= 5;
			y -= 5;
			renderObject(sub(add(w.m.r, complexx(float2(x*block_scale, y*block_scale), float2(cos(w.m.theta), -sin(w.m.theta)))), w.cam.r), float2(sin(w.m.theta)*(block_scale+block_margin), cos(w.m.theta)*(block_scale+block_margin)), (modId) map_s(w.m.id, x, y), renderArgs);
			x += 5;
			y += 5;

			if(x == x_p && y == y_p){
				renderObject(sub(add(w.m.r, complexx(float2(x_p*block_scale, y_p*block_scale), float2(cos(w.m.theta), -sin(w.m.theta)))), w.cam.r), float2(sin(w.m.theta), cos(w.m.theta)), modId_crosshair, renderArgs);
			}
		}
	}

	renderObject(w.mousePos, float2(0.0f, 1.0f), modId_crosshair, renderArgs);
}

world createWorld(){//make constuctor?
	srand((unsigned) time(NULL));
	
	world w = {0};

	w.m.r = float2(-0.0, -0.0);
	w.m.theta = 0.0;

	/*
	w.gun.fireWait = 0.0015f;
	w.gun.spread = 0.5f;
	w.gun.offset = float2(0.1f, 0.0f);
	*/
	w.gun.fireWait = 0.5f;
	w.gun.spread = 0.01f;
	w.gun.offset = float2(0.1f, 0.0f);
	
	w.plr.m = 1.0;
	return w;
}

void collideMap(world & w, float m, float2 & r, float2 & v, float radius){//TODO(if necessary):
	float2 r_t = complexx(scale(sub(r, w.m.r), 1.0f/block_scale), float2(cos(w.m.theta), sin(w.m.theta)));//transform the particle from the world space to map space
	int x = (int)round(r_t[0]);
	int y = (int)round(r_t[1]);
	//if(x < 0 || y < 0 || x >= sizeof(maps[0])/sizeof(maps[0][0])|| y >= sizeof(maps[0][0])/sizeof(maps[0][0][0])){
	//	return;
	//}
	radius = radius/block_scale;
	float2 r_c = r_t;//in the map space
	if(map_s(w.m.id, x, y) == 1){
		if(map_s(w.m.id, y, x-1) == 1){//abs(r_t[0]-x) < abs(r_t[1]-y)){//TODO: use somthing else(maybe velocity?) to figure out which side to use?
			r_c = float2(r_t[0], y+(r_t[1] > y ? 0.5f+radius : -0.5f-radius));
		}
		else{
			r_c = float2(x+(r_t[0] > x ? 0.5f+radius : -0.5f-radius), r_t[1]);;
		}
	}
	float v_b;
	if(abs(r_t[0]-x)+radius >= 0.5 && map_s(w.m.id, x+normalize(r_t[0]-x), y) == 1){
		r_c[0] = x-normalize(x-r_t[0])*(0.5-radius);
		if(projAbs(v, float2(cos(w.m.theta), -sin(w.m.theta)))*normalize(x-r_t[0]) < 0.0){
			float Jxr = abs(cross(float2(normalize(x-r_t[0]), 0.0f), r_c));
			v_b =
				(-v_t[0] + Jxr*w.m.omega)/
				(2*(1.0f + sq(Jxr)*w.m.m/w.m.I + w.m.m/m));

			//v = sub(v, scale(proj(v, float2(cos(w.m.theta), -sin(w.m.theta))), 2.0));
		}
	}
	if(abs(r_t[1]-y)+radius >= 0.5 && map_s(w.m.id, x, y+normalize(r_t[1]-y)) == 1){
		r_c[1] = y-normalize(y-r_t[1])*(0.5-radius);
		if(projAbs(v, float2(sin(w.m.theta), cos(w.m.theta)))*normalize(y-r_t[1]) < 0.0){
			v = sub(v, scale(proj(v, float2(sin(w.m.theta), cos(w.m.theta))), 2.0));
		}
	}
	r = add(w.m.r, complexx(scale(r_c, block_scale), float2(cos(w.m.theta), -sin(w.m.theta))));
}

//TODO: set accelerations first, then update velocities and positions, otherwise some objects are unsyncronized, which causes problems at low framerates or high speeds
void worldLoop(world & w, float dt){//make member function for convinience?
	//w.mousePos = float2(1.0f/(1.0f*500.0f)*input::mouse[0], -1.0f/(1.0f*500.0f)*input::mouse[1]);
	w.mousePos = float2(0.0, 0.0);
	//float2 plr_dv = float2(0.0, 0.0);

	//w.m.theta += -dt*0.01;
	
	{
		float2 inDir = float2(
			-1.0f*float(input::pressed('A')) + 1.0f*float(input::pressed('D')),
			-1.0f*float(input::pressed('S')) + 1.0f*float(input::pressed('W'))
		);

		float speed = 2.0f;
		if(input::pressed(VK_SHIFT)){
			speed = 50.0f;
		}

		inDir = scale(normalize(inDir), speed);
		const float aMag = 100.0f;
		const float drag = 50.0f;
		float2 velDif = sub(inDir, w.plr.v);
		w.plr.dv = add(w.plr.dv, scale(w.plr.v, -drag*dt));
		if(dotMe(sub(velDif, w.plr.dv)) >= pow(aMag*dt, 2)){
			w.plr.dv = add(scale(normalize(velDif), aMag*dt), w.plr.dv);
			//w.plr.v = add(w.plr.v, w.plr.dv);
		}
		else{
			w.plr.dv = velDif;		
			//w.plr.v = add(w.plr.v, plrAccel);
			//w.plr.v = inDir;
		}
		w.plr.r = add(scale(w.plr.dv, dt/2), add(w.plr.r, scale(w.plr.v, dt)));
		w.plr.v = add(w.plr.v, w.plr.dv);

		//w.cam.r = add(w.plr.r, scale(w.mousePos, 0.3f));
		w.plr.dir = normalize(w.mousePos);
		w.plr.dv = float2(0.0, 0.0);
		collideMap(w, w.plr.m, w.plr.r, w.plr.v, 0.05f);
	}

	for(int i = 0; i < sizeof(w.bs)/sizeof(w.bs[0]); i++){
		auto a = w.bs[i];
		if(w.bs[i].alive){
			w.bs[i].r = add(w.bs[i].r, scale(w.bs[i].v, dt));
			if(dotMe(sub(w.bs[i].r, w.plr.r)) > 10.0){
				w.bs[i].alive = false;
			}

			collideMap(w, 1.0, w.bs[i].r, w.bs[i].v, 0.05f);
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
				float2 relV = scale(float2(sin(theta), cos(theta)), 1.5);
				w.bs[i].v = add(relV, w.plr.v);

				//knockback: w.plr.v = sub(w.plr.v, scale(w.plr.dir, 0.5f));//sub(w.plr.v, scale(relV, 0.152f));

				w.cam.v = add(w.cam.v, add(scale(randF2, 1.0f), scale(relV, -0.2f)));
		
				w.gun.fireTimer += w.gun.fireWait;
			}
		}
		if(w.gun.fireTimer <= 0.0){//the bullet limit has been reached
			w.gun.fireTimer = w.gun.fireWait;
		}
	}

	if(w.gun.fireTimer >= 0.0){
		w.gun.fireTimer -= dt;
	}
	else {
		w.gun.fireTimer = 0.0f;
	}

	{//x = e^(-gamma*t)*A*cos(omega*t + theta) //damped shm //consistaint under different frame rates
		const float omega = 100.0f;
		const float gamma = 20.0f;
		for(int i = 0; i < 2; i++){//having each axis "springed" separatly feels better than a single spring attached to the center //jk it should be the same for a linear spring
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