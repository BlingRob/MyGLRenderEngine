#pragma once
#include "Headers.h"
#include "Time.h"
#include "Scene.h"
//interface libs
//GUI
#include "GUI.h"
//Loader
#include "Loader.h"
//Frame buffer
#include "FrameBuffer.h"

	class RenderEngine
	{
		std::shared_ptr<Position_Controller> _mcontr;
		std::unique_ptr<Window> _mWindow;
		//Scene
		//std::unique_ptr<Scene> scene;
		std::shared_ptr<std::unique_ptr<Scene>> scene;
		//time:difference, current, last
		float dt;

		//Time
		Chronometr chron;
		//GUI
		std::unique_ptr<GUI> gui;
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

	};

	void MessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);
