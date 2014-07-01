#include "Math.h"

float normalize(float a){
	if(a >= 0.0f){
		return 1.0f;
	}
	return -1.0f;
}

float round(float a){
	float out = floor(a);
	for(unsigned i = 0; i < 1000; i++){
		if(out - a > 0.5f) {
			out = floor(out-1.0f);
		}
		else if(a - out >= 0.5f) {
			out = floor(out+1.0f);
		}
		else {
			break;
		}
	}
	return out;
}


float clamp(float a, float min, float max){
	if(a < min){
		return min;
	}
	if(a > max){
		return max;
	}
	return a;
}

int clamp(int a, int min, int max){
	if(a < min){
		return min;
	}
	if(a >= max){
		return max;
	}
	return a;
}

float bClamp(float a, float min){
	if(a < min){
		return min;
	}
	return a;
}

float random(){
	return ((float)(rand()%(1<<8)))/((float)(1<<8));
}