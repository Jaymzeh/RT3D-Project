// MD2 animation renderer
// This demo will load and render an animated MD2 model, an OBJ model and a skybox
// Most of the OpenGL code for dealing with buffer objects, etc has been moved to a 
// utility library, to make creation and display of mesh objects as simple as possible

// Windows specific: Uncomment the following line to open a console window for debug output
#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include "rt3d.h"
#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>
#include "md2model.h"
#include "model.h"
#include "bass.h"

#include <SDL_ttf.h>

using namespace std;

#include "skybox.h"

TTF_Font * textFont;

GLuint textToTexture(const char * str/*, TTF_Font *font, SDL_Color colour, GLuint &w,GLuint &h */);


// Globals
// Real programs don't use globals :-D

GLuint meshIndexCount = 0;
GLuint md2VertCount = 0;
GLuint meshObjects[2];

GLuint shaderProgram;
GLuint skyboxProgram;

GLfloat rot = 0.0f;

glm::vec3 eye(0.0f, 1.0f, 0.0f);
glm::vec3 at(0.0f, 1.0f, -1.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

glm::vec3 playerPos(0.0f, 1.0f, 0.0f);
glm::vec3 oldPlayerPos(0.0f, 1.0f, 0.0f);
glm::vec3 playerScale(0.05f, 0.05f, 0.05f);

stack<glm::mat4> mvStack;

// TEXTURE STUFF
GLuint textures[5];
GLuint labels[5];

rt3d::lightStruct light0 = {
	{ 0.3f, 0.3f, 0.3f, 1.0f }, // ambient
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
	{ -10.0f, 10.0f, 10.0f, 1.0f }  // position
};
glm::vec4 lightPos(-10.0f, 10.0f, 10.0f, 1.0f); //light position

rt3d::materialStruct material0 = {
	{ 0.2f, 0.4f, 0.2f, 1.0f }, // ambient
	{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
	{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
	2.0f  // shininess
};
rt3d::materialStruct material1 = {
	{ 0.4f, 0.4f, 1.0f, 1.0f }, // ambient
	{ 0.8f, 0.8f, 1.0f, 1.0f }, // diffuse
	{ 0.8f, 0.8f, 0.8f, 1.0f }, // specular
	1.0f  // shininess
};

// md2 stuff
md2model tmpModel;
int currentAnim = 0;

int score = 0;

HSAMPLE sample;
HCHANNEL channel;

Model ground[2];
Model building[7];
Model box;

Skybox newSkybox;

// Set up rendering context
SDL_Window * setupRC(SDL_GLContext &context) {
	SDL_Window * window;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		rt3d::exitFatalError("Unable to initialize SDL");

	// Request an OpenGL 3.0 context.

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

													   // Create 800x600 window
	window = SDL_CreateWindow("SDL/GLM/OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
	return window;
}

HSAMPLE loadSample(char * filename) {
	HSAMPLE sam;
	if (sam = BASS_SampleLoad(FALSE, filename, 0, 0, 3, BASS_SAMPLE_OVER_POS))
		cout << "sample " << filename << " loaded!" << endl;
	else {
		cout << "Can't load sample";
		exit(0);
	}
	return sam;
}


void init(void) {
	// For this simple example we'll be using the most basic of shader programs
	shaderProgram = rt3d::initShaders("phong-tex.vert", "phong-tex.frag");
	rt3d::setLight(shaderProgram, light0);
	rt3d::setMaterial(shaderProgram, material0);

	skyboxProgram = rt3d::initShaders("textured.vert", "textured.frag");

	textures[1] = rt3d::loadBitmap("hobgoblin2.bmp");
	meshObjects[1] = tmpModel.ReadMD2Model("tris.MD2");
	md2VertCount = tmpModel.getVertDataCount();

	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	GLuint size = indices.size();
	meshIndexCount = size;
	textures[0] = rt3d::loadBitmap("ground.bmp");
	meshObjects[0] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());

	textures[2] = rt3d::loadBitmap("building.bmp");
	textures[3] = rt3d::loadBitmap("fence.bmp");

	textures[4] = rt3d::loadBitmap("box.bmp");

	//ground
	ground[0].pos = { 0.0f, 0.0f, 0.0f };
	ground[0].scale = { 20.0f, 0.2f, 20.0f };
	ground[0].texture = textures[0];
	
	ground[1].pos = { 40.0f,0.0f ,10.0f };
	ground[1].scale = { 20.0f,0.2f,10.0f };
	ground[1].texture = textures[0];

	//front
	building[0].pos = { -10.0f, 2.0f, -21.0f };
	building[0].scale = { 10.0f, 2.0f, 1.0f };
	building[0].texture = textures[2];

	building[1].pos = { 10.0f, 2.5f, -21.0f };
	building[1].scale = { 10.0f, 2.5f, 1.0f };
	building[1].texture = textures[2];

	//left
	building[2].pos = { -21.0f, 1.0f, 0.0f };
	building[2].scale = { 1.0f, 1.0f, 20.0f };
	building[2].texture = textures[3];

	//back
	building[3].pos = { 0.0f, 1.0f, 21.0f };
	building[3].scale = { 20.0f, 1.0f, 1.0f };
	building[3].texture = textures[3];

	//front right
	building[4].pos = { 40.0f, 3.0f, -10.0f };
	building[4].scale = { 20.0f, 3.0f, 10.0f };
	building[4].texture = textures[2];

	//back right
	building[5].pos = { 40.0f, 3.0f, 30.0f };
	building[5].scale = { 20.0f, 3.0f, 10.0f };
	building[5].texture = textures[2];

	//right end wall
	building[6].pos = { 60.0f, 5.0f, 10.0f };
	building[6].scale = { 1.0f, 5.0f, 10.0f };
	building[6].texture = textures[2];

	box.pos = { 2.0f,1.0f,2.0f };
	box.scale = {1.0f, 1.0f, 1.0f};
	box.texture = textures[4];


	newSkybox = Skybox("cube.obj");
	newSkybox.setTexture(Skybox::Side::FRONT, "Town-skybox/Town_ft.bmp");
	newSkybox.setTexture(Skybox::Side::BACK, "Town-skybox/Town_bk.bmp");
	newSkybox.setTexture(Skybox::Side::LEFT, "Town-skybox/Town_lf.bmp");
	newSkybox.setTexture(Skybox::Side::RIGHT, "Town-skybox/Town_rt.bmp");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// set up TrueType / SDL_ttf
	if (TTF_Init() == -1)
		cout << "TTF failed to initialise." << endl;

	textFont = TTF_OpenFont("MavenPro-Regular.ttf", 48);
	if (textFont == NULL)
		cout << "Failed to open font." << endl;

	labels[0] = 0;

	if (!BASS_Init(-1, 44100, 0, 0, NULL))
		cout << "Can't initialize device";

	sample = loadSample("The Forgotten Age.mp3");

	channel = BASS_SampleGetChannel(sample, FALSE);
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_FREQ, 0);
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, 0.5);
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_PAN, -1);
	BASS_ChannelPlay(channel, FALSE);
}


glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::sin(angle*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(angle*DEG_TO_RADIAN));
}

glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
	return glm::vec3(pos.x + d*std::cos(angle*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(angle*DEG_TO_RADIAN));
}

void update(void) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	oldPlayerPos = { playerPos.x,playerPos.y ,playerPos.z };
	if (keys[SDL_SCANCODE_W]) {
		playerPos = moveForward(playerPos, rot, 0.3f);
		currentAnim = 1;
	}
	else
		currentAnim = 0;

	if (keys[SDL_SCANCODE_S]) playerPos = moveForward(playerPos, rot, -0.3f);
	if (keys[SDL_SCANCODE_A]) playerPos = moveRight(playerPos, rot, -0.3f);
	if (keys[SDL_SCANCODE_D]) playerPos = moveRight(playerPos, rot, 0.3f);

	if (keys[SDL_SCANCODE_R]) playerPos.y += 0.1;
	if (keys[SDL_SCANCODE_F]) playerPos.y -= 0.1;

	if (keys[SDL_SCANCODE_COMMA]) rot -= 1.0f;
	if (keys[SDL_SCANCODE_PERIOD]) rot += 1.0f;

	if (keys[SDL_SCANCODE_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	if (keys[SDL_SCANCODE_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}
	if (keys[SDL_SCANCODE_Z]) {
		if (--currentAnim < 0) currentAnim = 19;
		cout << "Current animation: " << currentAnim << endl;
	}
	if (keys[SDL_SCANCODE_X]) {
		if (++currentAnim >= 20) currentAnim = 0;
		cout << "Current animation: " << currentAnim << endl;
	}
}

void moveCollectable(glm::vec3 lastPos) {
	if (lastPos == glm::vec3 {2.0f, 1.0f, 2.0f }) {
		box.pos = { 20.0f, 1.0f, 10.0f };
	}
	else
		if (lastPos == glm::vec3{ 20.0f, 1.0f, 10.0f }) {
			box.pos = { 40.0f, 1.0f, 10.0f };
		}
		else
			if (lastPos == glm::vec3{ 40.0f, 1.0f, 10.0f }) {
				box.pos = { 2.0f, 1.0f, 2.0f };
			}
}

//AABB Collision
void checkCollision(glm::vec3 boxA, glm::vec3 scaleA, glm::vec3 boxB, glm::vec3 scaleB, bool collectable) {
	glm::vec3 tmp(boxA.x, boxA.y - 1.0f, boxA.z);
	//check the X axis
	if (abs(tmp.x - boxB.x) < scaleA.x + scaleB.x) {
		//check the Y axis
		if (abs(tmp.y - boxB.y) < scaleA.y + scaleB.y) {
			//check the Z axis
			if (abs(tmp.z - boxB.z) < scaleA.z + scaleB.z) {
				if (collectable) {
					score++;
					moveCollectable(box.pos);
				}
				playerPos = oldPlayerPos;
			}
		}
	}
}

// textToTexture
GLuint textToTexture(const char * str, GLuint textID) {
	GLuint texture = textID;
	TTF_Font * font = textFont;

	SDL_Surface * stringImage = TTF_RenderText_Blended(font, str, { 255, 255, 255 });

	if (stringImage == NULL) {
		std::cout << "String surface not created." << std::endl;
	}

	if (texture == 0) {
		glGenTextures(1, &texture);
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stringImage->w, stringImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, stringImage->pixels);
	glBindTexture(GL_TEXTURE_2D, NULL);

	SDL_FreeSurface(stringImage);
	return texture;
}

void clearTextTexture(GLuint textID) {
	if (textID != NULL) {
		glDeleteTextures(1, &textID);
	}
}


void draw(SDL_Window * window) {

	// clear the screen
	glEnable(GL_CULL_FACE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection(1.0);
	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 150.0f);
	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));

	GLfloat scale(1.0f); // just to allow easy scaling of complete scene

	glm::mat4 modelview(1.0); // set base position for scene
	mvStack.push(modelview);

	

	eye = playerPos;
	at = playerPos;
	eye = moveForward(at, rot, -6.0f);
	eye.y = at.y + 3.0f;
	mvStack.top() = glm::lookAt(eye, at, up);

	newSkybox.draw(skyboxProgram, projection, mvStack);

	glUseProgram(shaderProgram);

	glm::vec4 tmp = mvStack.top()*lightPos;
	light0.position[0] = tmp.x;
	light0.position[1] = tmp.y;
	light0.position[2] = tmp.z;
	rt3d::setLightPos(shaderProgram, glm::value_ptr(tmp));


	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));

	for (int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, ground[i].texture);
		rt3d::materialStruct tmpMaterial = ground[i].material;
		rt3d::setMaterial(shaderProgram, ground[i].material);

		mvStack.push(mvStack.top());
		mvStack.top() = glm::translate(mvStack.top(), ground[i].pos);
		mvStack.top() = glm::rotate(mvStack.top(), float(ground[i].rot.x * DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
		mvStack.top() = glm::rotate(mvStack.top(), -float((ground[i].rot.y)*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		mvStack.top() = glm::rotate(mvStack.top(), -float((ground[i].rot.z)*DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));

		mvStack.top() = glm::scale(mvStack.top(), ground[i].scale);

		rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
		rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
		mvStack.pop();

	}

	for (int i = 0; i < 10; i++) {
		glBindTexture(GL_TEXTURE_2D, building[i].texture);
		rt3d::materialStruct tmpMaterial = building[i].material;
		rt3d::setMaterial(shaderProgram, building[i].material);

		mvStack.push(mvStack.top());
		mvStack.top() = glm::translate(mvStack.top(), building[i].pos);
		mvStack.top() = glm::rotate(mvStack.top(), float(building[i].rot.x * DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
		mvStack.top() = glm::rotate(mvStack.top(), -float((building[i].rot.y)*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		mvStack.top() = glm::rotate(mvStack.top(), -float((building[i].rot.z)*DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));

		mvStack.top() = glm::scale(mvStack.top(), building[i].scale);

		rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
		rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
		mvStack.pop();

		checkCollision(playerPos, playerScale, building[i].pos, building[i].scale, false);
	}

	glBindTexture(GL_TEXTURE_2D, box.texture);
	rt3d::materialStruct tmpMaterial = box.material;
	rt3d::setMaterial(shaderProgram, box.material);

	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), box.pos);
	mvStack.top() = glm::rotate(mvStack.top(), float((box.rot.x)* DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), -float((box.rot.y += 0.5f)*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), -float((box.rot.z)*DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));

	mvStack.top() = glm::scale(mvStack.top(), box.scale);

	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
	checkCollision(playerPos, playerScale, box.pos, box.scale, true);

	// Animate the md2 model, and update the mesh with new vertex data
	tmpModel.Animate(currentAnim, 0.1);
	rt3d::updateMesh(meshObjects[1], RT3D_VERTEX, tmpModel.getAnimVerts(), tmpModel.getVertDataSize());

	// draw the hobgoblin
	glCullFace(GL_FRONT); // md2 faces are defined clockwise, so cull front face
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	tmpMaterial = material1;
	rt3d::setMaterial(shaderProgram, tmpMaterial);

	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), playerPos);
	mvStack.top() = glm::rotate(mvStack.top(), float(90.0f*DEG_TO_RADIAN), glm::vec3(-1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), -float((rot - 90)*DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(scale*0.05, scale*0.05, scale*0.05));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawMesh(meshObjects[1], md2VertCount, GL_TRIANGLES);

	mvStack.pop();
	glCullFace(GL_BACK);





	////////////////////////////////////////////////////////////////////
	//This renders a 3D label
	////////////////////////////////////////////////////////////////////

	glUseProgram(skyboxProgram);//Use texture-only shader for text rendering
	labels[0] = textToTexture(" 3D label ", labels[0]);
	glBindTexture(GL_TEXTURE_2D, labels[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-1.0f, 2.0f, -7.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.0f, 1.0f, 0.0f));
	rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();



	////////////////////////////////////////////////////////////////////
	//This renders a HUD label
	////////////////////////////////////////////////////////////////////

	glUseProgram(skyboxProgram);//Use texture-only shader for text rendering
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label

	for (int i = 0; i < score; i++) {
		//labels[0] = textToTexture(" HUD label ", labels[0]);
		glBindTexture(GL_TEXTURE_2D, textures[4]);
		mvStack.push(glm::mat4(1.0));
		mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-0.9f +(i*0.1f), -0.9f, 0.0f));
		mvStack.top() = glm::scale(mvStack.top(), glm::vec3(0.05f, 0.05f, 0.0f));
		rt3d::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
		rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));

		rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
		mvStack.pop();
	}
	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label 

	mvStack.pop(); // initial matrix
	glDepthMask(GL_TRUE);

	SDL_GL_SwapWindow(window); // swap buffers
}



// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[]) {
	SDL_Window * hWindow; // window handle
	SDL_GLContext glContext; // OpenGL context handle
	hWindow = setupRC(glContext); // Create window and render context 

								  // Required on Windows *only* init GLEW to access OpenGL beyond 1.1
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << endl;
		exit(1);
	}
	cout << glGetString(GL_VERSION) << endl;

	init();

	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running) {	// the event loop
		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT)
				running = false;
		}
		update();
		draw(hWindow); // call the draw function
	}
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(hWindow);
	SDL_Quit();
	return 0;
}
