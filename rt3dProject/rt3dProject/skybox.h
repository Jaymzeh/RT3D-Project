#pragma once
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <stack>
using namespace std;

class Skybox {
public:

	Skybox() {};

	Skybox(char* objName);

	~Skybox();

	enum Side{FRONT = 0, BACK = 1, LEFT = 2, RIGHT = 3};

	void setTexture(Side side, char* fileName);

	void draw(GLuint shaderProgram, glm::mat4 projection, ::stack<glm::mat4>& mvStack);

private:

	GLuint mesh;
	GLuint texture[4];
	glm::vec3 scale = { 2.0f, 2.0f, 2.0f };
	glm::vec3 pos[4];
	GLuint meshIndexCount = 0;

};