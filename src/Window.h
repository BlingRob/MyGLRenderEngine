#pragma once

#include "Headers.h"
#include <SDL.h>
#include "Camera.h"
#include "Transformation.h"
//GUI
#include "GUI.h"

class Window
{
	//windows
	std::shared_ptr<SDL_Window> window;
	std::shared_ptr <void> context;
	//queue of pressed keys
	bool keys[1024] = { false };

	//mouse controll
	GLfloat lastX, lastY;
	GLboolean clicked, firstMouse;
	bool ChangedProj = false;
	bool ChangedView = false;
	//callbacks
	inline void do_movement();
	inline void change_matrixes();
	
	//Data for connect with render engine
	std::shared_ptr<Position_Controller> _mcontr;
public:
	Window(std::shared_ptr<Position_Controller>);
	bool ProcEvents();
	void SwapBuffer();


	//Size of window
	GLuint SCR_WIDTH = 800,
		SCR_HEIGHT = 800;
	std::pair<int32_t, int32_t> MaxSize();
	std::shared_ptr < void> GetContext();
	std::shared_ptr<SDL_Window> GetWindow();
};

