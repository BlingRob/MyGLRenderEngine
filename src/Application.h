#pragma once
#include "Headers.h"
#include "Time.h"
#include "Scene.h"
//interface libs
#define SDL_MAIN_HANDLED
#include <SDL.h>
//GUI
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM "gl/gl.h"
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
		std::unique_ptr<Scene> scene;
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
		void do_movement();
		bool ProcEvents();
		//GUI 
		void GUIproc();
		ImGui::FileBrowser fileDialog;
		SDL_Cursor  *DefaultCursor,
					*LoadingCursor;
		//check changing of matrixes
		bool ChangedProj = false;
		bool ChangedView = false;
		//Frame Buffer
		std::unique_ptr<FrameBuffer> frame;

		public:
			RenderEngine();
			~RenderEngine()
			{
				frame.release();
				ImGui_ImplOpenGL3_Shutdown();
				ImGui_ImplSDL2_Shutdown();
				ImGui::DestroyContext();

				SDL_FreeCursor(DefaultCursor);
				SDL_FreeCursor(LoadingCursor);
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
