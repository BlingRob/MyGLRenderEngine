#include "Application.h"

	RenderEngine::RenderEngine()
	{
		_mcontr = std::make_shared<Position_Controller>();
		_mWindow = std::make_unique<Window>(_mcontr);
		glViewport(0, 0, _mWindow->SCR_WIDTH, _mWindow->SCR_HEIGHT);

		//callbacks
		glDebugMessageCallback(MessageCallback, nullptr);

		//gl options
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glEnable(GL_CULL_FACE);
		glEnable(GL_MULTISAMPLE);

		glFrontFace(GL_CCW);
		//texture load option
		//stbi_set_flip_vertically_on_load(true);

		//Setup default scene and its settings
		Scene::DefaultSkyBox = Loader::LoadSkyBox({
					 "../Textures/lightblue/right.png",
					 "../Textures/lightblue/left.png",
					 "../Textures/lightblue/top.png",
					 "../Textures/lightblue/bottom.png",
					 "../Textures/lightblue/front.png",
					 "../Textures/lightblue/back.png" });
		Loader pointloader;
		pointloader.LoadScene("../Models/PointLight.obj");
		if (pointloader.Is_Load())
		{
			Scene::DefaultPointLightModel = pointloader.GetModel(0);
			Scene::DefaultPointLightModel->SetName("PointModel");
		}
		scene = std::make_shared<std::unique_ptr<Scene>>(std::make_unique<Scene>(_mcontr));
		//GUI Initialization

		gui = std::make_unique<GUI>(_mWindow->GetWindow(), _mWindow->GetContext(), scene, _mcontr);
		//Create addition frame buffer
		std::pair<uint32_t, uint32_t> DM = _mWindow->MaxSize();
		frame = std::make_unique<PostProcessBuffer>(DM.first, DM.second);
		frame->AddFrameBuffer(BufferType::Color);
		frame->AddRenderBuffer(BufferType::Depth_Stencil);
		frame->Core = gui->Core;
		if (!frame->IsCorrect())
			std::cerr << "Creating addition buffer is failed";
	}

	bool RenderEngine::MainLoop()
	{
		*_mcontr->dt = static_cast<float>(chron());
		if (!_mWindow->ProcEvents())
			return false;
		if (gui->FrameClicked)
			frame->AttachBuffer();
		(*scene)->Draw();
		if (gui->FrameClicked)
		{
			frame->invertion = gui->invertion;
			frame->convolution = gui->convolution;
			frame->DetachBuffer();
			frame->Draw(_mWindow->SCR_WIDTH, _mWindow->SCR_HEIGHT);
		}
		gui->Draw();
		_mWindow->SwapBuffer();

		return true;
	}

	double RenderEngine::GetTime()
	{
		return chron.GetTime();
	}

	void RenderEngine::SetScen(std::unique_ptr<Scene> s)
	{
		*scene = std::move(s);
	}
	const std::shared_ptr<std::unique_ptr<Scene>> RenderEngine::GetScen() const
	{
		return scene;
	}


	void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
	{
		auto source_str = [source]() -> std::string {
			switch (source)
			{
			case GL_DEBUG_SOURCE_API: return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY:  return "THIRD PARTY";
			case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER: return "OTHER";
			default: return "UNKNOWN";
			}
		}();

		auto type_str = [type]() -> std::string {
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR: return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
			case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
			case GL_DEBUG_TYPE_MARKER:  return "MARKER";
			case GL_DEBUG_TYPE_OTHER: return "OTHER";
			default: return "UNKNOWN";
			}
		}();

		auto severity_str = [severity]() -> std::string {
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
			case GL_DEBUG_SEVERITY_LOW: return "LOW";
			case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
			case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
			default: return "UNKNOWN";
			}
		}();

		if(severity_str != "NOTIFICATION")
			std::cerr
				<< source_str << ", "
				<< type_str << ", "
				<< severity_str << ", "
				<< id << ": "
				<< message << std::endl;
		else
			std::cout
				<< source_str << ", "
				<< type_str << ", "
				<< severity_str << ", "
				<< id << ": "
				<< message << std::endl;
	}
