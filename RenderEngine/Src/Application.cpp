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

			//Setup default scene and its settings
			SceneLoader pointloader;
			pointloader.LoadScene("../Models/PointLight.obj");
			if (pointloader.Is_Load())
			{
				Scene::DefaultPointLightModel = pointloader.GetModel(0);
				Scene::DefaultPointLightModel->SetName("PointModel");
			}

			//Create addition frame buffer
			std::pair<uint32_t, uint32_t> DM = _pWindow->MaxSize();
			frame = std::make_unique<PostProcessBuffer>(DM.first, DM.second);
			frame->AddFrameBuffer(BufferType::Color);
			frame->AddRenderBuffer(BufferType::Depth_Stencil);
			frame->Core = gui->Core;
			if (!frame->IsCorrect())
				std::cerr << "Creating addition buffer is failed";
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

	bool RenderEngine::MainLoop()
	{
		try
		{
			_pContr->dt = static_cast<float>(chron());
			if (!_pWindow->ProcEvents())
				return false;
			if (gui->FrameClicked)
				frame->AttachBuffer();
			(*_ppScene)->Draw();
			if (gui->FrameClicked)
			{
				frame->invertion = gui->invertion;
				frame->convolution = gui->convolution;
				frame->DetachBuffer();
				frame->Draw(_pWindow->SCR_WIDTH, _pWindow->SCR_HEIGHT);
			}
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