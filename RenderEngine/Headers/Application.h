#pragma once
#include "gl/gl.h"
#include "Time.h"
#include "Scene.h"
//interface libs
//GUI
#include "GUI.h"
//Loader
#include "Loaders/SceneLoader.h"
//Frame buffer
#include "FrameBuffer.h"
#include <iostream>

	class RenderEngine
	{
		std::shared_ptr<Position_Controller> _mcontr;
		//Pointer of window class
		std::unique_ptr<Window> _mWindow;
		//Scene
		std::shared_ptr<std::unique_ptr<Scene>> scene;
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
			}
			bool MainLoop();
			double GetTime();
			void SetScen(std::unique_ptr<Scene>);
			Scene* GetScen() const;
	};

	void MessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);
