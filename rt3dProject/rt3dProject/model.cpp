#include "model.h"

Model::Model(char* modelName, char* textureName) {
	texture = rt3d::loadBitmap(textureName);
	mesh = tmpModel.ReadMD2Model(modelName);
	md2VertCount = tmpModel.getVertDataCount();

	rot.x = 90;
}
Model::~Model() {
	
}
void Model::update() {
	tmpModel.Animate(currentAnim, 0.1);
	rt3d::updateMesh(mesh, RT3D_VERTEX, tmpModel.getAnimVerts(), tmpModel.getVertDataSize());
}

void Model::draw(GLuint shaderProgram, glm::mat4 mat4) {

	

	glCullFace(GL_FRONT); // md2 faces are defined clockwise, so cull front face
	glBindTexture(GL_TEXTURE_2D, texture);
	rt3d::materialStruct tmpMaterial = material;
	rt3d::setMaterial(shaderProgram, material);


	mat4 = glm::translate(mat4, pos);
	mat4 = glm::rotate(mat4, float(rot.x * DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mat4 = glm::rotate(mat4, -float((rot.y)*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	mat4 = glm::rotate(mat4, -float((rot.z - 90)*DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	
	mat4 = glm::scale(mat4, scale);

	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mat4));
	rt3d::drawMesh(mesh, md2VertCount, GL_TRIANGLES);
	glCullFace(GL_BACK);
}

bool Model::loadTexture(char* fileName) {
	texture = rt3d::loadBitmap(fileName);
	if (texture != NULL)
		return true;

	return false;
}