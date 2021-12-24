#include "../Headers/GUI.h"

GUI::GUI(const Window* window, void* context, std::shared_ptr<std::unique_ptr<Scene>> scene, std::shared_ptr<Position_Controller> contr)
{
	_ppScene = scene;
	_pWindow = window;
	_pContr = contr;
	std::pair<uint32_t, uint32_t> rect = _pWindow->MaxSize();
	_pPostEffect = std::make_unique<ProcessEffect>(rect.first, rect.second);
	//GUI Initialization
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.Fonts->AddFontDefault();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends for imgui
	ImGui_ImplSDL2_InitForOpenGL(window->GetWindow(), context);
	ImGui_ImplOpenGL3_Init("#version 450");
	// Setup file dialog
	fileDialog.SetTitle("Choosing scene");
	fileDialog.SetTypeFilters({ ".glb", ".fbx", ".obj" });
	//init cursors
	DefaultCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	LoadingCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
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
	if (FrameClicked)
	{
		_pPostEffect->DetachBuffer();
		_pPostEffect->Draw(_pWindow->SCR_WIDTH, _pWindow->SCR_HEIGHT);
	}

	Interface();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (FrameClicked)
		_pPostEffect->AttachBuffer();
}
void GUI::Interface() 
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(_pWindow->GetWindow());

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
				if (ImGui::MenuItem("Inversion", NULL, &_pPostEffect->invertion, FrameClicked))
				{
					_pPostEffect->convolution = false;
				}
				if (ImGui::MenuItem("Convolution", NULL, &_pPostEffect->convolution, FrameClicked))
				{
					_pPostEffect->invertion = false;
					AddWindowActive = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Background"))
			{
				if (ImGui::MenuItem("Turn on havens", NULL,&(*_ppScene)->SkyBoxSetted)){}
				{
					glm::vec4 col = (*_ppScene)->GetBackGround();
					ImGui::ColorEdit4("Choose color", &col[0]);
					(*_ppScene)->SetBackGround(col);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();


			fileDialog.Display();

			if (fileDialog.HasSelected())
			{
				std::shared_ptr<Scene> OldScene = std::move(*_ppScene);
				OldScene.reset();
				SceneLoader loader;
				SDL_SetCursor(LoadingCursor);
				loader.LoadScene(fileDialog.GetSelected().string());
				if(loader.IsLoad())
					*_ppScene = loader.GetScene();
				_ppScene->get()->SetController(_pContr);
				fileDialog.ClearSelected();
				SDL_SetCursor(DefaultCursor);
			}
		}
		{
			static int SCR_WIDTH, SCR_HEIGHT;
			if (_ppScene)
			{
				Scene_Information info = (*_ppScene)->GetInfo();
				//SDL_GetWindowSize(_pWindow->GetWindow(), &SCR_WIDTH, &SCR_HEIGHT);
				ImGui::SetNextWindowPos(ImVec2(0, 20));
				ImGui::SetNextWindowSize(ImVec2(_pWindow->SCR_WIDTH/ 3.0f, _pWindow->SCR_HEIGHT / 3.0f));
				ImGui::Begin("Statistics", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
					//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", TimeOnFrame, 1000.0f / TimeOnFrame);
					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
					ImGui::Text("Time on frame %.6f ms", _pContr->dt);
					ImGui::Text("Models in scene %d", info.Amount_models);
					ImGui::Text("Lights in scene %d", info.Amount_lights);
					ImGui::Text("Shaders in scene %d", info.Amount_shaders);
				ImGui::End();
			}

			if (_pPostEffect->convolution & AddWindowActive)
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
							ImGui::InputFloat(buff[i], &(*_pPostEffect->Core)[i / 3][i % 3]);
						}
					}
					ImGui::EndTable();
					if (ImGui::Button("Ok")) { AddWindowActive = false; };
					ImGui::SameLine();
					if (ImGui::Button("Cancel")) { AddWindowActive = false; _pPostEffect->convolution = false; };
				}
				ImGui::End();
			}
		}
	}
}