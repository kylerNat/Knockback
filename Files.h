#pragma once

#include "RenderingStructs.h"

#include <sys/stat.h>
#include <fstream>

#include <GL/glew.h>

namespace files{
	extern const char * getString(const char * filename);

	extern off_t read(const char * filename, void * out);

	extern modelData getVertexData(const char * filename);
}