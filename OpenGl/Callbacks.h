#pragma once
#include "Headers.h"
#include "camera.h"

extern GLFWwindow* window;
extern std::shared_ptr <Camera> camera;
extern GLfloat deltaTime;
extern GLfloat lastFrame;
extern GLfloat currentFrame;
const GLuint SCR_WIDTH = 800, SCR_HEIGHT = 800;
//frame
extern bool keys[1024];
//mouse controll
extern GLdouble lastX, lastY;
extern GLfloat yaw;
extern GLfloat pitch;
extern GLboolean clicked;
extern GLboolean firstMouse;

bool Init();

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