#pragma once
#include "rt3d.h"
#include "md2model.h"
#include <glm\glm.hpp>

#include <stack>

class Model {

public:

	Model() {};
	
	~Model();
	Model(char* modelName, char* textureName);

	void update();

	void draw(GLuint shaderProgram, glm::mat4 mat4);

	bool loadTexture(char* fileName);


	enum Animation{IDLE = 0, RUN = 1};
	Animation anim = IDLE;
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale = { 0.5f, 0.5f, 0.5f };
	int currentAnim = 1;

private:
	GLuint mesh;
	GLuint md2VertCount = 0;
	md2model tmpModel;
	GLuint texture;
	rt3d::materialStruct material = {
		{ 0.4f, 0.4f, 1.0f, 1.0f }, // ambient
		{ 0.8f, 0.8f, 1.0f, 1.0f }, // diffuse
		{ 0.8f, 0.8f, 0.8f, 1.0f }, // specular
		1.0f  // shininess
	};
	

	
	float animSpeed = 0.1f;
	
};