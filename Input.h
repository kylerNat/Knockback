#pragma once

#include "Math.h"

#include <windows.h>

namespace input{
	const size_t maxBut = 256;// the greatest virtual key code+1 //actually 255

	extern bool buts[maxBut];

	extern long2 mouse;

	extern void dopress(USHORT button);
	extern void unpress(USHORT button);

	extern bool pressed(USHORT button);

	extern void moveMouse(long dx, long dy);
}