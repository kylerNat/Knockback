#pragma once

#include "Math.h"
#include "Input.h"
#include "RenderingStructs.h"

#include <GL/glew.h>
#include <time.h>

#define renderArgs vOs, trans

const float block_scale = 0.2;
const float block_margin = 0.01f;

const int map_size_x = 21;
const int map_size_y = 21;

struct level{
	int walls[map_size_x][map_size_y];
	float2 ers[10];
};

const level levels[1] = {
	{
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,2,3,4,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,
		1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
		1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		//ers
		float2(1.0, -0.25),
		float2(1.1, 0.0),
		float2(1.2, 0.25),
		float2(1.3, 0.0),
		float2(1.4, -0.25),
		float2(1.5, 0.0),
		float2(1.6, 0.25),
		float2(1.7, 0.0),
		float2(1.8, -0.25),
		float2(1.9, 0.0),
	},
};

/*const int maps[1][map_size_x][map_size_y] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,2,3,4,5,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
	1,0,1,1,1,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};*/

struct body{
	float m;
	float2 r;
	float2 v;
	float I;
	float theta;
	float omega;
};

struct player{
	int health;
	float m;
	float2 r;
	float2 v;
	float2 dir;
	float2 dv;
};

struct enemy{
	int health;
	float fireTimer;
	float m;
	float2 r;
	float2 v;
	float2 dir;
	float2 dv;
	enemy():health(1), fireTimer(0.0), m(1.0), r(0.0f, 0.0f), v(0.0f, 0.0f), dir(1.0f, 0.0f), dv(0.0f, 0.0f){}
};

struct weapon{
	float fireWait;
	float fireTimer;
	float spread;
	float2 offset;
};

struct bullet{//TODO: make a table of bullet masses to save memory
	//float m;
	float2 r;
	float2 v;
	float2 dir;
	float alive;
	bool evil;
	bullet():/*m(1.0), */r(0), v(0), alive(0), evil(0){}
};

struct camera{
	float2 r;
	float2 rr;
	float2 v;
};

struct map{
	unsigned id;
	float m;
	float2 r;
	float2 v;
	float2 dv;
	float I;
	float theta;
	float omega;
};

struct world{
	float2 debug_marker;
	float2 debug_ms[500];
	float2 mousePos;
	camera cam;
	map m;
	enemy es[10];
	player plr;
	//bullet ebs[500];
	bullet bs[500];
	weapon gun;
};

void renderObject(float2 pos, float2 dir, modId model, vertexObject * vOs, GLuint trans);

void renderWorld(world &/*temp*/ w, vertexObject * vOs, GLuint trans);

world createWorld();

void worldLoop(world &, float);