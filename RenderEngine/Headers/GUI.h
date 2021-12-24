#pragma once
#include "SDL.h"
#include "Loaders/SceneLoader.h"
#include "Window.h"
#include "Transformation.h"
//GUI
#define IMGUI_INCLUDE_IMCONFIG_H
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imfilebrowser.h"
#include "PostProcessEffects.h"

class GUI
{
public:
	GUI(const Window* window, void* context, std::shared_ptr<std::unique_ptr<Scene>> scene, std::shared_ptr<Position_Controller>);
	~GUI();
	void Draw();

private:
	std::shared_ptr<std::unique_ptr<Scene>> _ppScene;
	const Window* _pWindow;
	std::shared_ptr<Position_Controller> _pContr;
	std::unique_ptr<ProcessEffect> _pPostEffect;
	ImGui::FileBrowser fileDialog;
	SDL_Cursor *DefaultCursor,
			   *LoadingCursor;

	bool AddWindowActive = false;
	bool FrameClicked = false;
	void Interface();
};

