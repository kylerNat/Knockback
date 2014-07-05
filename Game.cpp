#include "Game.h"
/*TODO:
	multiple weapons
	levels
	enemies
*/

int map_s(int id, int x, int y){
	x += (sizeof(maps[0])/sizeof(maps[0][0])-1)/2;
	y = -y;
	y += ((sizeof(maps[0][0])/sizeof(maps[0][0][0]))-1)/2;
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
			renderObject(sub(w.bs[i].r, w.cam.r), scale(normalize(w.bs[i].v), 0.75), modId_bullet, renderArgs);
		}
	}

	for(int i = 0; i < sizeof(w.es)/sizeof(w.es[0]); i++){
		renderObject(sub(w.es[i].r, w.cam.r), w.es[i].dir, modId_player, renderArgs);
	}


	for(int x = -(map_size_x-1)/2; x < map_size_x+(map_size_x-1)/2; x++){
		for(int y = -(map_size_y-1)/2; y < map_size_y+(map_size_y-1)/2; y++){
			renderObject(sub(add(w.m.r, complexx(float2(x*block_scale, y*block_scale), float2(cos(w.m.theta), -sin(w.m.theta)))), w.cam.r), float2(sin(w.m.theta)*(block_scale+block_margin), cos(w.m.theta)*(block_scale+block_margin)), (modId) map_s(w.m.id, x, y), renderArgs);
		}
	}

	renderObject(w.mousePos, float2(0.0f, 1.0f), modId_crosshair, renderArgs);
}

world createWorld(){//make constuctor?
	srand((unsigned) time(NULL));
	
	world w = {0};

	w.m.r = float2(-0.0, -0.0);
	w.m.theta = 0.0;
	w.m.omega = 0.0;
	w.m.m = 100.0;
	w.m.I = 100.0;

	/*
	w.gun.fireWait = 0.0015f;
	w.gun.spread = 0.5f;
	w.gun.offset = float2(0.1f, 0.0f);
	*/
	
	w.gun.fireWait = 0.1f;
	w.gun.spread = 0.05f;
	w.gun.offset = float2(0.1f, 0.0f);
	

	w.plr.m = 1.0;
	return w;
}

void collideWall(float2 r_c, float2 n, world & w, float m, float2 & r, float2 & v){
	float Jxr = cross(n, r_c);

	float2 n_ws = normalize(complexx(n, float2(cos(w.m.theta), -sin(w.m.theta))));

	float v_b =
		(2.0f*(projAbs(sub(v, w.m.v), n_ws) - Jxr*w.m.omega))/
		(1.0f + sq(Jxr)*w.m.m/w.m.I + w.m.m/m);

	v = add(v, scale(n_ws, -w.m.m/m*v_b));
	w.m.v = add(w.m.v, scale(n_ws, v_b));
	w.m.omega += Jxr*w.m.m*v_b/w.m.I;
}

bool collideMap(world & w, float m, float2 & r, float2 & v, float radius){//TODO: add corner checking
	bool collision = false;
	
	float2 r_t = complexx(scale(sub(r, w.m.r), 1.0f/block_scale), float2(cos(w.m.theta), sin(w.m.theta)));//transform the particle from the world space to map space
	int x = (int)round(r_t[0]);
	int y = (int)round(r_t[1]);
	float2 v_t = complexx(sub(v, w.m.v), float2(cos(w.m.theta), sin(w.m.theta)));
	
	//if(x < 0 || y < 0 || x >= sizeof(maps[0])/sizeof(maps[0][0])|| y >= sizeof(maps[0][0])/sizeof(maps[0][0][0])){
	//	return;
	//}
	radius = radius/block_scale;
	float2 r_t_f = r_t;//in the map space
	float2 r_c = r_t;
	if(map_s(w.m.id, x, y) == 1){
		if(map_s(w.m.id, y, x-1) == 1){//abs(r_t[0]-x) < abs(r_t[1]-y)){//TODO: use somthing else(maybe velocity?) to figure out which side to use?
			r_c = float2(r_t[0], y+(r_t[1] > y ? 0.5f : -0.5f));
			r_t_f = float2(r_t[0], y+(r_t[1] > y ? 0.5f+radius : -0.5f-radius));

			if(v_t[1]*normalize(y-r_t[1]) < 0.0){
				collideWall(r_c, float2(0.0f, -block_scale), w, m, r, v);
				collision = true;
			}
		}
		else{
			r_c = float2(x+(r_t[0] > x ? 0.5f : -0.5f), r_t[1]);
			r_t_f = float2(x+(r_t[0] > x ? 0.5f+radius : -0.5f-radius), r_t[1]);
			if(v_t[0]*normalize(x-r_t[0]) < 0.0){
				collideWall(r_c, float2(block_scale, 0.0f), w, m, r, v);
				collision = true;
			}
		}
	}

	float v_b;

	float2 corner_dir = float2(0.5f*normalize(r_t[0]-x), 0.5f*normalize(r_t[1]-y));
	if(abs(sub(r_t, add(float2(x, y), corner_dir))) <= radius && map_s(w.m.id, x+normalize(r_t[0]-x), y+normalize(r_t[1]-y)) == 1){
		r_c = add(float2(x, y), corner_dir);//the position of the corner
		r_t_f = add(r_c, scale(normalize(sub(r_t, r_c)), radius));
		if(projAbs(v_t, sub(r_t, r_c)) < 0.0){
			collideWall(r_c, sub(r_t_f, r_c), w, m, r, v);
			collision = true;
		}
	}

	if(abs(r_t[0]-x)+radius >= 0.5 && map_s(w.m.id, x+normalize(r_t[0]-x), y) == 1){//TODO: put the physics stuff in a seperate function
		r_c[0] = x-normalize(x-r_t[0])*0.5;
		r_t_f[0] = x-normalize(x-r_t[0])*(0.5-radius);
		if(v_t[0]*normalize(x-r_t[0]) < 0.0){
			collideWall(r_c, float2(block_scale, 0.0f), w, m, r, v);
			/*
			
			float Jxr = cross(float2(block_scale, 0.0f), r_c);
			v_b =
				(2.0f*(v_t[0] - Jxr*w.m.omega))/
				(1.0f + sq(Jxr)*w.m.m/w.m.I + w.m.m/m);

			float2 m_dir = float2(cos(w.m.theta), -sin(w.m.theta));

			v = add(v, scale(m_dir,  -w.m.m/m*v_b));
			w.m.v = add(w.m.v, scale(m_dir, v_b));
			w.m.omega += Jxr*w.m.m*v_b/w.m.I;
			
			//v = sub(v, scale(proj(v, float2(cos(w.m.theta), -sin(w.m.theta))), 2.0));
			*/
			collision = true;
		}
	}
	if(abs(r_t[1]-y)+radius >= 0.5 && map_s(w.m.id, x, y+normalize(r_t[1]-y)) == 1){
		r_c[1] = y-normalize(y-r_t[1])*0.5;
		r_t_f[1] = y-normalize(y-r_t[1])*(0.5-radius);
		if(v_t[1]*normalize(y-r_t[1]) < 0.0){
			collideWall(r_c, float2(0.0f, -block_scale), w, m, r, v);
			/**float Jxr = cross(float2(0.0f, block_scale), r_c);
			v_b =
				(2.0f*(v_t[1] - Jxr*w.m.omega))/
				(1.0f + sq(Jxr)*w.m.m/w.m.I + w.m.m/m);

			float2 m_dir = float2(sin(w.m.theta), cos(w.m.theta));

			v = add(v, scale(m_dir,  -w.m.m/m*v_b));
			w.m.v = add(w.m.v, scale(m_dir, v_b));
			w.m.omega += Jxr*w.m.m*v_b/w.m.I;
			
			//v = sub(v, scale(proj(v, float2(sin(w.m.theta), cos(w.m.theta))), 2.0));
			*/
			collision = true;
		}
	}
	r = add(w.m.r, complexx(scale(r_t_f, block_scale), float2(cos(w.m.theta), -sin(w.m.theta))));

	return collision;
}

//TODO: set accelerations first, then update velocities and positions, otherwise some objects are unsyncronized, which causes problems at low framerates or high speeds
void worldLoop(world & w, float dt){//make member function for convinience?
	w.mousePos = float2(1.0f/(1.0f*500.0f)*input::mouse[0], -1.0f/(1.0f*500.0f)*input::mouse[1]);
	//float2 plr_dv = float2(0.0, 0.0);
	
	{
		w.m.theta += dt*w.m.omega;
		w.m.r = add(w.m.r, scale(w.m.v, dt));
		w.m.v = sub(w.m.v, scale(w.m.v, dt*1.0f));
		w.m.omega -= w.m.omega*dt*1.0f;
	}

	{
		float2 inDir = float2(
			-1.0f*float(input::pressed('A')) + 1.0f*float(input::pressed('D')),
			-1.0f*float(input::pressed('S')) + 1.0f*float(input::pressed('W'))
		);

		float speed = 1.5f;
		if(input::pressed(VK_SHIFT)){
			speed = 50.0f;
		}

		inDir = scale(normalize(inDir), speed);
		const float aMag = 50.0f;
		const float drag = 25.0f;
		float2 velDif = sub(inDir, w.plr.v);
		w.plr.dv = add(w.plr.dv, scale(sub(w.plr.v, w.m.v), -drag*dt));
		if(inDir[0] != 0.0f || inDir[1] != 0.0f){
			if(dotMe(sub(velDif, w.plr.dv)) >= pow(aMag*dt, 2)){
				w.plr.dv = add(scale(normalize(velDif), aMag*dt), w.plr.dv);
				//w.plr.v = add(w.plr.v, w.plr.dv);
			}
			else{
				w.plr.dv = velDif;		
				//w.plr.v = add(w.plr.v, plrAccel);
				//w.plr.v = inDir;
			}
		}
		w.m.v = add(w.m.v, scale(w.plr.dv, -w.plr.m/w.m.m));
		w.plr.r = add(scale(w.plr.dv, dt/2), add(w.plr.r, scale(w.plr.v, dt)));
		w.plr.v = add(w.plr.v, w.plr.dv);

		//w.cam.r = add(w.plr.r, scale(w.mousePos, 0.3f));
		w.plr.dir = normalize(w.mousePos);
		w.plr.dv = float2(0.0, 0.0);
		collideMap(w, w.plr.m, w.plr.r, w.plr.v, 0.05f);
	}

	for(int i = 0; false && i < sizeof(w.es)/sizeof(w.es[0]); i++){
		float2 inDir = normalize(sub(w.plr.r, w.es[i].r));

		float speed = 1.5f;

		float2 v_w = scale(inDir, speed);
		const float aMag = 40.0f;
		const float drag = 25.0f;
		float2 velDif = sub(v_w, w.es[i].v);
		w.es[i].dv = add(w.es[i].dv, scale(w.es[i].v, -drag*dt));
		if(dotMe(sub(velDif, w.es[i].dv)) >= pow(aMag*dt, 2)){
			w.es[i].dv = add(scale(normalize(velDif), aMag*dt), w.es[i].dv);
		}
		else{
			w.es[i].dv = velDif;	
		}
		w.es[i].r = add(scale(w.es[i].dv, dt/2), add(w.es[i].r, scale(w.es[i].v, dt)));
		w.es[i].v = add(w.es[i].v, w.es[i].dv);

		//w.cam.r = add(w.plr.r, scale(w.mousePos, 0.3f));
		//w.es[i].dir = inDir;
		w.es[i].dv = float2(0.0, 0.0);
		collideMap(w, 1.0f, w.es[i].r, w.es[i].v, 0.05f);
	}

	for(int i = 0; i < sizeof(w.bs)/sizeof(w.bs[0]); i++){
		auto a = w.bs[i];
		if(w.bs[i].alive > 0){
			w.bs[i].r = add(w.bs[i].r, scale(w.bs[i].v, dt));
			if(dotMe(sub(w.bs[i].r, w.plr.r)) > 10.0){
				w.bs[i].alive = 0;
			}

			if(collideMap(w, 0.5, w.bs[i].r, w.bs[i].v, 0.05f)){
				w.bs[i].alive -= 1;
			}
		}
	}

	if(input::pressed(VK_LBUTTON)){
		for(int i = 0; i < sizeof(w.bs)/sizeof(w.bs[0]) && w.gun.fireTimer <= 0.0; i++){
			if(!w.bs[i].alive){
				w.bs[i].alive = 10;
				w.bs[i].r = add(add(w.plr.r, scale(perp(w.plr.dir), -w.gun.offset[1])), scale(w.plr.dir, w.gun.offset[0]));
				float2 randF2 = normalize(float2(2.0f*(random()-0.5f), 2.0f*(random()-0.5f)));
				float theta = atan2(w.plr.dir[0], w.plr.dir[1]);
				theta += w.gun.spread*2.0f*(random()-0.5f);//TODO: make gaussian distrobution
				float2 relV = scale(float2(sin(theta), cos(theta)), 1.5);
				w.bs[i].v = add(relV, w.plr.v);

				w.plr.v = sub(w.plr.v, scale(w.plr.dir, 0.5f));//sub(w.plr.v, scale(relV, 0.152f));

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