#pragma once
#include "SDL.h"
#include "gl/gl.h"
#include "../Window.h"
#include <string>
#include <iostream>

#ifdef NDEBUG
const bool Debugging = false;
#else
const bool Debugging = true;
#endif

void MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

class OGLRenderer 
{
public:
	OGLRenderer(const Window* win)
	{
		_pWin = win;
		ContextSetup();
		FirstBufferInit();
		CreateContext();
		SetOptions();
		if(Debugging)
			setupDebugMessenger();
	}
	~OGLRenderer();

	void SetViewPort(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y2);

private:
	void* context;
	const Window* _pWin;

	void ContextSetup();
	void FirstBufferInit();
	void CreateContext();
	void SetOptions();
	void setupDebugMessenger();
};