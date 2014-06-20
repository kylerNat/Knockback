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
	modId_crosshair,
	modId_player,
	modId_bullet,
	modId_count,
} modId;

vertexObject bindVO(vertexObject);