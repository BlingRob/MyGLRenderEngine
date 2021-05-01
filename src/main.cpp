#include "Headers.h"
#include "Scene.h"
#include "Application.h"

/*
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
}*/

void Base(Scene*);
//void Ground(Scene*);
void LightSphere(Scene*);

void rend() 
	{
		/*std::shared_ptr<Model> Ground = std::make_shared<Model>("..\\Models\\Earth.obj");
		std::shared_ptr<Model> Base = std::make_shared<Model>("..\\Models\\OnlyBase.obj");
		std::shared_ptr<Shader> shaders = std::make_shared<Shader>("..\\Shaders\\Base.vs", "..\\Shaders\\Base.frag"),
			 shad2 = std::make_shared<Shader>("..\\Shaders\\GroundSh.vs", "..\\Shaders\\GroundSh.frag");*/
		/*std::unique_ptr<Camera> cam = std::make_unique<Camera>(glm::vec3(10.0f, 10.0f, 20.0f));
		std::shared_ptr <glm::mat4> model = std::make_shared<glm::mat4>(1.0f),
			view = std::make_shared<glm::mat4>(1.0f),
			proj = std::make_shared<glm::mat4>(1.0f),
			lightSpaceMatrix = std::make_shared<glm::mat4>(1.0f);
		std::unique_ptr<Scene> scen = std::make_unique<Scene>();
		std::shared_ptr<Object> obj1 = std::make_shared<Object>(Base);
		//	sphere = std::make_shared<Object>(LightSphere);
								//ground = std::make_shared<Object>(Ground);
		std::shared_ptr <Light> light = std::make_shared<Light>(0.6f, 0.09f, 0.032f, glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-1.5f, 10.0f, 0.0f));
		light->SetType(LightTypes::Point);

		scen->SetView(view);
		scen->SetModel(model);
		scen->SetProj(proj);
		
		scen->SetBackGround(glm::vec4(0.117f, 0.187f, 0.253f, 1.0f));
		scen->AddLight("PointLight",light);
		scen->AddObject("Base", obj1);
		//scen->AddObject("Sphere", sphere);
		//scen->AddObject("Ground", ground);

		scen->SetCam(std::move(cam));

		RenderEngine eng;
		eng.SetScen(std::move(scen));

		light->SetPos(glm::vec3(sin(eng.GetTime()) * 5.0f, light->GetPos().y, cos(eng.GetTime()) * 5.0f));*/
		RenderEngine eng;
		std::unique_ptr<Scene> scen = std::make_unique<Scene>();//std::string("..\\Models\\Models.glb")
		scen->SetBackGround(glm::vec4(0.117f, 0.187f, 0.253f, 1.0f));
		eng.SetScen(std::move(scen));

		while(eng.MainLoop())
		{
			//light->SetPos(glm::vec3(sin(eng.GetTime()) * 5.0f, light->GetPos().y, cos(eng.GetTime()) * 5.0f));
		}
		std::cout << std::endl;
	}


int main(int argc, char* args[])
{
	rend();

	return 0;
}

/*void Ground(Scene* scene) 
{
	try
	{
		static Shader shaders("..\\Shaders\\Base.vert", "..\\Shaders\\Base.frag");
		static Model figure("..\\Models\\Earth.obj");
		static glm::mat4 model = glm::identity<glm::mat4>();
		static glm::mat4 MVP;
		static glm::mat3 NormalMat;

		shaders.Use();

		//something
		MVP = (*scene->GetProj()) * (*scene->GetView()) * model;

		shaders.setMat("transform.view", *scene->GetView());
		shaders.setMat("transform.projection", *scene->GetProj());
		shaders.setMat("transform.model", model);
		shaders.setMat("transform.MVP", MVP);

		const auto lights = scene->GetLights();
		for (const auto& el : *lights)
			el.second->SendToShader(shaders);

		NormalMat = glm::mat3(transpose(inverse(model)));
		shaders.setMat("NormalMatrix", NormalMat);
		shaders.setVec("viewPos", scene->GetCam()->Position);

		figure.Draw(&shaders);
	}
	catch (std::string str)
	{
		std::cerr << str << std::endl;
	}
}*/
/*
void LightSphere(Scene* scene)
{
	try
	{
		static Shader shaders("..\\Shaders\\Light.vs", "..\\Shaders\\Light.frag");
		static Model figure("..\\Models\\SpotLight.obj");

		static glm::mat4 model;
		shaders.Use();
		const auto lights = scene->GetLights();
		for (const auto& el : *lights)
			el.second->SendToShader(shaders);

		model = glm::identity<glm::mat4>();
		model = glm::translate(model, scene->GetLight("PointLight")->GetPos());

		shaders.setMat("transform.view", *scene->GetView());
		shaders.setMat("transform.projection", *scene->GetProj());
		shaders.setMat("transform.model", model);

		figure.Draw(&shaders);
	}
	catch (std::string str)
	{
		std::cerr << str << std::endl;
	}
}
void Base(Scene* scene)
{
	try
	{
		static Shader shaders("..\\Shaders\\Base.vert", "..\\Shaders\\Base.frag");
		static Model figure("..\\Models\\Base.obj");

		shaders.Use();

		static glm::mat4 model;
		static glm::mat4 MVP;
		static glm::mat3 NormalMat;

		model = glm::identity<glm::mat4>();

		//something
		MVP = (*scene->GetProj()) * (*scene->GetView()) * model;

		shaders.setMat("transform.view", *scene->GetView());
		shaders.setMat("transform.projection", *scene->GetProj());
		shaders.setMat("transform.model", model);
		shaders.setMat("transform.MVP", MVP);

		//glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		const auto lights = scene->GetLights();
		for (const auto& el : *lights)
			el.second->SendToShader(shaders);
	
		NormalMat = glm::mat3(transpose(inverse(model)));
		shaders.setMat("NormalMatrix", NormalMat);
		shaders.setVec("viewPos", scene->GetCam()->Position);

		figure.Draw(&shaders);
	}
	catch (std::string str)
	{
		std::cerr << str << std::endl;
	}

}
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



