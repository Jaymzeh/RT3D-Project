#pragma once
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include "md2model.h"
#include <glm\glm.hpp>
#include <vector>
#include <stack>

using namespace std;

class Model{

public:

	GLuint texture;
	rt3d::materialStruct material = {
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // ambient
		{ 0.8f, 0.8f, 1.0f, 1.0f }, // diffuse
		{ 0.8f, 0.8f, 0.8f, 1.0f }, // specular
		1.0f  // shininess
	};
	GLuint mesh;
	glm::vec3 pos;
	glm::vec3 oldPos;
	glm::vec3 rot;
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

private:
	
};