#pragma once

#include <math.h>

template <class type, unsigned d>//bluuuuuuuuuuuuuuugh
class vec {
public:
	type data[d];

	vec(){}
	vec(int a){
		type data = {0};
	}
	vec(type x, type y){
		data[0] = x;
		data[1] = y;
	}

	type& operator[](int a){
		return data[a];
	}
};

template class vec<long, 2>;
typedef vec<long, 2> long2;

template class vec<float, 2>;
typedef vec<float, 2> float2;

template <class type, unsigned c, unsigned r>
struct mat {
	type data[c][r];
	
	mat operator=(type dat[c][r]){
		data = dat;
	}

	type * operator[](int a){
		if(a >= c){
			return data[c];
		}
		return data[a];
	}
};

typedef mat<float, 4, 4> float4x4;

template<class type>
vec<type, 2> normalize(vec<type, 2> a){
	if(a[0] == 0.0 && a[1] == 0.0){
		return a;
	}
	return scale(a, 1.0/sqrt(dot(a, a)));
}

template<class type>
vec<type, 2> add(vec<type, 2> a, vec<type, 2> b){
	vec<type, 2> out = vec<type, 2>(a[0]+b[0], a[1]+b[1]);
	return out;
}

template<class type>
vec<type, 2> sub(vec<type, 2> a, vec<type, 2> b){
	vec<type, 2> out = vec<type, 2>(a[0]-b[0], a[1]-b[1]);
	return out;
}

template<class type>
type dot(vec<type, 2> a, vec<type, 2> b){
	return a[0]*b[0] + a[1]*b[1];
}

template<class type, unsigned d>
type dotMe(vec<type, d> a){
	return dot(a, a);
}

template<class type>
type dot(vec<type, 4> a, vec<type, 4> b){
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
}


template<class type>
vec<type, 2> scale(vec<type, 2> v, float s){
	vec<type, 2> out = vec<type, 2>(v[0]*s, v[1]*s);
	return out;
}

template<class type, unsigned c, unsigned r>
mat<type, r, r> multiply(mat<type, c, r> a, mat<type, r, c> b){
	mat<type, r, r> out = {0};
	for(unsigned i = 0; i < r; i++){
		for(unsigned j = 0; j < r; j++){
			for(unsigned k = 0; k < c; k++){
				out[i][j] += a[k][j]*b[i][k];
			}
		}
	}
	return out;
}

float round(float a);