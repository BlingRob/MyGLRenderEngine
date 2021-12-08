#pragma once
#include "SDL.h"
#include <stdio.h>
#include <string>
#include <cstdlib>

class Logger 
{
public:
	Logger(SDL_Window* win = nullptr):_pWindow(win)
	{
		#if defined(__WIN32__)
				//Logging
				FILE *stdinStream, *stderrStream;
				freopen_s(&stdinStream, "Log.txt", "w", stdout);
				freopen_s(&stderrStream, "ErrLog.txt", "w", stderr);
		#else
				freopen("Log.txt", "w", stdout);
				freopen("ErrLog.txt", "w", stderr);
		#endif
	}
	void ExceptionMSG(const char* msg)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception", msg, _pWindow);
		exit(EXIT_FAILURE);
	}
	void ExceptionMSG(std::string& msg)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception", msg.c_str(), _pWindow);
		exit(EXIT_FAILURE);
	}
	void ExceptionMSG(std::exception& msg)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception", msg.what(), _pWindow);
		exit(EXIT_FAILURE);
	}
	void SetWindow(SDL_Window* win) 
	{
		_pWindow = win;
	}
private:
	SDL_Window* _pWindow;
};