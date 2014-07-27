#include "Game.h"
/*TODO:
	levels
	water physics(in progress)
	make bullet mechanics more consistant (the direction the level moves in vs. the direction you shoot)
	multiple weapons
	enemy ai
	corpses
	bullet gaps in enemy bullets?
	vsync related lag issue?
*/

int map_raw_s(int id, int x, int y){
	if(x < 0 || y < 0 || x >= map_size_x || y >= map_size_y){
		return 0;
	}
	return levels[id].walls[y][x];
}

int map_s(int id, int x, int y){
	x += (map_size_x-1)/2;
	y = -y;
	y += (map_size_y-1)/2;
	return map_raw_s(id, x, y);
}

void loadLevel(world & w, int id){
	for(int i = 0; i < sizeof(levels[id].ers)/sizeof(levels[id].ers[0]); i++){
		w.es[i].r = levels[id].ers[i];
	}
}

void renderObject(float2 pos, float2 dir, modId model, vertexObject * vOs, GLuint trans){
	float4x4 transform = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};

	transform[0][3] = pos[0];//round(pos[0]*500.0f)/500.0f;
	transform[1][3] = pos[1];//round(pos[1]*500.0f)/500.0f;
	
	transform[0][0] = dir[1];
	transform[0][1] = dir[0];
	transform[1][0] = -dir[0];
	transform[1][1] = dir[1];

	bindVO(vOs[model]);
	glUniformMatrix4fv(trans, 1, false, (float*)&transform[0][0]);
	glDrawElements(GL_TRIANGLES, vOs[model].data.indexSize, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void renderWorld(world * w, vertexObject * vOs, GLuint trans){
	renderObject(sub(w->plr.r, w->cam.r), w->plr.dir, (w->plr.health > 0.0 ? modId_player : modId_corpse), renderArgs);
	
	for(int i = 0; i < sizeof(w->bs)/sizeof(w->bs[0]); i++){
		if(w->bs[i].alive > 0.0f){
			renderObject(sub(w->bs[i].r, w->cam.r), scale(w->bs[i].dir, 0.75), modId(modId_bullet+w->bs[i].evil), renderArgs);
		}
	}

	for(int i = 0; i < sizeof(w->es)/sizeof(w->es[0]); i++){
		renderObject(sub(w->es[i].r, w->cam.r), w->es[i].dir, (w->es[i].health > 0.0 ? modId_pirate : modId_corpse), renderArgs);
	}

	//TODO: temp
	/*for(int x = -(map_size_x-1)/2; x < map_size_x-(map_size_x-1)/2; x++){
		for(int y = -(map_size_y-1)/2; y < map_size_y-(map_size_y-1)/2; y++){
			renderObject(sub(add(w->m.r, complexx(float2(x*block_scale+block_margin, y*block_scale), float2(cos(w->m.theta), -sin(w->m.theta)))), w->cam.r), float2(sin(w->m.theta)*block_scale, cos(w->m.theta)*block_scale), (modId) (map_s(w->m.id, x, y)+modId_floor), renderArgs);		
			renderObject(sub(add(w->m.r, complexx(float2(x*block_scale, y*block_scale+block_margin), float2(cos(w->m.theta), -sin(w->m.theta)))), w->cam.r), float2(sin(w->m.theta)*block_scale, cos(w->m.theta)*block_scale), (modId) (map_s(w->m.id, x, y)+modId_floor), renderArgs);
			renderObject(sub(add(w->m.r, complexx(float2(x*block_scale+block_margin, y*block_scale+block_margin), float2(cos(w->m.theta), -sin(w->m.theta)))), w->cam.r), float2(sin(w->m.theta)*block_scale, cos(w->m.theta)*block_scale), (modId) (map_s(w->m.id, x, y)+modId_floor), renderArgs);		
		}
	}*/

	for(int x = -(map_size_x-1)/2; x < map_size_x-(map_size_x-1)/2; x++){
		for(int y = -(map_size_y-1)/2; y < map_size_y-(map_size_y-1)/2; y++){
			renderObject(sub(add(w->m.r, complexx(float2(x*block_scale, y*block_scale), float2(cos(w->m.theta), -sin(w->m.theta)))), w->cam.r), float2(sin(w->m.theta)*block_scale*1.1, cos(w->m.theta)*block_scale*1.1), (modId) (map_s(w->m.id, x, y)+modId_floor), renderArgs);
		}
	}

	for(int i = 0; i < sizeof(w->debug_ms)/sizeof(w->debug_ms[0]); i++){
		if(w->debug_ms[i][1] > 1000000.0f){
			continue;
		}
		renderObject(sub(w->debug_ms[i], w->cam.r), float2(0.0f, 1.0f), modId_crosshair, renderArgs);
	}
	renderObject(sub(w->debug_marker, w->cam.r), float2(0.0f, 1.0f), modId_bullet, renderArgs);
	
	renderObject(w->mousePos, float2(0.0f, 1.0f), modId_crosshair, renderArgs);
}

world createWorld(){//make constuctor?
	srand((unsigned) time(NULL));
	
	world w = {0};

	w.wh[0][10][10] = 100.0f;

	w.wh[0][10][11] = 100.0f;
	w.wh[0][11][10] = 100.0f;
	w.wh[0][9][10] = 100.0f;
	w.wh[0][10][9] = 100.0f;

	w.wh[0][11][11] = 100.0f;
	w.wh[0][11][9] = 100.0f;
	w.wh[0][9][11] = 100.0f;
	w.wh[0][9][9] = 100.0f;

	w.wh[0][10][12] = 100.0f;
	w.wh[0][12][10] = 100.0f;
	w.wh[0][8][10] = 100.0f;
	w.wh[0][10][8] = 100.0f;
	


	w.wh[0][90][10] = 10.0f;

	w.wh[0][90][11] = 1.0f;
	w.wh[0][91][10] = 1.0f;
	w.wh[0][89][10] = 1.0f;
	w.wh[0][90][9] = 1.0f;

	w.wh[0][91][11] = 1.0f;
	w.wh[0][91][9] = 1.0f;
	w.wh[0][89][11] = 1.0f;
	w.wh[0][89][9] = 1.0f;

	w.wh[0][90][12] = 1.0f;
	w.wh[0][92][10] = 1.0f;
	w.wh[0][88][10] = 1.0f;
	w.wh[0][90][8] = 1.0f;

	
	//w.wh[0][45][90] = 10000.0f;
	
	//w.wh[0][55][90] = 10000.0f;

	//w.wh[0][20][20] = 10.0f;

	loadLevel(w, 0);

	w.m.r = float2(-0.0, -0.0);
	w.m.theta = 0.0;
	w.m.omega = 0.0;
	w.m.m = 10.0;
	w.m.I = 25.0;

	w.plr.health = 1;

	/*
	w.gun.fireWait = 0.0015f;
	w.gun.spread = 0.5f;
	w.gun.offset = float2(0.1f, 0.0f);
	*/

	w.gun.fireWait = 0.05f;
	w.gun.spread = 0.105f;
	w.gun.offset = float2(0.05f, 0.0f);

	w.plr.m = 1.0;
	return w;
}

void collideWall(float2 r_c, float2 n, world * w, float m, float2 & r, float2 & v){
	float Jxr = cross(n, r_c);

	float2 n_ws = normalize(complexx(n, float2(cos(w->m.theta), -sin(w->m.theta))));

	float v_b =
		(2.0f*(projAbs(sub(v, w->m.v), n_ws) - Jxr*w->m.omega))/
		(1.0f + sq(Jxr)*w->m.m/w->m.I + w->m.m/m);

	v = add(v, scale(n_ws, -w->m.m/m*v_b));
	w->m.v = add(w->m.v, scale(n_ws, v_b));
	w->m.omega += Jxr*w->m.m*v_b/w->m.I;
}

bool touchingBlock(world * w, float2 r, float radius, int id){
	float2 r_t = complexx(scale(sub(r, w->m.r), 1.0f/block_scale), float2(cos(w->m.theta), sin(w->m.theta)));//transform the particle from the world space to map space
	int x = (int)round(r_t[0]);
	int y = (int)round(r_t[1]);

	radius = radius/block_scale;
	float2 r_t_f = r_t;//in the map space
	float2 r_c = r_t;
	if(map_s(w->m.id, x, y) == id){
		return true;
	}

	float2 corner_dir = float2(0.5f*normalize(r_t[0]-x), 0.5f*normalize(r_t[1]-y));
	if(abs(sub(r_t, add(float2(x, y), corner_dir))) <= radius && map_s(w->m.id, x+normalize(r_t[0]-x), y+normalize(r_t[1]-y)) == id){
		return true;
	}

	if(abs(r_t[0]-x)+radius >= 0.5 && map_s(w->m.id, x+normalize(r_t[0]-x), y) == id){
		return true;
	}
	if(abs(r_t[1]-y)+radius >= 0.5 && map_s(w->m.id, x, y+normalize(r_t[1]-y)) == id){
		return true;
	}
	return false;
}

bool collideMap(world * w, float m, float2 & r, float2 & v, float radius){
	bool collision = false;
	
	float2 r_t = complexx(scale(sub(r, w->m.r), 1.0f/block_scale), float2(cos(w->m.theta), sin(w->m.theta)));//transform the particle from the world space to map space
	int x = (int)round(r_t[0]);
	int y = (int)round(r_t[1]);
	float2 v_t = complexx(sub(v, w->m.v), float2(cos(w->m.theta), sin(w->m.theta)));
	
	//if(x < 0 || y < 0 || x >= sizeof(maps[0])/sizeof(maps[0][0])|| y >= sizeof(maps[0][0])/sizeof(maps[0][0][0])){
	//	return;
	//}
	radius = radius/block_scale;
	/*if(map_s(w->m.id, x, y) == 1){
		//push to the nearest empty neighbor
		float d;
		float d_nearest = abs(0.5-radius+(r_t[0] - x));
		int nearest = 0;
		d = abs(0.5-radius-(r_t[0] - x));
		if(map_s(w->m.id, x+1, y) == 0 && d < d_nearest){
			d_nearest = d;
			nearest = 2;
		}
		d = abs(0.5-radius+(r_t[1] - y));
		if(map_s(w->m.id, x, y-1) == 0 && d < d_nearest){
			d_nearest = d;
			nearest = 1;
		}
		d = abs(0.5-radius-(r_t[1] - y));
		if(map_s(w->m.id, x, y+1) == 0 && d < d_nearest){
			d_nearest = d;
			nearest = 3;
		}
		switch(nearest){
			case 0:
				r_t[0] -= 0.5;
			break;
			case 2:
				r_t[0] += 0.5;
			case 1:
				r_t[1] -= 0.5;
				r_t[1] = y-1;
			break;
			case 3:
				r_t[1] += 0.5;
				r_t[1] = y+1;
			break;
		}
		/*
		if(map_s(w->m.id, y, x-1) == 1){//abs(r_t[0]-x) < abs(r_t[1]-y)){//TODO: use somthing else(maybe velocity?) to figure out which side to use?
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
		*/
	//}
	float2 r_t_f = r_t;//in the map space
	float2 r_c = r_t;
	float v_b;

	float2 corner_dir = float2(0.5f*normalize(r_t[0]-x), 0.5f*normalize(r_t[1]-y));
	if(abs(sub(r_t, add(float2(x, y), corner_dir))) <= radius && map_s(w->m.id, x+normalize(r_t[0]-x), y+normalize(r_t[1]-y)) == 1){
		r_c = add(float2(x, y), corner_dir);//the position of the corner
		r_t_f = add(r_c, scale(normalize(sub(r_t, r_c)), radius));
		if(projAbs(v_t, sub(r_t, r_c)) < 0.0){
			collideWall(r_c, sub(r_t_f, r_c), w, m, r, v);
			collision = true;
		}
	}

	if(abs(r_t[0]-x)+radius >= 0.5 && map_s(w->m.id, x+normalize(r_t[0]-x), y) == 1){
		r_c[0] = x-normalize(x-r_t[0])*0.5;
		r_t_f[0] = x-normalize(x-r_t[0])*(0.5-radius);
		if(v_t[0]*normalize(x-r_t[0]) < 0.0){
			collideWall(r_c, float2(block_scale, 0.0f), w, m, r, v);
			collision = true;
		}
	}
	if(abs(r_t[1]-y)+radius >= 0.5 && map_s(w->m.id, x, y+normalize(r_t[1]-y)) == 1){
		r_c[1] = y-normalize(y-r_t[1])*0.5;
		r_t_f[1] = y-normalize(y-r_t[1])*(0.5-radius);
		if(v_t[1]*normalize(y-r_t[1]) < 0.0){
			collideWall(r_c, float2(0.0f, -block_scale), w, m, r, v);
			collision = true;
		}
	}
	r = add(w->m.r, complexx(scale(r_t_f, block_scale), float2(cos(w->m.theta), -sin(w->m.theta))));

	return collision;
}

bool intersectsLine(world * w, float2 r0, float2 r1) {
	float2 rt0 = complexx(scale(sub(r0, w->m.r), 1.0f/block_scale), float2(cos(w->m.theta), sin(w->m.theta)));//transform the particle from the world space to map space
	
	float2 rt1 = complexx(scale(sub(r1, w->m.r), 1.0f/block_scale), float2(cos(w->m.theta), sin(w->m.theta)));//transform the particle from the world space to map space

	float2 r01 = sub(rt1, rt0);
	/*
		x to y formula
		y(x) = rt0[1]+r01[1]*sub(rt0[0], x)/r01[0];
	*/
	for(int x = round(rt0[0])+normalize(r01[0]); x*normalize(r01[0]) <= round(rt1[0])*normalize(r01[0]); x += normalize(r01[0])){//TODO: check rounding
		float y = rt0[1]+r01[1]*(x-0.5f*normalize(r01[0])-rt0[0])/r01[0];
		if(map_s(w->m.id, x, round(y)) == 1){
			return true;
		}
	}

	for(int y = round(rt0[1])+normalize(r01[1]); y*normalize(r01[1]) <= round(rt1[1])*normalize(r01[1]); y += normalize(r01[1])){//TODO: check rounding
		float x = rt0[0]+r01[0]*(y-0.5f*normalize(r01[1])-rt0[1])/r01[1];
		if(map_s(w->m.id, round(x), y) == 1){
			return true;
		}
	}

	return false;
}

float wh(world * w, bool current, int x, int y){
	if(x < 0 || y < 0 || x >= 100 || y >= 100){
		return 0.0;
	}
	return w->wh[w->awh==current][x][y];
}

void ripple(world * w, float2 r){
	float2 r_t = complexx(scale(sub(r, w->m.r), 4.0/block_scale), float2(cos(w->m.theta), sin(w->m.theta)));//transform the particle from the world space to map space
	int x = (int)round(r_t[0])+50;
	int y = (int)round(r_t[1])+50;
	if(!(x < 0 || y < 0 || x >= 100 || y >= 100)){
		w->wh[w->awh][y][x] = -5.0;
	}
}

//TODO: set accelerations first, then update velocities and positions, otherwise some objects are unsyncronized, which causes problems at low framerates or high speeds
void worldLoop(world * w, float dt, float time){//make member function for convinience?
	{//if(input::pressed('G')){//water physics
		w->awh = !w->awh;
		for(int x = 0; x < sizeof(w->wh[0][0])/sizeof(w->wh[0][0][0]); x++){
			for(int y = 0; y < sizeof(w->wh[0])/sizeof(w->wh[0][0]); y++){
				float dh = dt*w->wv[!w->awh][x][y];
				//w->wh[w->awh][x+1][y] -= 0.25*dh;
				//w->wh[w->awh][x-1][y] -= 0.25*dh;
				//w->wh[w->awh][x][y+1] -= 0.25*dh;
				//w->wh[w->awh][x][y-1] -= 0.25*dh;
				
				if(map_raw_s(w->m.id, (int)floor(y*(1.0*map_size_x/100.0)), floor((99-x)*(1.0*map_size_y/100.0))) == 1){
					w->wh[w->awh][x][y] = 0.0;//10.0*abs(w->m.v);
					continue;
				}
				/*
				if(map_raw_s(w->m.id, (int)floor(y*(1.0*map_size_x/100.0)), floor((99-x-normalize(w->m.v[1]))*(1.0*map_size_y/100.0))) == 1){
					w->wh[!w->awh][x][y] = 5.0*w->m.v[1];
				}

				if(map_raw_s(w->m.id, (int)floor((y-normalize(w->m.v[0]))*(1.0*map_size_x/100.0)), floor((99-x)*(1.0*map_size_y/100.0))) == 1){
					w->wh[!w->awh][x][y] = 5.0*w->m.v[0];
				}
				*/
				float dv = (random()-0.5)*50.0*dt - 1.0*dt*w->wh[!w->awh][x][y] - 2.5*dt*w->wv[!w->awh][x][y]
					-50.0*dt*(
						+(w->wh[!w->awh][x][y] - wh(w, 0, x+1, y))
						+(w->wh[!w->awh][x][y] - wh(w, 0, x-1, y))
						+(w->wh[!w->awh][x][y] - wh(w, 0, x, y+1))
						+(w->wh[!w->awh][x][y] - wh(w, 0, x, y-1))
					);

				w->wv[w->awh][x][y] = 
					 w->wv[!w->awh][x][y] + dv;
				
				w->wh[w->awh][x][y] = 0.5f*dv*dt + w->wh[!w->awh][x][y]+dt*w->wv[!w->awh][x][y];
			}
		}
		/*
		for(int x = 0; x < sizeof(w->wh[0][0])/sizeof(w->wh[0][0][0]); x++){
			for(int y = 0; y < sizeof(w->wh[0])/sizeof(w->wh[0][0]); y++){
				w->wh[w->awh][x][y] = w->wh[!w->awh][x][y]+dt*w->wv[!w->awh][x][y];
			}
		}*/
	}
	
	/*
	input::mouse[0] = clamp(input::mouse[0], -500.0, 500.0);
	input::mouse[1] = clamp(input::mouse[1], -500.0, 500.0);
	*/
	w->mousePos = float2(1.0f/(1.0f*500.0f)*input::mouse[0], -1.0f/(1.0f*500.0f)*input::mouse[1]);

	//float2 plr_dv = float2(0.0, 0.0);

	if(w->plr.health > 0){
		float2 inDir = float2(
			-1.0f*float(input::pressed('A')) + 1.0f*float(input::pressed('D')),
			-1.0f*float(input::pressed('S')) + 1.0f*float(input::pressed('W'))
		);

		inDir = normalize(inDir);

		float foot_speed = 2.0f;
		float drag_const = 5.0f;

		float2 v_feet = add(scale(inDir, -foot_speed), w->plr.v);
		float2 J_drag = scale(sub(v_feet, add(w->m.v, scale(perp(sub(w->m.r, w->plr.r)), w->m.omega))), -drag_const*dt);
		w->plr.dv = scale(J_drag, 1.0f/w->plr.m);

		w->m.dv = add(w->m.dv, scale(J_drag, -1.0f/w->m.m));
		w->m.omega += cross(J_drag, sub(w->m.r, w->plr.r))/w->m.I;
		w->plr.r = add(scale(w->plr.dv, dt/2), add(w->plr.r, scale(w->plr.v, dt)));
		w->plr.v = add(w->plr.v, w->plr.dv);

		w->plr.dir = normalize(w->mousePos);
		w->plr.dv = float2(0.0, 0.0);
		collideMap(w, w->plr.m, w->plr.r, w->plr.v, 0.05f);
		if(touchingBlock(w, w->plr.r, 0.05f, 2)){
			w->plr.health = 0;
		}
		
		w->plr.foot_timer += 10.0*dt*abs(w->plr.v);

		for(; w->plr.foot_timer > 0.0; w->plr.foot_timer -= 1.0){
			ripple(w, add(w->plr.r, scale(w->plr.v, 0.025)));
		}
	}

	for(int i = 0; i < sizeof(w->debug_ms)/sizeof(w->debug_ms[0]); i++){
		if(w->debug_ms[i][1] > 1000000.0f){
			continue;
		}
		w->debug_ms[i] = float2(0.0, 100000000.0f);
	}

	for(int i = 0; i < sizeof(w->es)/sizeof(w->es[0]); i++){
		if(w->es[i].health > 0){
			float2 inDir = scale(normalize(w->es[i].v), -1.0);//float2(random()-0.5f, random()-0.5f));// = normalize(sub(w->plr.r, w->es[i].r));

			if(intersectsLine(w, w->es[i].r, w->plr.r) || dotMe(sub(w->es[i].r, w->plr.r)) > sq(2.0f)){
				//inDir = normalize(add(sub(w->plr.r, w->es[i].r), float2(10.0*(random()-0.5), 10.0*(random()-0.5))));//normalize(w->es[i].v);
				//inDir = normalize(float2(random()-0.5f, random()-0.5f));//scale(inDir, -1.0f);
			}else{//TODO stop bullets from spawning in walls
				inDir = normalize(w->es[i].v);
				//w->plr.v = sub(w->plr.v, scale(w->plr.dir, 2.0f*dt));
				if(int(time) % 3 < 2){
					for(int j = 0; j < sizeof(w->bs)/sizeof(w->bs[0]) && w->es[i].fireTimer <= 0.0; j++){
						if(w->bs[j].alive <= 0.0f){
							w->bs[j].alive = 3.0f;
							w->bs[j].r = add(add(w->es[i].r, scale(perp(w->plr.dir), -w->gun.offset[1])), scale(w->es[i].dir, w->gun.offset[0]));
							float2 randF2 = normalize(float2(2.0f*(random()-0.5f), 2.0f*(random()-0.5f)));
							float theta = atan2(w->es[i].dir[0], w->es[i].dir[1]);
							theta += w->gun.spread*2.0f*(random()-0.5f);//TODO: make gaussian distrobution
							float2 relV = scale(float2(sin(theta), cos(theta)), 1.5);
							w->bs[j].v = add(relV, w->es[i].v);
							w->bs[j].dir = w->es[i].dir;

							w->es[i].v = sub(w->es[i].v, scale(w->es[i].dir, 0.2f));//sub(w->plr.v, scale(relV, 0.152f));

							w->cam.v = add(w->cam.v, add(scale(randF2, 1.0f), scale(relV, -0.2f)));
		
							w->es[i].fireTimer += w->gun.fireWait;
							w->bs[j].evil = true;
						}
					}
				}
				if(w->es[i].fireTimer <= 0.0){//the bullet limit has been reached
					w->es[i].fireTimer = w->gun.fireWait;
				}
			}

			if(w->es[i].fireTimer >= 0.0){
				w->es[i].fireTimer -= dt;
			}
			else {
				w->es[i].fireTimer = 0.0f;
			}

			float foot_speed = 0.5f;
			float drag_const = 1.0f;

			float2 v_feet = add(scale(inDir, -foot_speed), w->es[i].v);
			float2 J_drag = scale(sub(v_feet, add(w->m.v, scale(perp(sub(w->m.r, w->es[i].r)), w->m.omega))), -drag_const*dt);
			w->es[i].dv = scale(J_drag, 1.0f/w->es[i].m);

			w->m.dv = add(w->m.dv, scale(J_drag, -1.0f/w->m.m));
			w->m.omega += cross(J_drag, sub(w->m.r, w->es[i].r))/w->m.I;
			w->es[i].r = add(scale(w->es[i].dv, dt/2), add(w->es[i].r, scale(w->es[i].v, dt)));
			w->es[i].v = add(w->es[i].v, w->es[i].dv);

			w->es[i].dir = normalize(sub(w->plr.r, w->es[i].r));//inDir;
			w->es[i].dv = float2(0.0, 0.0);
			collideMap(w, w->es[i].m, w->es[i].r, w->es[i].v, 0.05f);
			
			ripple(w, w->es[i].r);

			if(dotMe(sub(w->plr.r, w->es[i].r)) < sq(0.1)){
				w->plr.health -= 1;
			}

			for(int b = 0; b < sizeof(w->bs)/sizeof(w->bs[0]); b++){
				if(w->bs[b].alive > 0.0f && !w->bs[b].evil){
					if(dotMe(sub(w->bs[b].r, w->es[i].r)) < sq(0.1)){
						w->es[i].health -= 1;
						w->bs[b].alive = 0.0;
					}
				}
			}
		}
	}
	
	{
		w->m.theta += dt*w->m.omega;
		w->m.dv = sub(w->m.dv, scale(w->m.v, dt*1.0f));
		w->m.omega -= w->m.omega*dt*1.0f;
		w->m.r = add(scale(w->m.dv, dt/2), add(w->m.r, scale(w->m.v, dt)));
		w->m.v = add(w->m.v, w->m.dv);
		w->m.dv = float2(0.0f, 0.0f);
	}

	for(int i = 0; i < sizeof(w->bs)/sizeof(w->bs[0]); i++){
		if(w->bs[i].alive > 0.0f){
			w->bs[i].r = add(w->bs[i].r, scale(w->bs[i].v, dt));
			if(dotMe(sub(w->bs[i].r, w->plr.r)) > 10.0){
				w->bs[i].alive = 0.0f;
			}

			if(touchingBlock(w, w->bs[i].r, 0.05f, 1)){
				w->bs[i].alive = 0.0f;
			}

			if(collideMap(w, 0.2, w->bs[i].r, w->bs[i].v, 0.05f)){
				w->bs[i].dir = normalize(w->bs[i].v);
				w->bs[i].alive = 0.025f;
			}
			w->bs[i].alive -= dt;

			if(w->bs[i].evil && dotMe(sub(w->bs[i].r, w->plr.r)) < sq(0.1f)){
				w->plr.health -= 1;
			}
		}
	}

	if(input::pressed(VK_LBUTTON) && w->plr.health > 0.0){//TODO stop bullets from spawning in walls
		//w->plr.v = sub(w->plr.v, scale(w->plr.dir, 2.0f*dt));
		for(int i = 0; i < sizeof(w->bs)/sizeof(w->bs[0]) && w->gun.fireTimer <= 0.0; i++){
			if(w->bs[i].alive <= 0.0f){
				w->bs[i].alive = 3.0f;
				w->bs[i].r = add(add(w->plr.r, scale(perp(w->plr.dir), -w->gun.offset[1])), scale(w->plr.dir, w->gun.offset[0]));
				float2 randF2 = normalize(float2(2.0f*(random()-0.5f), 2.0f*(random()-0.5f)));
				float theta = atan2(w->plr.dir[0], w->plr.dir[1]);
				theta += w->gun.spread*2.0f*(random()-0.5f);//TODO: make gaussian distrobution
				float2 relV = scale(float2(sin(theta), cos(theta)), 1.5);
				w->bs[i].v = add(relV, w->plr.v);
				w->bs[i].dir = w->plr.dir;

				w->plr.v = sub(w->plr.v, scale(w->plr.dir, 0.2f));//sub(w->plr.v, scale(relV, 0.152f));

				w->m.v = add(w->m.v, scale(w->plr.dir, w->plr.m*0.2f/w->m.m));//TODO: apply a torque aswell

				w->cam.v = add(w->cam.v, add(scale(randF2, 1.0f), scale(relV, -0.2f)));
		
				w->gun.fireTimer += w->gun.fireWait;
				w->bs[i].evil = false;
			}
		}
		if(w->gun.fireTimer <= 0.0){//the bullet limit has been reached
			w->gun.fireTimer = w->gun.fireWait;
		}
	}

	if(w->gun.fireTimer >= 0.0){
		w->gun.fireTimer -= dt;
	}
	else {
		w->gun.fireTimer = 0.0f;
	}
	
	{//x = e^(-gamma*t)*A*cos(omega*t + theta) //damped shm //consistaint under different frame rates
		const float omega = 100.0f;
		const float gamma = 20.0f;
		for(int i = 0; i < 2; i++){//having each axis "springed" separatly feels better than a single spring attached to the center //jk it should be the same for a linear spring
			float x = w->cam.rr[i];
			float v = w->cam.v[i];
			float theta = atan2(-v-x*gamma, omega*x);
			float A = sqrt(pow(x, 2) + pow((v+x*gamma)/omega, 2));//by math, not by cons. of energy; A != the undampened amplitude
			x = exp(-gamma*dt)*A*cos(omega*dt+theta);
			v = -x*gamma -exp(-gamma*dt)*omega*A*sin(omega*dt+theta);
			w->cam.rr[i] = x;
			w->cam.v[i] = v;
		}
		
		w->cam.r = add(add(w->plr.r, scale(w->mousePos, 0.3f)), w->cam.rr);
	}
}