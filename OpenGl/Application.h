#pragma once
#include "Headers.h"
#include "Time.h"
#include "Scene.h"

	class RenderEngine 
	{
		//windows
		SDL_Window* window;
		SDL_GLContext context;
		//Scene
		std::unique_ptr<Scene> scene;
		//time:difference, current, last
		float dt;
		//queue of pressed keys
		bool keys[1024];

		//mouse controll
		GLfloat lastX, lastY;
		GLboolean clicked, firstMouse;
		//Time
		Chronometr chron;
		//callbacks
		void do_movement();
		bool ProcEvents();
		//
		bool ChangedProj = false;
		bool ChangedView = false;

		public:
			RenderEngine();
			~RenderEngine()
			{
				// Close and destroy the window
				SDL_DestroyWindow(window);

				// Clean up
				SDL_Quit();

			}
			bool MainLoop();
			double GetTime();
			void SetScen(std::unique_ptr<Scene>);
			const Scene* GetScen() const;

		//Size of window
		GLuint SCR_WIDTH = 800,
			SCR_HEIGHT = 800;
			
	};

	void APIENTRY MessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);
