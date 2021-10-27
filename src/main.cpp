#include "Headers.h"
#include "MemoryManager.h"
#include "Scene.h"
#include "Application.h"

void rend() 
	{
		RenderEngine eng;
		eng.GetScen()->get()->SetBackGround(glm::vec4(0.117f, 0.187f, 0.253f, 1.0f));
		std::shared_ptr<Light> lig;
		while(eng.MainLoop())
		{
			//lig = eng.GetScen()->get()->GetLight("Default light");//Sun_Orientation
			//if (lig)
			//	lig->ChangeDirection(glm::vec3(3 * sin(eng.GetTime()), 3.0f, 3 * cos(eng.GetTime())));
			//	lig->SetPos(5.0f * glm::vec3(1.3f * sin(eng.GetTime()), 2.0f, 0.2f));
			//light->SetPos(glm::vec3(sin(eng.GetTime()) * 5.0f, light->GetPos().y, cos(eng.GetTime()) * 5.0f));
		}
	}


int main(int argc, char* args[])
{
	#if defined(__WIN32__)
		//Logging
		FILE* stdinStream, * stderrStream;
		freopen_s(&stdinStream, "Log.txt", "w", stdout);
		freopen_s(&stderrStream, "ErrLog.txt", "w", stderr);
	#else
		freopen("Log.txt", "w", stdout);
		freopen("ErrLog.txt", "w", stderr);
	#endif
	//	Manager mgr;
	//	{std::shared_ptr<glm::mat4> mat = mgr.GetMat4(); }
		
	try
	{
		rend();
	}
	catch(std::exception e)
	{
		std::cerr << e.what() << "\n";
	}
	catch (const char* err) 
	{
		std::cerr << err << "\n";
	}
	catch(std::string err) 
	{
		std::cerr << err << "\n";
	}
	return 0;
}
