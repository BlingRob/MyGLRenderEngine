#include "Window.h"

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
	window = std::shared_ptr<SDL_Window>(SDL_CreateWindow("GL Render engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE),
		[](SDL_Window* ptr) {SDL_DestroyWindow(ptr); });// | SDL_WINDOW_FULLSCREEN_DESKTOP
	if (!window)
		throw(std::string("Failed to create SDL window ") + SDL_GetError());
	context = std::shared_ptr<void>(SDL_GL_CreateContext(window.get()), [](void* ptr) {SDL_GL_DeleteContext(ptr); }); 
	if (!context)
		throw(std::string("Failed to create OpenGL context ") + SDL_GetError());
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
				float yoffset = lastY - e.button.y;

				lastX = static_cast<GLfloat>(e.button.x);
				lastY = static_cast<GLfloat>(e.button.y);

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
		_mcontr->cam->ProcessKeyboard(Camera::Camera_Movement::FORWARD, *_mcontr->dt);
	if (keys[SDL_SCANCODE_S])
		_mcontr->cam->ProcessKeyboard(Camera::Camera_Movement::BACKWARD, *_mcontr->dt);
	if (keys[SDL_SCANCODE_A])
		_mcontr->cam->ProcessKeyboard(Camera::Camera_Movement::LEFT, *_mcontr->dt);
	if (keys[SDL_SCANCODE_D])
		_mcontr->cam->ProcessKeyboard(Camera::Camera_Movement::RIGHT, *_mcontr->dt);
	ChangedView = true;
}

void Window::change_matrixes()
{
	if (ChangedProj)
	{
		*_mcontr->Projection = std::move(glm::perspective(glm::radians(_mcontr->cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 150.0f));
		ChangedProj = false;
	}
	if (ChangedView)
	{
		*_mcontr->View = std::move(_mcontr->cam->GetViewMatrix());
		ChangedView = false;
	}
}

std::pair<int32_t, int32_t> Window::MaxSize()
{
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	return std::make_pair(DM.w, DM.h);
}

std::shared_ptr <void> Window::GetContext() 
{
	return context;
}
std::shared_ptr<SDL_Window> Window::GetWindow()
{
	return window;
}
void Window::SwapBuffer() 
{
	SDL_GL_SwapWindow(window.get());
}