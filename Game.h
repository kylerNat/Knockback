#pragma once

#include "Math.h"
#include "Input.h"
#include "RenderingStructs.h"

#include <GL/glew.h>
#include <time.h>

#define particle \
	float m;\
	float2 r;\
	float2 v;\
	float2 dir;

struct body{
	float m;
	float2 r;
	float2 v;
	float I;
	float theta;
	float omega;
};

struct player{
	particle
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
	bool alive;
	bullet():/*m(1.0), */r(0), v(0), alive(0){}
};

struct camera{
	float2 r;
	float2 rr;
	float2 v;
};

struct world{
	float2 mousePos;
	camera cam;
	player plr;
	bullet bs[1000];
	weapon gun;
};

void renderObject(float2 pos, float2 dir, modId model, vertexObject * vOs, GLuint trans);

void renderWorld(world w, vertexObject * vOs, GLuint trans);

world createWorld();

void worldLoop(world &, float);