#include "../Headers/Window.h"

Window::Window(std::shared_ptr<Position_Controller> contr)
{
	_mcontr = contr;
	//Mouse setup
	lastX = SDL_WINDOWPOS_CENTERED,
	lastY = SDL_WINDOWPOS_CENTERED;
	clicked = false;
	firstMouse = true;

	//SDL init
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		throw(std::string("Failed SDL init ") + SDL_GetError());
	//Creating SDL window
	_pWindow = SDL_CreateWindow("Render engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);// | SDL_WINDOW_FULLSCREEN_DESKTOP
	if (!_pWindow)
		throw(std::string("Failed to create SDL window ") + SDL_GetError());
}

bool Window::ProcEvents()
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
					lastX = static_cast<float>(e.button.x);
					lastY = static_cast<float>(e.button.y);
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
					float yoffset = lastY - e.button.y;

					lastX = static_cast<float>(e.button.x);
					lastY = static_cast<float>(e.button.y);

					_mcontr->cam->ProcessMouseMovement(xoffset, yoffset);
					ChangedView = true;
					ChangedProj = true;
				}
				break;
			}
			case SDL_MOUSEWHEEL:
			{
				if (clicked)
					_mcontr->cam->ProcessMouseScroll(static_cast<float>(e.wheel.y));
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

void Window::do_movement()
{
	// Camera controls
	if (keys[SDL_SCANCODE_W])
		_mcontr->cam->ProcessKeyboard(Camera::Camera_Movement::FORWARD, _mcontr->dt);
	if (keys[SDL_SCANCODE_S])
		_mcontr->cam->ProcessKeyboard(Camera::Camera_Movement::BACKWARD, _mcontr->dt);
	if (keys[SDL_SCANCODE_A])
		_mcontr->cam->ProcessKeyboard(Camera::Camera_Movement::LEFT, _mcontr->dt);
	if (keys[SDL_SCANCODE_D])
		_mcontr->cam->ProcessKeyboard(Camera::Camera_Movement::RIGHT, _mcontr->dt);
	ChangedView = true;
}

void Window::change_matrixes()
{
	if (ChangedProj)
	{
		*_mcontr->Projection = glm::perspective(glm::radians(_mcontr->cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 150.0f);
		ChangedProj = false;
	}
	if (ChangedView)
	{
		*_mcontr->View = _mcontr->cam->GetViewMatrix();
		ChangedView = false;
	}
}

std::pair<int32_t, int32_t> Window::MaxSize() const
{
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	return std::make_pair(DM.w, DM.h);
}

SDL_Window* Window::GetWindow() const
{
	return _pWindow;
}

void Window::SwapBuffer()
{
	SDL_GL_SwapWindow(_pWindow);
}

Window::~Window() 
{
	SDL_DestroyWindow(_pWindow);
}