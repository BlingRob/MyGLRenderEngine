//#include "CommonHeaders.h"
//#include "MemoryManager.h"
//#include "../Headers/Scene.h"
#include "../Headers/Application.h"

void rend() 
	{
		RenderEngine eng;
		eng.GetScen()->SetBackGround(glm::vec4(0.117f, 0.187f, 0.253f, 1.0f));
		std::shared_ptr<Light> lig;
		while(eng.MainLoop())
		{
			//lig = eng.GetScen()->GetLight("Default light");//Sun_Orientation
			//if (lig)
			//	lig->ChangeDirection(glm::vec3(3 * sin(eng.GetTime()), 3.0f, 3 * cos(eng.GetTime())));
			//	lig->SetPos(5.0f * glm::vec3(1.3f * sin(eng.GetTime()), 2.0f, 0.2f));
			//light->SetPos(glm::vec3(sin(eng.GetTime()) * 5.0f, light->GetPos().y, cos(eng.GetTime()) * 5.0f));
		}
	}

//#include "../Headers/Loaders/ResourceManager.h"
int main(int argc, char* args[])
{
	/*
		Manager mgr;
		{
			std::shared_ptr<glm::mat4> mat = mgr.GetMat<glm::mat4>(); 
			std::shared_ptr<glm::mat4> mat2 = mgr.GetMat<glm::mat4>();
			std::shared_ptr<glm::mat3> mat3 = mgr.GetMat<glm::mat3>();
			std::shared_ptr<glm::mat3> mat4 = mgr.GetMat<glm::mat3>();
			mat4.reset();
			mat.reset();
			mat2.reset();
		}*/

	rend();

	//ResourceManager mgr;
	//mgr.LoadSkyBox();
	return 0;
}
