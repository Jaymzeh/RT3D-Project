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

	Model() {};
	
	~Model();
	Model(char* modelName, char* textureName);

	void draw(GLuint shaderProgram, stack<glm::mat4>& mvStack);

	bool loadTexture(char* fileName);

	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

private:
	GLuint mesh;
	GLuint meshIndexCount;
	GLuint texture;
	rt3d::materialStruct material = {
		{ 0.4f, 0.4f, 1.0f, 1.0f }, // ambient
		{ 0.8f, 0.8f, 1.0f, 1.0f }, // diffuse
		{ 0.8f, 0.8f, 0.8f, 1.0f }, // specular
		1.0f  // shininess
	};
};