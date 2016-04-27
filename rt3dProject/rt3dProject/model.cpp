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
}
Model::~Model() {
	
}

void Model::draw(GLuint shaderProgram, glm::mat4 mat4) {
	glBindTexture(GL_TEXTURE_2D, texture);
	rt3d::materialStruct tmpMaterial = material;
	rt3d::setMaterial(shaderProgram, material);


	mat4 = glm::translate(mat4, pos);
	mat4 = glm::rotate(mat4, float(rot.x * DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mat4 = glm::rotate(mat4, -float((rot.y)*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	mat4 = glm::rotate(mat4, -float((rot.z - 90)*DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	
	mat4 = glm::scale(mat4, scale);

	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mat4));
	rt3d::drawIndexedMesh(mesh, meshIndexCount, GL_TRIANGLES);
}

bool Model::loadTexture(char* fileName) {
	texture = rt3d::loadBitmap(fileName);
	if (texture != NULL)
		return true;

	return false;
}