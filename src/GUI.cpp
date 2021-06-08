#include "GUI.h"

GUI::GUI(SDL_Window* window, SDL_GLContext* context, std::shared_ptr<std::unique_ptr<Scene>> scene)
{
	_mScene = scene;
	_mWindow = window;
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
	fileDialog.SetTypeFilters({ ".glb", ".fbx", ".obj" });
	//init cursors
	DefaultCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	LoadingCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
	//Post process matrix
	Core = std::make_shared<glm::mat3>(0.0f);
	(*Core)[1][1] = 1.0f;
}
GUI::~GUI() 
{
	SDL_FreeCursor(DefaultCursor);
	SDL_FreeCursor(LoadingCursor);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}
void GUI::Draw() 
{
	Interface();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void GUI::Interface() 
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(_mWindow);

	ImGui::NewFrame();
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open new scene", "Ctrl+O"))
				{
					fileDialog.Open();
				}
				if (ImGui::MenuItem("Add new model into scene", "Ctrl+S")) {}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Postprocess affects"))
			{
				if (ImGui::MenuItem("Turn on affects", NULL, &FrameClicked))
				{
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Inversion", NULL, &invertion, FrameClicked))
				{
					convolution = false;
				}
				if (ImGui::MenuItem("Convolution", NULL, &convolution, FrameClicked))
				{
					invertion = false;
					AddWindowActive = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Background"))
			{
				if (ImGui::MenuItem("Turn on havens", NULL,&(*_mScene)->SkyBoxSetted)){}
				{
					glm::vec4 col = (*_mScene)->GetBackGround();
					ImGui::ColorEdit4("Choose color", &col[0]);
					(*_mScene)->SetBackGround(col);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();


			fileDialog.Display();

			if (fileDialog.HasSelected())
			{
				std::shared_ptr<Scene> OldScene = std::move(*_mScene);
				OldScene.reset();
				Loader loader;
				SDL_SetCursor(LoadingCursor);
				_mScene->reset(loader.GetScene(fileDialog.GetSelected().string()).release() );
				fileDialog.ClearSelected();
				SDL_SetCursor(DefaultCursor);
			}
		}
		{
			static int SCR_WIDTH, SCR_HEIGHT;
			if (_mScene)
			{
				Scene_Information info = (*_mScene)->GetInfo();
				SDL_GetWindowSize(_mWindow, &SCR_WIDTH, &SCR_HEIGHT);
				ImGui::SetNextWindowPos(ImVec2(0, 20));
				ImGui::SetNextWindowSize(ImVec2(SCR_WIDTH / 3.0f, SCR_HEIGHT / 3.0f));
				ImGui::Begin("Statistics", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
					//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", TimeOnFrame, 1000.0f / TimeOnFrame);
					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
					ImGui::Text("Time on frame %.6f ms", TimeOnFrame);
					ImGui::Text("Models in scene %d", info.Amount_models);
					ImGui::Text("Lights in scene %d", info.Amount_lights);
					ImGui::Text("Shaders in scene %d", info.Amount_shaders);
				ImGui::End();
			}

			if (convolution & AddWindowActive)
			{
				const char* buff[] = {"1","2","3","4","5","6","7","8","9"};
				ImGui::Begin("Matrix of convolution",&AddWindowActive,ImGuiWindowFlags_NoDecoration);
				{
					if (ImGui::BeginTable("", 3, ImGuiTableFlags_Borders))
					{
						for (uint16_t i = 0; i < 9; ++i)
						{
							if(!(i % 3))
								ImGui::TableNextRow(0, 20.0f);
							ImGui::TableSetColumnIndex(i % 3);
							ImGui::InputFloat(buff[i], &(*Core)[i / 3][i % 3]);
						}
					}
					ImGui::EndTable();
					if (ImGui::Button("Ok")) { AddWindowActive = false; };
					ImGui::SameLine();
					if (ImGui::Button("Cancel")) { AddWindowActive = false; convolution = false; };
				}
				ImGui::End();
			}
		}
	}
}