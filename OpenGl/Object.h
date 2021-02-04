#pragma once
#include "Headers.h"
#include "Model.h"
#include "Light.h"
#include "Shader.h"

struct Object
{
	Object(std::function<void(const std::list<std::shared_ptr<TypeLight>>, const std::vector< std::shared_ptr <glm::mat4>>)> f) : Draw(f) {}
	std::function< void(const std::list<std::shared_ptr<TypeLight>>,const std::vector< std::shared_ptr<glm::mat4> >) > Draw;
};

