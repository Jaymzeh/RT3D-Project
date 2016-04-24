//#include "rt3d.h"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <vector>
//
//class Skybox {
//public:
//
//	Skybox() {};
//
//	Skybox(char* front, char* back, char* left, char* right,
//		char* meshName) {
//		setTexture(0, front);
//		setTexture(1, back);
//		setTexture(2, left);
//		setTexture(3, right);
//
//		int errors = 0;
//		for (int i = 0; i < SDL_arraysize(texture);i++) {
//			if (texture[i] == NULL)
//				errors++;
//		}
//
//		pos[0] = { 0.0, 0.0f, -4.0f };
//		pos[1] = { 0.0, 0.0f, 2.0f };
//		pos[2] = { -2.0, 0.0f, 0.0f };
//		pos[3] = { 2.0, 0.0f, 0.0f };
//
//		std::cout << "Skybox textures loaded with " << errors << " errors." << std::endl;
//	}
//
//	void loadMesh(char* fileName) {
//
//
//		//rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
//		//size = indices.size();
//		//mesh = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());
//	}
//
//
//	void setTexture(int index, char* fileName) {
//		texture[index] = rt3d::loadBitmap(fileName);
//	}
//
//	void draw(glm::mat4 transform, glm::mat4 modelView) {
//		/*for (int i = 0; i < SDL_arraysize(texture);i++) {
//			glBindTexture(GL_TEXTURE_2D, texture[0]);
//			transform = glm::scale(transform, scale);
//			transform = glm::translate(transform, pos[i]);
//			rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(transform));
//			rt3d::drawIndexedMesh(mesh, meshIndexCount, GL_TRIANGLES);
//		}*/
//	}
//
//	GLuint mesh;
//	int vertCount = 6;
//	GLuint texture[4];
//	glm::vec3 pos[4];
//	glm::vec3 scale{ 2.0f, 2.0f, 2.0f };
//
//	vector<GLfloat> verts;
//	vector<GLfloat> norms;
//	vector<GLfloat> tex_coords;
//	GLuint size;
//	vector<GLuint> indices;
//};
