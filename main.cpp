#include <GL/glew.h>
#include <GL/freeglut.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <map>
#include <set>

#define GLM_FORCE_RADIANS 
#define TWOPI 2*3.1415926535897932384626433832795

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "objloader.h"
#include "shader.h"
#include "skybox.h"
#include "Cube.h"

using namespace glm;
using namespace std;

mat4 view;
mat4 model;
mat4 projection;
mat4 model_view;


std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals; // Won't be used at the moment.
std::vector<glm::vec2> uvs;
vector<int> indices;
GLuint bunnyTexID;
vector<vec2> textures;
//vector<int> indices;
int numverts;
vector<vec3> tempVertices;
vector<vec3> tempNormals;
GLuint vao;
unsigned int vbo[3];
unsigned int texID;
GLuint ebo;
mat4 translateMatrix;
SkyBox* sky;

GLfloat timeUpdate = 0.0;
vec3 cameraPos;
bool reflective = false;
float reflectFactor = 1.0f;
Cube* cube;

GLuint projection_matrix_loc;
GLuint model_view_matrix_loc;
GLuint program;
GLuint programOne;
float aspect = 0.0;
GLfloat angle = 0.0;
int viewport[4];
// Properties
GLuint screenWidth = 512, screenHeight = 512;

ILboolean success = true;
bool mipmap = true;
unsigned int imageID, imageID1;

// Set the object data (buffers, vertex attributes)
GLfloat cubeVertices[] = {
	// Positions          // Texture Coords
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};
GLfloat floorVertices[] = {
	// Positions          // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
	5.0f, -0.5f, 5.0f,   2.0f, 0.0f,
	-5.0f, -0.5f, 5.0f,  0.0f, 0.0f,
	-5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

	5.0f, -0.5f, 5.0f,   2.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
	5.0f, -0.5f, -5.0f,    2.0f, 2.0f
};
GLfloat quadVertices[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// Positions   // TexCoords
	-1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,

	-1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f
};

// Setup cube VAO
GLuint cubeVAO, cubeVBO;
// Setup plane VAO
GLuint floorVAO, floorVBO;
// Setup screen VAO
GLuint quadVAO, quadVBO;
// Framebuffers
GLuint framebuffer;
GLuint rbo;
GLuint cubeTexture, floorTexture;
GLuint textureColorbuffer;

static const double kPI = 3.1415926535897932384626433832795;
static int gViewportWidth, gViewportHeight;
static double gCameraScale = 1.0;
static double gCameraTranslationX = 0;
static double gCameraTranslationY = 0;
static bool gIsRotatingCamera = false;
static bool gIsScalingCamera = false;
static bool gIsTranslatingCamera = false;
static int gLastMouseX, gLastMouseY;
mat4 gCameraRotation;



void Initialize();
void display(void);
void makeIdentity();
void Reshape(int width, int height);
void glutMouse(int button, int state, int x, int y);
void glutMotion(int x, int y);
double projectToTrackball(double radius, double x, double y);
unsigned int loadTexture(std::string filename);
void setMatrices();
unsigned int loadCubeMapTexture(std::string posX, std::string negX, std::string posY, std::string negY, std::string posZ, std::string negZ);




// Generates a texture that is suited for attachments to a framebuffer
GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil)
{
	// What enum to use?
	GLenum attachment_type;
	if (!depth && !stencil)
		attachment_type = GL_RGB;
	else if (depth && !stencil)
		attachment_type = GL_DEPTH_COMPONENT;
	else if (!depth && stencil)
		attachment_type = GL_STENCIL_INDEX;

	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (!depth && !stencil)
		glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, screenWidth, screenHeight, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
	else // Using both a stencil and depth test, needs special format arguments
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screenWidth, screenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

unsigned int loadTexture(std::string filename) {

	ILboolean success;
	unsigned int imageID;

	// Load Texture Map
	ilGenImages(1, &imageID);

	ilBindImage(imageID); /* Binding of DevIL image name */
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	success = ilLoadImage((ILstring)filename.c_str());

	if (!success) {
		std::cout << "Couldn't load the following texture file: " << filename.c_str() << std::endl;
		// The operation was not sucessfull hence free image and texture 
		ilDeleteImages(1, &imageID);
		return 0;
	}

	// add information to the log
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	GLuint tid;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_REPEAT);	// Use GL_MIRRORED_REPEAT to prevent white borders. Due to interpolation it takes value from next repeat 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	

	/* Because we have already copied image data into texture data
	we can release memory used by image. */
	ilDeleteImages(1, &imageID);
	return tid;
}


void setMatrices()
{
	mat4 mv = view * model;
	mat4 mvp = projection*mv;
	glUniformMatrix4fv(glGetUniformLocation(program, "ModelMatrix"), 1, GL_FALSE, (GLfloat*)&mv[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "MVP"), 1, GL_FALSE, (GLfloat*)&mvp[0]);
}

unsigned int loadCubeMapTexture(std::string posX, std::string negX, std::string posY, std::string negY, std::string posZ, std::string negZ) {

	ILboolean success;
	unsigned int imageID;
	GLuint textureID;
	GLuint faceTarget[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	std::string files[6];

	files[0] = posX;
	files[1] = negX;
	files[2] = posY;
	files[3] = negY;
	files[4] = posZ;
	files[5] = negZ;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID); /* Texture name generation */
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	// Load Textures for Cube Map

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	ilGenImages(1, &imageID);
	ilBindImage(imageID); /* Binding of DevIL image name */

	for (int i = 0; i < 6; ++i) {
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		success = ilLoadImage((ILstring)files[i].c_str());

		if (!success) {
			std::cout << "Couldn't load texture file." << std::endl;
			// The operation was not sucessfull hence free image and texture 
			ilDeleteImages(1, &imageID);
		}

		/* Convert image to RGBA */
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		/* Create and load textures to OpenGL */
		glTexImage2D(faceTarget[i], 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

		std::cout << "Texture Loaded: " << files[i].c_str() << std::endl;
	}
	std::cout << "Cube Map Loaded Successfully." << std::endl;


	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	/* Because we have already copied image data into texture data we can release memory used by image. */
	ilDeleteImages(1, &imageID);

	// add information to the log

	return textureID;
}

void Initialize(void){
	
	// Create and compile our GLSL program from the shaders
	objloader loader;

	bool loadfile = loader.load("bunnyTex.obj");
	vertices = loader.getVertices();
	normals = loader.getNormals();
	indices = loader.getVertexIndices();
	textures = loader.getTexCoordinates();
	numverts = normals.size();


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint offset = 0;
	glGenBuffers(1, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) + normals.size() * sizeof(glm::vec3) + textures.size() * sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, vertices.size() * sizeof(glm::vec3), &vertices[0]);//for vertices
	offset += vertices.size() * sizeof(glm::vec3);
	glBufferSubData(GL_ARRAY_BUFFER, offset, normals.size() * sizeof(glm::vec3), &normals[0]);//for normals
	offset += normals.size() * sizeof(glm::vec3);
	glBufferSubData(GL_ARRAY_BUFFER, offset, textures.size() * sizeof(glm::vec2), &textures[0]);//for textures

	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(vertices.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);  // Vertex normal

	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(vertices.size() * sizeof(glm::vec3) + normals.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);  // Vertex texture

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);


	cube = new Cube();//load bunny obj
	sky = new SkyBox();

	program = LoadShaders("cubemap_reflect.vs", "cubemap_reflect.fs");
	programOne = LoadShaders("texture.vs", "texture.fs");

	// Use our shader
	glUseProgram(program);
	loadCubeMapTexture("images/pos_x.tga", "images/neg_x.tga", "images/pos_y.tga", "images/neg_y.tga", "images/pos_z.tga", "images/neg_z.tga");
	glUniform1i(glGetUniformLocation(program, "CubeMapTex"), 0);

	cameraPos = vec3(2.0f * cos(angle), 1.5f, 1.0f * sin(angle));

	glClearColor(1.0, 1.0, 1.0, 1.0);


	
	//programOne = LoadShaders("framebuffer.vs", "framebuffer.frag");
	// Load textures
	cubeTexture = loadTexture("Marble.TGA");
	floorTexture = loadTexture("stone.TGA");
	// Use our shader
	//glUseProgram(program);
	view = glm::lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);

	// cube VAO
	
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	// floor VAO
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	// screen VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);

	
	// Framebuffers
	//glUseProgram(programOne);
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Create a color attachment texture
	textureColorbuffer = generateAttachmentTexture(false, false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight); // Use a single renderbuffer object for both a depth AND stencil buffer.
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // Now actually attach it
	// Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	
}
void Display(void)
{
	/////////////////////////////////////////////////////
	// Bind to framebuffer and draw to color texture 
	// as we normally would.
	// //////////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, gViewportWidth, gViewportHeight);
	// Clear all attached buffers        
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer so why bother with clearing?

	
	// Setup some OpenGL options
	//glDepthFunc(GL_LESS);
    //glUseProgram(program);

	glUseProgram(program);
	view = glm::lookAt(vec3(2.0f * cos(angle), 0.5f, 1.0f * sin(angle)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = glm::perspective(radians(80.0f), aspect, 0.3f, 100.0f);
	glUniform3fv(glGetUniformLocation(program, "WorldCameraPosition"), 1, (GLfloat*)&cameraPos[0]);
	model = mat4(1.0f);
	glUniform1i(glGetUniformLocation(program, "DrawSkyBox"), true);
	setMatrices();
	sky->render();

	glUniform1i(glGetUniformLocation(program, "DrawSkyBox"), reflective);
	vec4 material_color(1.0f, 0.5f, 0.5f, 1.0f);
	glUniform4fv(glGetUniformLocation(program, "MaterialColor"), 1, (GLfloat*)&material_color[0]);
	glUniform1f(glGetUniformLocation(program, "ReflectFactor"), reflectFactor);

	model = mat4(1.0f);

	mat4 scaled1 = glm::scale(glm::mat4(1.0f), glm::vec3(gCameraScale, gCameraScale, gCameraScale));
	mat4 translated1 = glm::translate(glm::mat4(1.0f), glm::vec3(gCameraTranslationX, gCameraTranslationY, 0.6));
	mat4 transformation_matrix = translated1*gCameraRotation*scaled1;
	model *= transformation_matrix;

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (GLfloat*)&view[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (GLfloat*)&projection[0]);


	setMatrices();
	//cube->render();//render bunny?
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
	// Choose whether to draw in wireframe mode or not
	model = mat4(1.0f);
	
	projection = glm::perspective(5.0f, aspect, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (GLfloat*)&view[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (GLfloat*)&projection[0]);

	// Floor
	/*
	glBindVertexArray(floorVAO);
	
	model = glm::mat4();
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);*/

	// Cubes
	/*
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.5f, 0.0f, -2.0f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0); */

	/////////////////////////////////////////////////////
	// Bind to default framebuffer again and draw the 
	// quad plane with attched screen texture.
	// //////////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, gViewportWidth, gViewportHeight);
	// Clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	// Draw Screen
	glUseProgram(programOne);
	
	// Choose whether to draw in wireframe mode or not
	model = mat4(1.0f);
	mat4 scaled = glm::scale(glm::mat4(1.0f), glm::vec3(gCameraScale, gCameraScale, gCameraScale));
	mat4 translated = glm::translate(glm::mat4(1.0f), glm::vec3(gCameraTranslationX, gCameraTranslationY, 0.0));
	 transformation_matrix = translated*gCameraRotation*scaled;
	model *= transformation_matrix;
	model_view = view*model;

	projection = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(programOne, "view"), 1, GL_FALSE, (GLfloat*)&view[0]);
	glUniformMatrix4fv(glGetUniformLocation(programOne, "projection"), 1, GL_FALSE, (GLfloat*)&projection[0]);
	
	glUniformMatrix4fv(glGetUniformLocation(programOne, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// Use the color attachment texture as the texture of the quad plane
	
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	
	glutSwapBuffers();
}

void Reshape(int width, int height)
{
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);

	gViewportWidth = width;
	gViewportHeight = height;
	glViewport(0, 0, width, height);

	//projection = glm::perspective(50.0f, (float)width / height, 0.3f, 1000.0f);
	aspect = float(width) / float(height);
}

void update()
{
	angle += 0.01f;
	if (angle > TWOPI) angle -= TWOPI;
}


void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	

	case 'r':
	case 'R':
		makeIdentity();
		break;
	}

	glutPostRedisplay();
}

/*********************************************************************************************/
void makeIdentity(){

	gCameraScale = 1.0;
	gCameraTranslationX = 0;
	gCameraTranslationY = 0;
	gCameraRotation = mat4(1.0f);


}
/************************************************************************************************/
/*******************************************************************************/
double projectToTrackball(double radius, double x, double y)
{
	static const double kUnitSphereRadius2D = sqrt(2.0);
	double z;

	double dist = sqrt(x * x + y * y);
	if (dist < radius * kUnitSphereRadius2D / 2.0)
	{
		// Solve for sphere case.
		z = sqrt(radius * radius - dist * dist);
	}
	else
	{
		// Solve for hyperbolic sheet case.
		double t = radius / kUnitSphereRadius2D;
		z = t * t / dist;
	}

	return z;
}

/*******************************************************************************/
/******************************************************************************
GLUT callback for responding to mouse button presses. Detecst whether to
initiate a point snapping, view rotation or view scale.
******************************************************************************/
void glutMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			gIsRotatingCamera = true;
		}
		else if (button == GLUT_MIDDLE_BUTTON)
		{
			gIsScalingCamera = true;
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			gIsTranslatingCamera = true;
		}

		gLastMouseX = x;
		gLastMouseY = y;
	}
	else
	{
		gIsRotatingCamera = false;
		gIsScalingCamera = false;
		gIsTranslatingCamera = false;
	}
}
/**************************************************************************/
/*******************************************************************************
*******************************************************************************/

void glutMotion(int x, int y)
{
	if (gIsRotatingCamera)
	{
		static const double kTrackBallRadius = 0.8;
		vec3 lastPos;
		vec3 currPos;

		lastPos.x = gLastMouseX * 2.0 / gViewportWidth - 1.0;
		lastPos.y = (gViewportHeight - gLastMouseY) * 2.0 / gViewportHeight - 1.0;
		lastPos.z = projectToTrackball(kTrackBallRadius, lastPos[0], lastPos[1]);

		currPos.x = x * 2.0 / gViewportWidth - 1.0;
		currPos.y = (gViewportHeight - y) * 2.0 / gViewportHeight - 1.0;
		currPos.z = projectToTrackball(kTrackBallRadius, currPos[0], currPos[1]);
		
		currPos = normalize(currPos);
		lastPos = normalize(lastPos);

		vec3 rotateVecCross;

		rotateVecCross = cross(vec3(lastPos), vec3(currPos)) ;
		
		double sin_rotate_angle = sqrt(rotateVecCross.x * rotateVecCross.x + rotateVecCross.y * rotateVecCross.y + rotateVecCross.z * rotateVecCross.z);
		double angle = asin(sqrt(rotateVecCross.x * rotateVecCross.x + rotateVecCross.y * rotateVecCross.y + rotateVecCross.z * rotateVecCross.z));
		double cos_rotate_angle = cos(angle);
		
		rotateVecCross = normalize(rotateVecCross);

		mat4 deltaRotation(1.0f);

		deltaRotation[0][0] = cos_rotate_angle + rotateVecCross[0] * rotateVecCross[0] * (1 - cos_rotate_angle);
		deltaRotation[0][1] = rotateVecCross[0] * rotateVecCross[1] * (1 - cos_rotate_angle) + rotateVecCross[2] * sin_rotate_angle;
		deltaRotation[0][2] = rotateVecCross[0] * rotateVecCross[2] * (1 - cos_rotate_angle) - rotateVecCross[1] * sin_rotate_angle;
		deltaRotation[0][3] = 0.0;
		deltaRotation[1][0] = rotateVecCross[0] * rotateVecCross[1] * (1 - cos_rotate_angle) - rotateVecCross[2] * sin_rotate_angle;
		deltaRotation[1][1] = cos_rotate_angle + rotateVecCross[1] * rotateVecCross[1] * (1 - cos_rotate_angle);
		deltaRotation[1][2] = rotateVecCross[2] * rotateVecCross[1] * (1 - cos_rotate_angle) + rotateVecCross[0] * sin_rotate_angle;
		deltaRotation[1][3] = 0.0;
		deltaRotation[2][0] = rotateVecCross[0] * rotateVecCross[2] * (1 - cos_rotate_angle) + rotateVecCross[1] * sin_rotate_angle;
		deltaRotation[2][1] = rotateVecCross[1] * rotateVecCross[2] * (1 - cos_rotate_angle) - rotateVecCross[0] * sin_rotate_angle;
		deltaRotation[2][2] = cos_rotate_angle + rotateVecCross[2] * rotateVecCross[2] * (1 - cos_rotate_angle);
		

		mat4 tempMatrix;

		
		if (abs(angle) > DBL_EPSILON*20.0){
			tempMatrix = gCameraRotation*deltaRotation;
			gCameraRotation = tempMatrix;
		}
	}
	if (gIsTranslatingCamera)
	{
		gCameraTranslationX += 2 * double(x - gLastMouseX) / gViewportWidth;
		gCameraTranslationY -= 2 * double(y - gLastMouseY) / gViewportHeight;

	}
	else if (gIsScalingCamera)
	{
		float y1 = gViewportHeight - gLastMouseY;
		float y2 = gViewportHeight - y;

		gCameraScale *= 1 + (y1 - y2) / gViewportHeight;
	}

	glutPostRedisplay();

	gLastMouseX = x;
	gLastMouseY = y;
}
/************************************************************************************************************************************************************/

/*******************************************************************************
*******************************************************************************/

/*********/
int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(800, 600);

	glutCreateWindow("FBO Example");

	if (glewInit()){
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
	}
	ilInit();
	Initialize();
	std::cout << glGetString(GL_VERSION) << std::endl;
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(glutMotion);
	glutMouseFunc(glutMouse);
	glutReshapeFunc(Reshape);
	glutMainLoop();
	return 0;
}


