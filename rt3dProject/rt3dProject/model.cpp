#include "model.h"


Model::Model(char* modelName, char* textureName) {
	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj(modelName, verts, norms, tex_coords, indices);
	GLuint size = indices.size();
	meshIndexCount = size;

	mesh = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());
	texture = rt3d::loadBitmap(textureName);
}
Model::~Model() {
	
}

void Model::draw(GLuint shaderProgram, stack<glm::mat4>& mvStack) {
	glBindTexture(GL_TEXTURE_2D, texture);
	rt3d::materialStruct tmpMaterial = material;
	rt3d::setMaterial(shaderProgram, material);

	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), pos);
	mvStack.top() = glm::rotate(mvStack.top(), float(rot.x * DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), -float((rot.y)*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), -float((rot.z)*DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	
	mvStack.top() = glm::scale(mvStack.top(), scale);

	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(mesh, meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
}

bool Model::loadTexture(char* fileName) {
	texture = rt3d::loadBitmap(fileName);
	if (texture != NULL)
		return true;

	return false;
}