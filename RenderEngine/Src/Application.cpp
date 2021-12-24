#include "../Headers/Application.h"

	RenderEngine::RenderEngine()
	{
		_pLog = std::make_unique<Logger>();
		try
		{
			_pContr = std::make_shared<Position_Controller>();
			_pWindow = std::make_unique<Window>(_pContr);
			_pLog->SetWindow(_pWindow->GetWindow());
			_pContext = std::make_unique<OGLRenderer>(_pWindow.get());
			_pResMgr = std::make_unique<ResourceManager>();
			//init default scene
			_ppScene = std::make_shared<std::unique_ptr<Scene>>(std::make_unique<Scene>(_pContr));
			//GUI Initialization
			gui = std::make_unique<GUI>(_pWindow.get(), _pContext.get(), _ppScene, _pContr);
		}
		catch (std::exception err)
		{
			_pLog->ExceptionMSG(err);
		}
		catch (const char* err)
		{
			_pLog->ExceptionMSG(err);
		}
		catch (std::string err)
		{
			_pLog->ExceptionMSG(err);
		}
	}

	RenderEngine::~RenderEngine() 
	{
		gui.reset(nullptr);
		_ppScene.get()->reset(nullptr);
		_pResMgr.reset(nullptr);
		_pContext.reset(nullptr);
		_pLog.reset(nullptr);
		_pWindow.reset(nullptr);
		_pContr.reset();
	}

	bool RenderEngine::MainLoop()
	{
		try
		{
			_pContr->dt = static_cast<float>(chron());
			if (!_pWindow->ProcEvents())
				return false;
			
			(*_ppScene)->Draw();

			gui->Draw();
			_pWindow->SwapBuffer();
		}
		catch (std::exception err)
		{
			_pLog->ExceptionMSG(err);
		}
		catch (const char* err)
		{
			_pLog->ExceptionMSG(err);
		}
		catch (std::string err)
		{
			_pLog->ExceptionMSG(err);
		}
		return true;
	}

	double RenderEngine::GetTime()
	{
		return chron.GetTime();
	}

	void RenderEngine::SetScen(std::unique_ptr<Scene> s)
	{
		*_ppScene = std::move(s);
	}
	Scene* RenderEngine::GetScen() const
	{
		return _ppScene.get()->get();
	}