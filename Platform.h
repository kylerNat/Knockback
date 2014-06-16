#pragma once

//platform specific code goes here

#include "Files.h"
#include "Input.h"
#include "Math.h"
#include "Game.h"

#include <windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <time.h>

const bool fullscreen = false;
const DWORD resolution[2] = {1000, 1000};

struct shaderInfo{
	GLenum type;
	const char * source;
};

struct vertexObject{
	files::modelData data;
	GLuint vertexArrayObject;
	GLuint vertexBufferObject;
	GLuint indexBufferObject;
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);