#pragma once
#include "Headers.h"
#include "Time.h"
#include "Scene.h"
//interface libs
//#define SDL_MAIN_HANDLED
#include <SDL.h>
//GUI
#include "GUI.h"
#define IMGUI_INCLUDE_IMCONFIG_H
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imfilebrowser.h"
//Loader
#include "Loader.h"
//Frame buffer
#include "FrameBuffer.h"

	class RenderEngine
	{
		//windows
		SDL_Window* window;
		SDL_GLContext context;
		//Scene
		//std::unique_ptr<Scene> scene;
		std::shared_ptr<std::unique_ptr<Scene>> scene;
		//time:difference, current, last
		float dt;
		//queue of pressed keys
		bool keys[1024] = {false};

		//mouse controll
		GLfloat lastX, lastY;
		GLboolean clicked, firstMouse;
		//Time
		Chronometr chron;
		//callbacks
		inline void do_movement();
		inline void change_matrixes();
		bool ProcEvents();
		//GUI
		std::unique_ptr<GUI> gui;
		//check changing of matrixes
		bool ChangedProj = false;
		bool ChangedView = false;
		//Frame Buffer
		std::unique_ptr<PostProcessBuffer> frame;
		public:
			RenderEngine();
			~RenderEngine()
			{
				//While all smart pointers won't clean gl objects, mustn't delete gl context
				//Close and destroy the window
				//SDL_DestroyWindow(window);
				// Clean up
				//SDL_Quit();
			}
			bool MainLoop();
			double GetTime();
			void SetScen(std::unique_ptr<Scene>);
			const std::shared_ptr<std::unique_ptr<Scene>> GetScen() const;

		//Size of window
		GLuint SCR_WIDTH = 800,
			SCR_HEIGHT = 800;

	};

	void MessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);
