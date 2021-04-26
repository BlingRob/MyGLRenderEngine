#pragma once
#include "Headers.h"
#include <SDL.h>
#include "Loader.h"
//GUI
#define IMGUI_INCLUDE_IMCONFIG_H
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imfilebrowser.h"
class GUI
{
public:
	GUI(SDL_Window* window, SDL_GLContext* context,std::shared_ptr<std::unique_ptr<Scene>> scene);
	~GUI();
	void Draw();

	bool FrameClicked = false;
	bool invertion = false;
	bool convolution = false;
	std::shared_ptr<glm::mat3> Core;
private:
	std::shared_ptr<std::unique_ptr<Scene>> _mScene;
	SDL_Window* _mWindow;
	ImGui::FileBrowser fileDialog;
	SDL_Cursor *DefaultCursor,
			   *LoadingCursor;

	bool AddWindowActive = false;
	void Interface();
};

