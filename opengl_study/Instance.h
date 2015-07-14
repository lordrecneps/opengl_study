#pragma once
#include "Model.h"
#include <glm/glm.hpp>

class Instance
{
public:
	Instance() : m(0), transform() {}
	Model* m;
	glm::mat4 transform;
};