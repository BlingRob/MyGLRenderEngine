#pragma once
#include "SDL.h"
#include "Camera.h"
#include "Transformation.h"
//#include "OGLSpec/OGLRenderer.h"
//GUI
#include "imgui_impl_sdl.h"
#include <memory>
#include <thread>
#include <chrono>

class Window
{
public:
	Window(std::shared_ptr<Position_Controller>);
	bool ProcEvents();

	//Size of window
	uint32_t SCR_WIDTH = 800,
		     SCR_HEIGHT = 800;
	std::pair<int32_t, int32_t> MaxSize() const;
	SDL_Window* GetWindow() const;
	void SwapBuffer();
	~Window();
private:
	//windows
	SDL_Window* _pWindow;
	//queue of pressed keys
	bool keys[1024] = { false };

	//mouse controll
	float lastX, lastY;
	bool clicked, firstMouse;
	bool ChangedProj = false;
	bool ChangedView = false;
	//callbacks
	inline void do_movement();
	inline void change_matrixes();
	
	//Data for connect with render engine
	std::shared_ptr<Position_Controller> _mcontr;
};

