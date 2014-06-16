#include "Input.h"

namespace input{
	bool buts[maxBut] = {};

	long2 mouse = long2(0, 0);

	void dopress(USHORT button) {
		if(button < maxBut){
			buts[button] = true;
		}
	}

	void unpress(USHORT button) {
		if(button < maxBut){
			buts[button] = false;
		}
	}

	bool pressed(USHORT button) {
		return buts[button];
	}

	void moveMouse(long dx, long dy) {
		long2 dMoose = long2(dx, dy);
		mouse = add(dMoose, mouse);
	}

}