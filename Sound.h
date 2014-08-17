#pragma once

#include <dsound.h>

typedef enum{
	sndId_shot,
	sndId_meat,
	sndId_step,
	sndId_count,
} sndId;

namespace sound{
	extern LPDIRECTSOUND8 ds_face;
	extern LPDIRECTSOUNDBUFFER snds[sndId_count];
}