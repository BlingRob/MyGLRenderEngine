#include "Headers.h"
#include "Shader.h"
#include "Model.h"
#include "Callbacks.h"
#include "Light.h"
//#include "Object.h"
#include "Scene.h"


GLFWwindow* window;
std::shared_ptr <Camera> camera;
GLfloat deltaTime;
GLfloat lastFrame;
GLfloat currentFrame;
//frame
bool keys[1024];
//mouse controll
GLdouble lastX, lastY;
GLfloat yaw;
GLfloat pitch;
GLboolean clicked;
GLboolean firstMouse;




GLuint depthMapFBO, depthMap;
inline void shadowMap() 
{
	//glCreateFramebuffers(1, &depthMapFBO);
	glGenFramebuffers(1, &depthMapFBO);
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Base(Scene*);
void Ground(Scene*);
void LightSphere(Scene*);

void rend() 
	{
		/*std::shared_ptr<Model> Ground = std::make_shared<Model>("..\\Models\\Earth.obj");
		std::shared_ptr<Model> Base = std::make_shared<Model>("..\\Models\\OnlyBase.obj");
		std::shared_ptr<Shader> shaders = std::make_shared<Shader>("..\\Shaders\\Base.vs", "..\\Shaders\\Base.frag"),
			 shad2 = std::make_shared<Shader>("..\\Shaders\\GroundSh.vs", "..\\Shaders\\GroundSh.frag");*/
		
		std::shared_ptr <glm::mat4> model = std::make_shared<glm::mat4>(1.0f),
			view = std::make_shared<glm::mat4>(1.0f),
			proj = std::make_shared<glm::mat4>(1.0f),
			lightSpaceMatrix = std::make_shared<glm::mat4>(1.0f);
		Scene scen;
		std::shared_ptr<Object> obj1 = std::make_shared<Object>(Base),
								sphere = std::make_shared<Object>(LightSphere);
		std::shared_ptr <Light> light = std::make_shared<Light>(0.6f, 0.09f, 0.032f, glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 15.0f, 0.0f));
		light->SetType(LightTypes::Point);

		scen.SetView(view);
		scen.SetModel(model);
		scen.SetProj(proj);
		
		scen.SetBackGround(glm::vec4(0.117f, 0.187f, 0.253f, 1.0f));
		scen.AddLight("PointLight",light);
		scen.AddObject("Base", obj1);
		scen.AddObject("Sphere", sphere);
		
		//world transform
		while (!glfwWindowShouldClose(window))
		{
			currentFrame = static_cast<GLfloat>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			glfwPollEvents();
			do_movement();

			light->SetPos(glm::vec3(sin(glfwGetTime()) * 5.0f, light->GetPos().y, cos(glfwGetTime()) * 5.0f));

			*proj = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			*view = camera->GetViewMatrix();
			scen.Draw();
			
			glfwSwapBuffers(window);
		}
		std::cout << std::endl;
	}


int main() 
{
	Init();
	rend();

	glfwTerminate();
	return 0;
}

void Ground(Scene*);

void LightSphere(Scene* scene)
{
	static std::shared_ptr<Shader> shaders = std::make_shared<Shader>("..\\Shaders\\Light.vs", "..\\Shaders\\Light.frag");
	static Model Scen("..\\Models\\SpotLight.obj");
	shaders->Use();

	for (auto el : scene->Lights)
		el.second->SendToShader(shaders);

	glm::mat4 model = glm::identity<glm::mat4>();
	model = glm::translate(model, scene->GetLight("PointLight")->GetPos());

	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.view"), 1, GL_FALSE, glm::value_ptr(*scene->matrixs[1]));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.projection"), 1, GL_FALSE, glm::value_ptr(*scene->matrixs[2]));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.model"), 1, GL_FALSE, glm::value_ptr(model));

	Scen.Draw(shaders.get());
}
void Base(Scene* scene)
{
	static std::shared_ptr<Model> Base = std::make_shared<Model>("..\\Models\\OnlyBase.obj");
	static std::shared_ptr<Shader> shaders = std::make_shared<Shader>("..\\Shaders\\Base.vert", "..\\Shaders\\Base.frag");
	shaders->Use();

	static glm::mat4 model = glm::identity<glm::mat4>();
	static glm::mat4 MVP;


	//something
	MVP = (*scene->matrixs[2]) * (*scene->matrixs[1]) *  model;

	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.view"), 1, GL_FALSE, glm::value_ptr(*scene->matrixs[1]));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.projection"), 1, GL_FALSE, glm::value_ptr(*scene->matrixs[2]));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	//glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	for (auto el : scene->Lights)
		el.second->SendToShader(shaders);

	static glm::mat4 NormalMat = glm::mat3(transpose(inverse(model)));
	glUniformMatrix3fv(glGetUniformLocation(shaders->Program, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(NormalMat));

	glUniform3f(glGetUniformLocation(shaders->Program, "viewPos"), camera->Position.x, camera->Position.y, camera->Position.z);

	Base->Draw(shaders.get());

}
/*
inline void scene(std::shared_ptr <Model> Scen, std::shared_ptr < Shader> shaders)
{
	shaders->Use();

	model = glm::identity<glm::mat4>();

	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	light.SendToShader(shaders);

	NormalMat = glm::mat3(transpose(inverse(model)));
	glUniformMatrix3fv(glGetUniformLocation(shaders->Program, "NormalMatr"), 1, GL_FALSE, glm::value_ptr(NormalMat));

	glUniform3f(glGetUniformLocation(shaders->Program, "viewPos"), camera->Position.x, camera->Position.y, camera->Position.z);
	//glUniform1i(glGetUniformLocation(shaders->Program, "shadowMap"), 10);

	Scen->Draw(shaders.get());
}

inline void Light()
{
	static std::shared_ptr<Shader> shaders = std::make_shared<Shader>("..\\Shaders\\Light.vs", "..\\Shaders\\Light.frag");
	static Model Scen("..\\Models\\SpotLight.obj");
	shaders->Use();

	light.SendToShader(shaders);

	model = glm::identity<glm::mat4>();

	model = glm::translate(model, light.GetPos());

	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.model"), 1, GL_FALSE, glm::value_ptr(model));

	Scen.Draw(shaders.get());
}*/



