#pragma once

#include <GL/glew.h>

struct modelData{
	size_t vertexBytes;
	size_t indexSize;
	GLfloat * vertexData;
	GLushort * indexData;
};

struct vertexObject{
	modelData data;
	GLuint vertexArrayObject;
	GLuint vertexBufferObject;
	GLuint indexBufferObject;
};

typedef enum{
	modId_none,
	modId_floor,
	modId_wall,
	modId_electricfloor,
	modId_crosshair,
	modId_corpse,
	modId_player,
	modId_pirate,
	modId_bullet,
	modId_evil_bullet,
	modId_count,
} modId;

vertexObject bindVO(vertexObject);