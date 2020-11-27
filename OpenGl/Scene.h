#pragma once
#include "Headers.h"
#include "Light.h" 
#include "Model.h"
#include "camera.h"

class Scene
{
	Camera camera;
	std::list<std::shared_ptr<std::pair<Model,std::function<void()>>>> models_op;
	std::list<std::shared_ptr<Light>> lights;
	public:
		//void Draw();

};

