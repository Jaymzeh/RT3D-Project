#include "skybox.h"

Skybox::Skybox(char* fileName) {

	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj(fileName, verts, norms, tex_coords, indices);
	GLuint size = indices.size();
	meshIndexCount = size;

	mesh = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());

	pos[0] = { 0.0f, 0.0f, -2.0f };
	pos[1] = { 0.0f, 0.0f, 2.0f };
	pos[2] = { -2.0f, 0.0f, 0.0f };
	pos[3] = { 2.0f, 0.0f, 0.0f };
}

Skybox::~Skybox() {

}

void Skybox::setTexture(Side side, char* fileName) {
	texture[side] = rt3d::loadBitmap(fileName);
}

void Skybox::draw(GLuint shaderProgram, glm::mat4 projection, stack<glm::mat4>& mvStack) {

	glUseProgram(shaderProgram);
	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));

	glDepthMask(GL_FALSE);
	glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
	mvStack.push(glm::mat4(mvRotOnlyMat3));

	for (int i = 0; i < SDL_arraysize(texture); i++) {
		mvStack.push(mvStack.top());
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		mvStack.top() = glm::scale(mvStack.top(), scale);
		mvStack.top() = glm::translate(mvStack.top(), pos[i]);
		rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
		rt3d::drawIndexedMesh(mesh, meshIndexCount, GL_TRIANGLES);
		mvStack.pop();
	}
	mvStack.pop();
	glDepthMask(GL_TRUE);

	/*glBindTexture(GL_TEXTURE_2D, texture[side]);
	mat4 = glm::scale(mat4, scale);
	mat4 = glm::translate(mat4, pos[side]);
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mat4));
	rt3d::drawIndexedMesh(mesh, meshIndexCount, GL_TRIANGLES);*/
}