#pragma once
#include "Headers.h"
#include "camera.h"

namespace Application
{
	class RenderEngine;
	GLFWwindow* window;
	std::shared_ptr <Camera> camera;

	GLfloat dt,t,lt;
	GLuint SCR_WIDTH = 800, SCR_HEIGHT = 800;
	//frame
	bool keys[1024];
	//mouse controll
	GLdouble lastX, lastY;
	GLfloat yaw, pitch;
	GLboolean clicked, firstMouse;

	bool Init();
	void Render();
	void Resize();
	void Exit();

	//io
	void GLAPIENTRY MessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void mouse_click(GLFWwindow* window, int button, int action, int mods);

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	void do_movement();

};

