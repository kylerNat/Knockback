#pragma once

#include "Math.h"
#include "Input.h"

struct body{
	float m;
	float2 r;
	float2 v;
	float I;
	float theta;
	float omega;
};

struct player{
	unsigned model;
	float m;
	float2 r;
	float2 v;
	float2 dir;
};

struct world{
	unsigned mouseModel;
	float2 up;
	float2 mousePos;
	float2 camUp;
	float2 camPos;
	player plr;
	unsigned n;
	unsigned * positions;
	unsigned * directions;
	unsigned * ids;
};

float2 getPosition(world & w, unsigned a);

float2 getDirection(world & w, unsigned a);

world createWorld();

world worldLoop(world wrld, float dt);