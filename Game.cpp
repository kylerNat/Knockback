#include "Game.h"

unsigned getUint(world w, unsigned a, unsigned data[]) {
	if(a >= w.n) {
		return 0;
	}
	unsigned out = *((unsigned*)((unsigned)&w+data[a]));
 	return out;
}

float2 getFloat2(world & w, unsigned a, unsigned data[]) {
	if(a >= w.n) {
		return float2();
	}
	float2 out = *((float2*)((unsigned)&w+data[a]));
 	return out;
}

float2 getPosition(world w, unsigned a) {
	return getFloat2(w, a, w.positions);
}

float2 getDirection(world w, unsigned a) {
	return getFloat2(w, a, w.directions);
}

unsigned getId(world w, unsigned a){
	return getUint(w, a, w.ids);
}

unsigned relP(void * a, void * b){
	return ((unsigned)b-(unsigned)a);
}

world createWorld(){//make constuctor?
	world w = {0};
	
	w.up = float2(0.0, 1.0);

	w.n = 2;
	w.positions = new unsigned[w.n];
	w.directions = new unsigned[w.n];

	w.positions[0] = relP(&w, &(w.mousePos));
	w.directions[0] = relP(&w, &(w.up));

	w.positions[1] = relP(&w, &(w.plr.r));
	w.directions[1] = relP(&w, &(w.plr.dir));

	w.plr.m = 1.0;
	return w;
}

world worldLoop(world w, float dt){//make member function for convinience?
	w.mousePos = float2(1.0f/(2.0f*500.0f)*input::mouse[0], -1.0f/(2.0f*500.0f)*input::mouse[1]);

	{
		player plr = w.plr;
		float2 inDir = float2(
			-1.0f*float(input::pressed('A')) + 1.0f*float(input::pressed('D')),
			-1.0f*float(input::pressed('S')) + 1.0f*float(input::pressed('W'))
		);

		inDir = scale(normalize(inDir), 2.0f);
		const float aMag = 20.0f;
		float2 accel;
		float velDifSq = dotMe(sub(inDir, plr.v));
		if(velDifSq >= pow(aMag*dt, 2)){
			accel = scale(normalize(sub(inDir, plr.v)), aMag*dt);
			plr.v = add(plr.v, accel);
		}
		else{
			accel = scale(normalize(sub(inDir, plr.v)), sqrt(velDifSq)*dt);		
			plr.v = inDir;
		}

		plr.r = add(scale(accel, dt*dt/2), add(plr.r, scale(plr.v, dt)));
		plr.dir = normalize(sub(plr.r, w.mousePos));
		w.plr = plr;
	}
	return w;
}