#include "Application.h"

void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
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

	RenderEngine::RenderEngine() 
	{
		//Mouse setup
		lastX = SDL_WINDOWPOS_CENTERED, lastY = SDL_WINDOWPOS_CENTERED;
		clicked = false;
		firstMouse = true;
		//Delta time setup
		dt = 0.0f;

		//SDL init
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

		glFrontFace(GL_CCW);
		//texture load option
		//stbi_set_flip_vertically_on_load(true);

		//GUI Initialization
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.Fonts->AddFontDefault();
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends for imgui
		ImGui_ImplSDL2_InitForOpenGL(window, context);
		ImGui_ImplOpenGL3_Init("#version 450");
		// Setup file dialog 
		fileDialog.SetTitle("Choosing scene");
		fileDialog.SetTypeFilters({ ".glb", ".fbx", ".obj"});
		//init cursors
		DefaultCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		LoadingCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
		//Setup defaul scene
		scene = std::make_unique<Scene>();
		//Logging
		FILE* stdinStream,*stderrStream;
		freopen_s(&stdinStream,"Log.txt", "w", stdout);
		freopen_s(&stderrStream,"ErrLog.txt", "w", stderr);

	}

	double RenderEngine::GetTime() 
	{
		return chron.GetTime();
	}

	void RenderEngine::SetScen(std::unique_ptr<Scene> s)
	{
		scene = std::move(s);
	}

	bool RenderEngine::ProcEvents() 
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				ImGui_ImplSDL2_ProcessEvent(&e);
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
						float yoffset = lastY - e.button.y; // перевернуто, так как Y-координаты идут снизу вверх

						lastX = static_cast<GLfloat>(e.button.x);
						lastY = static_cast<GLfloat>(e.button.y);

						scene->GetCam()->ProcessMouseMovement(xoffset, yoffset);
						ChangedView = true;
						ChangedProj = true;
					}
					break;
				}
				case SDL_MOUSEWHEEL: 
				{
					if (clicked)
						scene->GetCam()->ProcessMouseScroll(static_cast<float>(e.wheel.y));
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
		return true;
	}

	bool RenderEngine::MainLoop()
	{
		dt = static_cast<float>(chron());
		if(!ProcEvents())
			return false;
		do_movement();
		GUIproc();
		if (ChangedProj)
		{
			scene->matrs.Projection = std::make_shared<glm::mat4>(std::move(glm::perspective(glm::radians(scene->GetCam()->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 150.0f)));
			ChangedProj = false;
		}
		if (ChangedView)
		{
			scene->matrs.View = std::make_shared<glm::mat4>(std::move(scene->GetCam()->GetViewMatrix()));
			ChangedView = false;
		}

		std::shared_ptr<Light> lig = scene->GetLight("Point_Orientation");
		if(lig)
			lig->SetPos(glm::vec3(sin(GetTime()) * 5.0f, lig->GetPos().y, sin(GetTime()) * 5.0f));

		scene->Draw();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
		return true;
	}

	void RenderEngine::do_movement()
	{
			// Camera controls
		if (keys[SDL_SCANCODE_W])
			scene->GetCam()->ProcessKeyboard(Camera::Camera_Movement::FORWARD, dt);
		if (keys[SDL_SCANCODE_S])
			scene->GetCam()->ProcessKeyboard(Camera::Camera_Movement::BACKWARD, dt);
		if (keys[SDL_SCANCODE_A])
			scene->GetCam()->ProcessKeyboard(Camera::Camera_Movement::LEFT, dt);
		if (keys[SDL_SCANCODE_D])
			scene->GetCam()->ProcessKeyboard(Camera::Camera_Movement::RIGHT, dt);
		ChangedView = true;
	}

	const Scene* RenderEngine::GetScen() const
	{
		return scene.get();
	}

	void RenderEngine::GUIproc() 
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		
		ImGui::NewFrame();
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Open..", "Ctrl+O"))
					{
						fileDialog.Open();
					}
					if (ImGui::MenuItem("Save", "Ctrl+S")) {  }
					if (ImGui::MenuItem("Close", "Ctrl+W")) {}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();

				fileDialog.Display();

				if (fileDialog.HasSelected())
				{
					std::unique_ptr<Scene> OldScene = std::move(scene);
					Loader loader;
					SDL_SetCursor(LoadingCursor);
					scene = std::move(loader.GetScene(fileDialog.GetSelected().string()));
					scene->SetBackGround(OldScene->RetSkyBoxGround());
					//std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
					fileDialog.ClearSelected();
					SDL_SetCursor(DefaultCursor);
				}
			}
			
			ImGui::SetNextWindowPos(ImVec2(0, 15));
			ImGui::SetNextWindowSize(ImVec2(SCR_WIDTH / 3.0f, SCR_HEIGHT / 3.0f));
			ImGui::Begin("Tools menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
	}