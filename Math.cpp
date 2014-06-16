#include "Math.h"

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