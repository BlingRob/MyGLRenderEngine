#include "Headers.h"
#include "Shader.h"
#include "Model.h"
#include "Callbacks.h"
#include "Light.h"

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

/*struct PointLight
{
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};*/

PointLight light(0.6f, 0.09f, 0.032f,glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 15.0f, 0.0f));
/*void LightInit()
{
	light.position = glm::vec3(0.0f, 15.0f, 0.0f);
	light.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	light.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	light.constant = 0.6f;
	light.linear = 0.09f;
	light.quadratic = 0.032f;
}*/
//void LightInit();

glm::mat4 model(1.0f), view(1.0f), proj(1.0f), NormalMat(1.0f), lightSpaceMatrix(1.0f);

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

inline void Light();
inline void Scene(std::shared_ptr <Model> Scen, std::shared_ptr < Shader> shaders);

void rend() 
	{
		std::shared_ptr<Model> Ground = std::make_shared<Model>("..\\Models\\Earth.obj");
		std::shared_ptr<Model> Base = std::make_shared<Model>("..\\Models\\OnlyBase.obj");
		std::shared_ptr<Shader> shaders = std::make_shared<Shader>("..\\Shaders\\Base.vs", "..\\Shaders\\Base.frag"),
			 shad2 = std::make_shared<Shader>("..\\Shaders\\GroundSh.vs", "..\\Shaders\\GroundSh.frag");
		//	*depthsh = new Shader("C:\\VSProg\\GLTemplate\\Shaders\\DepthMap.vs", "C:\\VSProg\\GLTemplate\\Shaders\\DepthMap.frag");
		//shadowMap();
		
		//LightInit();
		//world transform
		while (!glfwWindowShouldClose(window))
		{
			currentFrame = static_cast<GLfloat>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			glfwPollEvents();
			do_movement();
			light.SetPos(glm::vec3(sin(glfwGetTime()) * 5.0f,light.GetPos().y, cos(glfwGetTime()) * 5.0f));
			//light.position.x = ;
			//light.position.z = ;

			//1.
			//glViewport(0,0, 1024, 1024);
			//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			//glClear(GL_DEPTH_BUFFER_BIT);

			proj = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			//view = glm::lookAt(glm::vec3(light.position.x, light.position.y, light.position.z),
			//	glm::vec3(0.0f, 0.0f, 0.0f),
		//		glm::vec3(0.0f, 1.0f, 0.0f));

		//	lightSpaceMatrix = proj * view;

			//Light();
			//Scene(Base, depthsh);
			//Scene(Ground, depthsh);

			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
    		//glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
			glClearColor(0.117f, 0.187f, 0.253f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			view = camera->GetViewMatrix();
			//glUniform1i(glGetUniformLocation(shader->Program, "shadowMap"), 0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			Light();
			Scene(Base,shaders);
			Scene(Ground, shad2);
			
			glfwSwapBuffers(window);
		}
	}


int main() 
{
	Init();
	rend();

	glfwTerminate();
	return 0;
}



inline void Scene(std::shared_ptr <Model> Scen, std::shared_ptr < Shader> shaders)
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
	static std::shared_ptr<Shader> shaders = std::make_shared<Shader>("C:\\VSProg\\GLTemplate\\Shaders\\Light.vs", "C:\\VSProg\\GLTemplate\\Shaders\\Light.frag");
	static Model Scen("C:\\VSProg\\GLTemplate\\Models\\SpotLight.obj");
	shaders->Use();

	light.SendToShader(shaders);

	model = glm::identity<glm::mat4>();

	model = glm::translate(model, light.GetPos());

	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "transform.model"), 1, GL_FALSE, glm::value_ptr(model));

	Scen.Draw(shaders.get());
}



