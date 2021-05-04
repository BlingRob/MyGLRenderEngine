#include "Application.h"

	RenderEngine::RenderEngine()
	{
		//Mouse setup
		lastX = SDL_WINDOWPOS_CENTERED, lastY = SDL_WINDOWPOS_CENTERED;
		clicked = false;
		firstMouse = true;
		//Delta time setup
		dt = 0.0f;

		//SDL init
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
			throw(std::string("Failed SDL init ") + SDL_GetError());
		//SDL_GL context setup
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		//SDL buffer init
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		//Creating SDL window
		window = SDL_CreateWindow("GL Render engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);// | SDL_WINDOW_FULLSCREEN_DESKTOP
		if (!window)
			throw(std::string("Failed to create SDL window ") + SDL_GetError());
		context = SDL_GL_CreateContext(window);
		if (!context)
			throw(std::string("Failed to create SDL context ") + SDL_GetError());

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

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
		scene = std::make_shared<std::unique_ptr<Scene>>(std::move(std::make_unique<Scene>()));
		//GUI Initialization
		gui = std::make_unique<GUI>(window, &context, scene);
		//Create addition frame buffer
		
		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);
		frame = std::make_unique<PostProcessBuffer>(DM.w, DM.h);
		frame->AddFrameBuffer(BufferType::Color);
		frame->AddRenderBuffer(BufferType::Depth_Stencil);
		frame->Core = gui->Core;
		if (!frame->IsCorrect())
			std::cerr << "Creating addition buffer is failed";
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

	bool RenderEngine::ProcEvents()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
            ImGui_ImplSDL2_ProcessEvent(&e);
			switch (e.type)
			{
				case SDL_QUIT:
				{
					return false;
				}
				case SDL_MOUSEBUTTONDOWN:
				{
					if (e.button.button == SDL_BUTTON_LEFT && keys[SDL_SCANCODE_LCTRL])
					{
						clicked = true;
						lastX = static_cast<GLfloat>(e.button.x);
						lastY = static_cast<GLfloat>(e.button.y);
						SDL_SetRelativeMouseMode(SDL_TRUE);
					}
					break;
				}
				case SDL_MOUSEBUTTONUP:
				{
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						clicked = false;
						SDL_SetRelativeMouseMode(SDL_FALSE);
					}
					break;
				}
				case SDL_MOUSEMOTION:
				{
					if (clicked)
					{
						float xoffset = e.button.x - lastX;
						float yoffset = lastY - e.button.y; // �����������, ��� ��� Y-���������� ���� ����� �����

						lastX = static_cast<GLfloat>(e.button.x);
						lastY = static_cast<GLfloat>(e.button.y);

						(*scene)->GetCam()->ProcessMouseMovement(xoffset, yoffset);
						ChangedView = true;
						ChangedProj = true;
					}
					break;
				}
				case SDL_MOUSEWHEEL:
				{
					if (clicked)
						(*scene)->GetCam()->ProcessMouseScroll(static_cast<float>(e.wheel.y));
						ChangedProj = true;
					break;
				}

				case SDL_KEYDOWN:
				{
					if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
						return false;
					keys[e.key.keysym.scancode] = true;
					break;
				}
				case SDL_KEYUP:
				{
					keys[e.key.keysym.scancode] = false;
					break;
				}

				case SDL_WINDOWEVENT:
				{

					switch (e.window.event)
					{
						case SDL_WINDOWEVENT_SIZE_CHANGED:

							SCR_WIDTH = static_cast<uint32_t>(e.window.data1);
							SCR_HEIGHT = static_cast<uint32_t>(e.window.data2);
							glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
							ChangedProj = true;
						break;
						default:
							break;
					}
				}
			}
		}
		//Apply keys
		do_movement();
		change_matrixes();

		return true;
	}

	bool RenderEngine::MainLoop()
	{
		dt = static_cast<float>(chron());
		if(!ProcEvents())
			return false;
		if(gui->FrameClicked)
			frame->AttachBuffer();
		(*scene)->Draw();
		if (gui->FrameClicked)
		{
			frame->invertion = gui->invertion;
			frame->convolution = gui->convolution;
			frame->DetachBuffer();
			frame->Draw(SCR_WIDTH, SCR_HEIGHT);
		}
		gui->Draw();
		SDL_GL_SwapWindow(window);

		return true;
	}

	void RenderEngine::do_movement()
	{
			// Camera controls
		if (keys[SDL_SCANCODE_W])
			(*scene)->GetCam()->ProcessKeyboard(Camera::Camera_Movement::FORWARD, dt);
		if (keys[SDL_SCANCODE_S])
			(*scene)->GetCam()->ProcessKeyboard(Camera::Camera_Movement::BACKWARD, dt);
		if (keys[SDL_SCANCODE_A])
			(*scene)->GetCam()->ProcessKeyboard(Camera::Camera_Movement::LEFT, dt);
		if (keys[SDL_SCANCODE_D])
			(*scene)->GetCam()->ProcessKeyboard(Camera::Camera_Movement::RIGHT, dt);
		ChangedView = true;
	}
	void RenderEngine::change_matrixes()
	{
		if (ChangedProj)
		{
			(*scene)->matrs.Projection = std::make_shared<glm::mat4>(std::move(glm::perspective(glm::radians((*scene)->GetCam()->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 150.0f)));
			ChangedProj = false;
		}
		if (ChangedView)
		{
			(*scene)->matrs.View = std::make_shared<glm::mat4>(std::move((*scene)->GetCam()->GetViewMatrix()));
			ChangedView = false;
		}
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

		auto type_str = [type]() {
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

		auto severity_str = [severity]() {
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
			case GL_DEBUG_SEVERITY_LOW: return "LOW";
			case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
			case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
			default: return "UNKNOWN";
			}
		}();

		std::cerr
			<< source_str << ", "
			<< type_str << ", "
			<< severity_str << ", "
			<< id << ": "
			<< message << std::endl;
	}
