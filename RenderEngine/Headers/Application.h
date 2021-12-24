#pragma once
#include "gl/gl.h"
#include "Time.h"
#include "Scene.h"
#include "Logger.h"
//interface libs
//GUI
#include "GUI.h"
//Loader
#include "Loaders/ResourceManager.h"
//Frame buffer
#include "OGLSpec/OGLRenderer.h"

class RenderEngine
	{
		std::shared_ptr<Position_Controller> _pContr;
		//Pointer of window class
		std::unique_ptr<Window> _pWindow;
		//GLcontext
		std::unique_ptr<OGLRenderer> _pContext;
		//Scene
		std::shared_ptr<std::unique_ptr<Scene>> _ppScene;
		//Time
		Chronometr chron;
		//GUI
		std::unique_ptr<GUI> gui;
		//Logger
		std::unique_ptr<Logger> _pLog;
		//Resource manager
		std::unique_ptr<ResourceManager> _pResMgr;
		public:
			RenderEngine();
			~RenderEngine();

			bool MainLoop();
			double GetTime();
			void SetScen(std::unique_ptr<Scene>);
			Scene* GetScen() const;
	};

