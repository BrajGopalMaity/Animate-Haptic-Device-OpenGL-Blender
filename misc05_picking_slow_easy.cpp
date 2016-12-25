// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <stack>   
#include <sstream>
#include <conio.h>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;
// Include AntTweakBar
#include <AntTweakBar.h>

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#define PI 3.14159265
const int window_width = 1024, window_height = 768;

typedef struct Vertex {
	float Position[4];
	float Color[4];
	float Normal[3];
	void SetPosition(float *coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = 1.0;
	}
	void SetColor(float *color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
	void SetNormal(float *coords) {
		Normal[0] = coords[0];
		Normal[1] = coords[1];
		Normal[2] = coords[2];
	}
};

// function prototypes
int initWindow(void);
void initOpenGL(void);
void loadObject(char*, glm::vec4, Vertex * &, GLushort* &, int);
void createVAOs(Vertex[], GLushort[], int);
void createObjects(void);
void pickObject(void);
void renderScene(void);
void cleanup(void);
static void keyCallback(GLFWwindow*, int, int, int, int);
static void mouseCallback(GLFWwindow*, int, int, int);
void rotateCam(void);
GLfloat xmove = 0.0f, zmove = 0.0f, an = 0.0f, ar=0.0f,a2=0.0f,le=0.0f,ra=0.0f,ro=0.0f,jx=0.0f,jz=0.0f,jy=0.0f,tm = 0.01f,yen=0.0f,vel=0.0f;
GLfloat xx = 0.0f, yy = 0.0f, zz = 0.0f;
int c = 1;
GLfloat aa=0.0f, bb=0.0f, cc=0.0f;
glm::vec3 vect;
// GLOBAL VARIABLES
GLFWwindow* window;
Vertex* Verts1,*Verts2,*Verts3, *Verts4, *Verts5, *Verts6, *Verts7,*Verts8,*Verts9;
GLushort* Idcs1,*Idcs2,*Idcs3, *Idcs4, *Idcs5, *Idcs6, *Idcs7, *Idcs8;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;
float px = 0.785f;
float py = 0.785f;
float x , y , z,lx1,ly1,lz1,lx2,ly2,lz2,theta,beta  ;
glm::vec3 camMatrix;
glm::vec3 lightPos;
glm::vec3 light2;
glm::vec4 tini, uni;
glm::mat4 MM;
glm::vec3 upMatrix = glm::vec3(0.0, 1.0, 0.0);
GLuint m = 0,base=0,top=0,arm1=0,arm2=0,pen=0,xwa=0,ywa=0,zwa=0,anime=0,fl=0,slingshot=0;

GLuint gPickedIndex = -1;
std::string gMessage;

GLuint programID;
GLuint pickingProgramID;

const GLuint NumObjects = 10;	// ATTN: THIS NEEDS TO CHANGE AS YOU ADD NEW OBJECTS
GLuint VertexArrayId[NumObjects] = { 0,1,2,3,4,5,6,7,8,9 };
GLuint VertexBufferId[NumObjects] = { 0,1,2,3,4,5,6,7,8,9 };
GLuint IndexBufferId[NumObjects] = { 0,1,2,3,4,5,6,7,8,9 };

size_t NumIndices[NumObjects] = { 0,1,2,3,4,5,6,7,8,9 };
size_t VertexBufferSize[NumObjects] = { 0,1,2,3,4,5,6,7,8,9 };
size_t IndexBufferSize[NumObjects] = { 0,1,2,3,4,5,6,7,8,9 };
size_t NumVertices[NumObjects] = { 0,1,2,3,4,5,6,7,8,9 };

GLuint MatrixID;
GLuint ModelMatrixID;
GLuint ViewMatrixID;
GLuint ProjMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorID;
GLuint LightID;
GLuint Light2ID;
GLuint ambientID;
GLuint tejaID;
GLuint yaya = 1;
GLfloat fsin = 0.0f, flt = 0.0f, fcos = 0.0f, strt = 0.0f, tco = 0.0f;
glm::vec3 tvect;
GLint gX = 0.0;
GLint gZ = 0.0;

// animation control
bool animation = false;
GLfloat phi = 0.0;

void loadObject(char* file, glm::vec4 color, Vertex * &out_Vertices, GLushort* &out_Indices, int ObjectId)
{
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ(file, vertices, normals);

	std::vector<GLushort> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, normals, indices, indexed_vertices, indexed_normals);

	const size_t vertCount = indexed_vertices.size();
	const size_t idxCount = indices.size();

	// populate output arrays
	out_Vertices = new Vertex[vertCount];
	for (int i = 0; i < vertCount; i++) {
		out_Vertices[i].SetPosition(&indexed_vertices[i].x);
		out_Vertices[i].SetNormal(&indexed_normals[i].x);
		out_Vertices[i].SetColor(&color[0]);
	}
	out_Indices = new GLushort[idxCount];
	for (int i = 0; i < idxCount; i++) {
		out_Indices[i] = indices[i];
	}
	// set global variables!!
	NumIndices[ObjectId] = idxCount;
	VertexBufferSize[ObjectId] = sizeof(out_Vertices[0]) * vertCount;
	IndexBufferSize[ObjectId] = sizeof(GLushort) * idxCount;
	NumVertices[ObjectId] = vertCount;
}


void createObjects(void)
{
	//-- COORDINATE AXES --//
	Vertex CoordVerts[] =
	{
		{ { 0.0, 0.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 5.0, 0.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 5.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 5.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } },
	};

	VertexBufferSize[0] = sizeof(CoordVerts);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	createVAOs(CoordVerts, NULL, 0);
	
	//-- GRID --//
	float coords1[3], coords2[3];
	float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float norm[] =  {0.0f, 0.0f, 0.0f};
	Vertex Grid[440];
	int i=0;
	for (float y=0.0f; y<11.0f; y++)
		for (float x = 0.0f; x < 10.0f; x++) {
			coords1[0] = -5.0f + x;
			coords1[1] = 0.0f;
			coords1[2] = -5.0f + y;
			coords2[0] = -5.0f + x + 1.0f;
			coords2[1] = 0.0f;
			coords2[2] = -5.0f + y;
			Grid[i].SetPosition(coords1);
			Grid[i].SetColor(color);
			Grid[i].SetNormal(norm);
			i++;
			Grid[i].SetPosition(coords2);
			Grid[i].SetColor(color);
			Grid[i].SetNormal(norm);
			i++;
		}
	for (float y = 0.0f; y<11.0f; y++)
		for (float x = 0.0f; x < 10.0f; x++) {
			coords1[0] = -5.0f + y;
			coords1[1] = 0.0f;
			coords1[2] = -5.0f + x;
			coords2[0] = -5.0f + y;
			coords2[1] = 0.0f;
			coords2[2] = -5.0f + x + 1.0f;
			Grid[i].SetPosition(coords1);
			Grid[i].SetColor(color);
			Grid[i].SetNormal(norm);
			i++;
			Grid[i].SetPosition(coords2);
			Grid[i].SetColor(color);
			Grid[i].SetNormal(norm);
			i++;
		}
	VertexBufferSize[1] = sizeof(Grid);
	createVAOs(Grid, NULL, 1);
	// ATTN: create your grid vertices here!
	
	//-- .OBJs --//

	// ATTN: load your models here
	//Vertex* Verts;
	
	//GLushort* Idcs;
	
	//loadObject("models/base.obj", glm::vec4(1.0, 0.0, 0.0, 1.0), Verts, Idcs, ObjectID);
	loadObject("base.obj", glm::vec4(0.8, 0.0, 0.0, 1.0), Verts1, Idcs1, 2);
	loadObject("cylinder.obj", glm::vec4(0.0, 0.8, 0.0, 1.0), Verts2, Idcs2, 3);
	loadObject("arm1.obj", glm::vec4(0.0, 0.0, 0.8, 1.0), Verts3, Idcs3, 4);
	loadObject("joint.obj", glm::vec4(1.0, 0.0, 1.0, 1.0), Verts4, Idcs4, 5);
	loadObject("arm2.obj", glm::vec4(0.0, 0.8, 0.8, 1.0), Verts5, Idcs5, 6);
	loadObject("pen.obj", glm::vec4(0.8, 0.8, 0.0, 1.0), Verts6, Idcs6, 7);
	loadObject("button.obj", glm::vec4(0.8, 0.0, 0.0, 1.0), Verts7, Idcs7, 8);
	loadObject("bullet.obj", glm::vec4(0.0, 0.0, 0.0, 1.0), Verts8, Idcs8, 9);
	//createVAOs(Verts, Idcs, ObjectID);
	//VertexBufferSize[2] = sizeof(&Verts1);
	createVAOs(Verts1, Idcs1, 2);
	createVAOs(Verts2, Idcs2, 3);
	createVAOs(Verts3, Idcs3, 4);
	createVAOs(Verts4, Idcs4, 5);
	createVAOs(Verts5, Idcs5, 6);
	createVAOs(Verts6, Idcs6, 7);
	createVAOs(Verts7, Idcs7, 8);
	createVAOs(Verts8, Idcs8, 9);
}

void animate() {
	
	if (anime == 1) {
		if (yaya == 1) {
			aa = uni[0] - tini[0];
			bb = uni[1] - tini[1];
			cc = uni[2] - tini[2];
			vect = glm::normalize(vec3(aa, bb, cc));
			tvect = glm::normalize(vec3(aa, 0, cc));
			yaya = 0;
	 }
		if (uni[0] - tini[0] >= 0){
			fsin = glm::dot(vect, vec3(0, 1, 0));
			flt = glm::dot(tvect, vec3(0, 0, 1));
			fcos = cosf(glm::asin(fsin));
			strt = 2.7f*fcos*tm;
			jz = strt*flt;
			tco = cosf(glm::asin(flt));
			jx = strt*tco;
			jy = (2.7f*fsin*tm) - ((0.5f)*(9.8f)*tm*tm);
			tm += 0.01f;
		}
		else if (uni[0] - tini[0] < 0) {
			fsin = glm::dot(vect, vec3(0, 1, 0));
			flt = glm::dot(tvect, vec3(0, 0, 1));
			fcos = cosf(glm::asin(fsin));
			strt = 2.7f*fcos*tm;
			jz = strt*flt;
			tco = cosf(glm::asin(flt));
			jx = -(strt*tco);
			jy = (2.7f*fsin*tm) - ((0.5f)*(9.8f)*tm*tm);
			tm += 0.01f;
		}
	//	printf(" %f %f %f \n",jx,jy,jz);
	}
}

void rotatePen(void) {
	if (pen == 1) {
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ) {
			le += 0.01f;
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			le -= 0.01f;
		}
		else if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
				ra -= 0.01f;
				}
		else if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
				ra += 0.01f;
			}
		else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ) {
				ro -= 0.01f;
				}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				ro += 0.01f;
			}
	}
}

void rotateArm2(void) {
	if (arm2 == 1) {
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ) {
			a2 += 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ) {
			a2 -= 0.01f;
		}

	}
}

void rotateArm1(void) {
	if (arm1 == 1) {
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ) {
			ar += 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ) {
			ar -= 0.01f;
		}
	}
}
void rotateTop(void) {
	if (top == 1) {
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			an -= 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			an += 0.01f;
		}
	}
}
void rotateBase(void) {
	if (base == 1) {
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && zmove >= -4.0f) {
			zmove -= 0.1f;
			}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && zmove <= 4.0f) {
			zmove += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && xmove >= -4.0f) {
			xmove -= 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && xmove <= 4.0f) {
			xmove += 0.1f;
		}
	}
}
void rotateCam(void) {
	if (m == 1) {
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			py += 0.01f;
			if (py > 6.28f)
				py = 0.00f;
			y = 17.32f*sin(py);
			z = 17.32f*cos(py)*cos(px);
			x = 17.32f*cos(py)*sin(px);
			camMatrix = glm::vec3(x, y, z);
			lz1 = 10.0f*cos(py)*cos(px - 0.5f);
			lz2 = 10.0f*cos(py)*cos(px + 0.5f);
			lx1 = 10.0f*cos(py)*sin(px - 0.5f);
			lx2 = 10.0f*cos(py)*sin(px + 0.5f);
			ly1 = 10.0f*sin(py);
			ly2 = 10.0f*sin(py);
			lightPos = glm::vec3(lx1, ly1, lz1);
			light2 = glm::vec3(lx2, ly2, lz2);
			if (py > 1.57  && py < 4.71)
				upMatrix = glm::vec3(0.0, -1.0, 0.0);
			else
				upMatrix = glm::vec3(0.0, 1.0, 0.0);
			gViewMatrix = glm::lookAt(camMatrix,	// eye
				glm::vec3(0.0, 0.0, 0.0),	// center
				upMatrix);	// up
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			py -= 0.01f;
			if (py < 0.00f)
				py = 6.28f;
			y = 17.32f*sin(py);
			z = 17.32f*cos(py)*cos(px);
			x = 17.32f*cos(py)*sin(px);
			camMatrix = glm::vec3(x, y, z);
			lz1 = 10.0f*cos(py)*cos(px - 0.5f);
			lz2 = 10.0f*cos(py)*cos(px + 0.5f);
			lx1 = 10.0f*cos(py)*sin(px - 0.5f);
			lx2 = 10.0f*cos(py)*sin(px + 0.5f);
			ly1 = 10.0f*sin(py);
			ly2 = 10.0f*sin(py);
			lightPos = glm::vec3(lx1, ly1, lz1);
			light2 = glm::vec3(lx2, ly2, lz2);
			if (py > 1.57  && py < 4.71)
				upMatrix = glm::vec3(0.0, -1.0, 0.0);
			else
				upMatrix = glm::vec3(0.0, 1.0, 0.0);
			gViewMatrix = glm::lookAt(camMatrix,	// eye
				glm::vec3(0.0, 0.0, 0.0),	// center
				upMatrix);	// up
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			px -= 0.01f;
			if (px < 0.00f)
				px = 6.28f;
			x = 17.32f*cos(py)*sin(px);
			z = 17.32f*cos(py)*cos(px);
			camMatrix = glm::vec3(x, y, z);
			lz1 = 10.0f*cos(py)*cos(px - 0.5f);
			lz2 = 10.0f*cos(py)*cos(px + 0.5f);
			lx1 = 10.0f*cos(py)*sin(px - 0.5f);
			lx2 = 10.0f*cos(py)*sin(px + 0.5f);
			ly1 = 10.0f*sin(py);
			ly2 = 10.0f*sin(py);
			lightPos = glm::vec3(lx1, ly1, lz1);
			light2 = glm::vec3(lx2, ly2, lz2);
			if (py > 1.57  && py < 4.71)
				upMatrix = glm::vec3(0.0, -1.0, 0.0);
			else
				upMatrix = glm::vec3(0.0, 1.0, 0.0);
			gViewMatrix = glm::lookAt(camMatrix,	// eye
				glm::vec3(0.0, 0.0, 0.0),	// center
				upMatrix);	// up
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			px += 0.01f;
			if (px > 6.28f)
				px = 0.00f;
			x = 17.32f*cos(py)*sin(px);
			z = 17.32f*cos(py)*cos(px);
			camMatrix = glm::vec3(x, y, z);
			lz1 = 10.0f*cos(py)*cos(px - 0.5f);
			lz2 = 10.0f*cos(py)*cos(px + 0.5f);
			lx1 = 10.0f*cos(py)*sin(px - 0.5f);
			lx2 = 10.0f*cos(py)*sin(px + 0.5f);
			ly1 = 10.0f*sin(py);
			ly2 = 10.0f*sin(py);
			lightPos = glm::vec3(lx1, ly1, lz1);
			light2 = glm::vec3(lx2, ly2, lz2);
			if (py > 1.57  && py < 4.71)
				upMatrix = glm::vec3(0.0, -1.0, 0.0);
			else
				upMatrix = glm::vec3(0.0, 1.0, 0.0);
			gViewMatrix = glm::lookAt(camMatrix,	// eye
				glm::vec3(0.0, 0.0, 0.0),	// center
				upMatrix);	// up
		}
	}
}
void renderScene(void)
{
	//ATTN: DRAW YOUR SCENE HERE. MODIFY/ADAPT WHERE NECESSARY!


	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
	// Re-clear the screen for real rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4x4 ModelMatrix1,ModelMatrix;
	glUseProgram(programID);
		{
			

		ModelMatrix = glm::mat4(1.0);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(Light2ID, light2.x, light2.y, light2.z);
		glUniform3f(ambientID, 1, 1, 1);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);


		glBindVertexArray(VertexArrayId[0]);	// draw CoordAxes
		glDrawArrays(GL_LINES, 0, 6);

		glBindVertexArray(0);

		glBindVertexArray(VertexArrayId[1]);
		glDrawArrays(GL_LINES, 0, 440);
		glBindVertexArray(0);

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(xmove, 0.0f, zmove));
		glUniform3f(ambientID, 0.2f, 0.2f, 0.2f);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glBindVertexArray(VertexArrayId[2]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[2]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts1), Verts1);
		glDrawElements(GL_TRIANGLES, NumIndices[2], GL_UNSIGNED_SHORT, (void *)0);
		glBindVertexArray(0);

		ModelMatrix = glm::rotate(ModelMatrix, an, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3f(ambientID, 0.2f, 0.2f, 0.2f);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glBindVertexArray(VertexArrayId[3]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts2), Verts2);
		glDrawElements(GL_TRIANGLES, NumIndices[3], GL_UNSIGNED_SHORT, (void *)0);
		glBindVertexArray(0);

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 2.11f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, ar, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -2.11f, 0.0f));
		glUniform3f(ambientID, 0.2f, 0.2f, 0.2f);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glBindVertexArray(VertexArrayId[4]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts3), Verts3);
		glDrawElements(GL_TRIANGLES, NumIndices[4], GL_UNSIGNED_SHORT, (void *)0);
		glBindVertexArray(0);

		glBindVertexArray(VertexArrayId[5]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts4), Verts4);
		glDrawElements(GL_TRIANGLES, NumIndices[5], GL_UNSIGNED_SHORT, (void *)0);
		glBindVertexArray(0);

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1.63f, 2.07f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, a2, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-1.63f, -2.07f, 0.0f));
		glUniform3f(ambientID, 0.2f, 0.2f, 0.2f);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glBindVertexArray(VertexArrayId[6]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[6]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts5), Verts5);
		glDrawElements(GL_TRIANGLES, NumIndices[6], GL_UNSIGNED_SHORT, (void *)0);
		glBindVertexArray(0);
		//MM = ModelMatrix;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1.63f, 1.05f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, ro, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, le, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, ra, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-1.63f, -1.05f, 0.0f));
		glUniform3f(ambientID, 0.2f, 0.2f, 0.2f);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glBindVertexArray(VertexArrayId[7]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[7]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts6), Verts6);
		glDrawElements(GL_TRIANGLES, NumIndices[7], GL_UNSIGNED_SHORT, (void *)0);
		glBindVertexArray(0);

		glBindVertexArray(VertexArrayId[8]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[8]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts7), Verts7);
		glDrawElements(GL_TRIANGLES, NumIndices[8], GL_UNSIGNED_SHORT, (void *)0);
		glBindVertexArray(0);
		tini = ModelMatrix*(glm::vec4(1.49f, 1.04f, 0.0f, 1.0f));
		uni = ModelMatrix*(glm::vec4(2.66f, 1.05f, 0.0f, 1.0f));
		if (fl == 1) {
			
			glm::mat4 rota;
			rota = glm::translate(glm::mat4(), glm::vec3(jx, jy, jz));
			ModelMatrix = rota * ModelMatrix;
			glm::vec4 gul = glm::vec4(Verts8[0].Position[0], Verts8[0].Position[1], Verts8[0].Position[2], Verts8[0].Position[3]);
			glm::vec4 vete = ModelMatrix*gul;

			
				if (vete[1] <= 0) {
					anime = 0;
					xmove = vete[0];
					zmove = vete[2];
					if (xmove > 5.0f || xmove < -5.0f) {
						xmove = 0;
						zmove = 0;
					}
					if (zmove > 5.0f || zmove < -5.0f) {
						zmove = 0;
						xmove = 0;
					}
					fl = 0;
				}
			
			glUniform3f(ambientID, 0.2f, 0.2f, 0.2f);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			
			glBindVertexArray(VertexArrayId[9]);
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[9]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Verts8), Verts8);
			glDrawElements(GL_TRIANGLES, NumIndices[9], GL_UNSIGNED_SHORT, (void *)0);
			glBindVertexArray(0);
			}
		}
		glUseProgram(0);
	// Draw GUI
	TwDraw();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void pickObject(void)
{
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		
		// ATTN: DRAW YOUR PICKING SCENE HERE. REMEMBER TO SEND IN A DIFFERENT PICKING COLOR FOR EACH OBJECT BEFOREHAND
		glBindVertexArray(0);

	}
	glUseProgram(0);
	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	// Convert the color back to an integer ID
	gPickedIndex = int(data[0]);
	
	if (gPickedIndex == 255){ // Full white, must be the background !
		gMessage = "background";
	}
	else {
		std::ostringstream oss;
		oss << "point " << gPickedIndex;
		gMessage = oss.str();
	}

	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
}

int initWindow(void)
{
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(window_width, window_height, "Maity,Braja Gopal(81490468)", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window_width, window_height);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

	// Set up inputs
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);

	return 0;
}

void initOpenGL(void)
{

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	gProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	y = 17.32f*sin(py);
	z = 17.32f*cos(py)*cos(px);
	x = 17.32f*cos(py)*sin(px);
	camMatrix = glm::vec3(x, y, z);
	lz1 = 10.0f*cos(py)*cos(px - 0.5f);
	lz2 = 10.0f*cos(py)*cos(px + 0.5f);
	lx1 = 10.0f*cos(py)*sin(px - 0.5f);
	lx2 = 10.0f*cos(py)*sin(px + 0.5f);
	ly1 = 10.0f*sin(py);
	ly2 = 10.0f*sin(py);
	lightPos = glm::vec3(lx1, ly1, lz1);
	light2 = glm::vec3(lx2,ly2,lz2);

	gViewMatrix = glm::lookAt(camMatrix,	// eye
		glm::vec3(0.0, 0.0, 0.0),	// center
		upMatrix);	// up

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("Picking.vertexshader", "Picking.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ProjMatrixID = glGetUniformLocation(programID, "P");
	
	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	Light2ID = glGetUniformLocation(programID, "LightP2");
	ambientID = glGetUniformLocation(programID, "lit");

	createObjects();
}

void createVAOs(Vertex Vertices[], unsigned short Indices[], int ObjectId) {

	GLenum ErrorCheckValue = glGetError();
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);
	const size_t Normaloffset = sizeof(Vertices[0].Color) + RgbOffset;

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);	//
	glBindVertexArray(VertexArrayId[ObjectId]);		//

	// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[ObjectId], Vertices, GL_STATIC_DRAW);

	// Create Buffer for indices
	if (Indices != NULL) {
		glGenBuffers(1, &IndexBufferId[ObjectId]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[ObjectId], Indices, GL_STATIC_DRAW);
	}

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset); 
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Normaloffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color
	glEnableVertexAttribArray(2);	// normal

	// Disable our Vertex Buffer Object 
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);
	}
}

void cleanup(void)
{
	// Cleanup VBO and shader
	for (int i = 0; i < NumObjects; i++) {
		glDeleteBuffers(1, &VertexBufferId[i]);
		glDeleteBuffers(1, &IndexBufferId[i]);
		glDeleteVertexArrays(1, &VertexArrayId[i]);
	}
	glDeleteProgram(programID);
	glDeleteProgram(pickingProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ATTN: MODIFY AS APPROPRIATE
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_B:
			gMessage = "Base";
			base = (base + 1) % 2;
			if (base == 0) {
				loadObject("base.obj", glm::vec4(0.8, 0.0, 0.0, 1.0), Verts1, Idcs1, 2);
				createVAOs(Verts1, Idcs1, 2);
			}
			else {
				loadObject("base.obj", glm::vec4(1.0, 0.0, 0.0, 1.0), Verts1, Idcs1, 2);
				createVAOs(Verts1, Idcs1, 2);
			}
			break;
		case GLFW_KEY_T:
			gMessage = "Top";
			top = (top + 1) % 2;
			if (top == 0) {
				loadObject("cylinder.obj", glm::vec4(0.0, 0.8, 0.0, 1.0), Verts2, Idcs2, 3);
				createVAOs(Verts2, Idcs2, 3);
			}
			else {
				loadObject("cylinder.obj", glm::vec4(0.0, 1.0, 0.0, 1.0), Verts2, Idcs2, 3);
				createVAOs(Verts2, Idcs2, 3);
			}
			break;
		case GLFW_KEY_1:
			gMessage = "Arm1";
			arm1 = (arm1 + 1) % 2;
			if (arm1 == 0) {
				loadObject("arm1.obj", glm::vec4(0.0, 0.0, 0.8, 1.0), Verts3, Idcs3, 4);
				createVAOs(Verts3, Idcs3, 4);
			}
			else {
				loadObject("arm1.obj", glm::vec4(0.0, 0.0, 1.0, 1.0), Verts3, Idcs3, 4);
				createVAOs(Verts3, Idcs3, 4);
			}
			break;
		case GLFW_KEY_2: 
			gMessage = "Arm2";
			arm2 = (arm2 + 1) % 2;
			if (arm2 == 0) {
				loadObject("arm2.obj", glm::vec4(0.0, 0.8, 0.8, 1.0), Verts5, Idcs5, 6);
				createVAOs(Verts5, Idcs5, 6);
			}
			else {
				loadObject("arm2.obj", glm::vec4(0.0, 1.0, 1.0, 1.0), Verts5, Idcs5, 6);
				createVAOs(Verts5, Idcs5, 6);
			}		
			break;
		case GLFW_KEY_P:
			gMessage = "Pen";
			pen = (pen + 1) % 2;
			if (pen == 0) {
				loadObject("pen.obj", glm::vec4(0.8, 0.8, 0.0, 1.0), Verts6, Idcs6, 7);
				createVAOs(Verts6, Idcs6, 7);
			}
			else {
				loadObject("pen.obj", glm::vec4(1.0, 1.0, 0.0, 1.0), Verts6, Idcs6, 7);
				createVAOs(Verts6, Idcs6, 7);
			}
			break;
		case GLFW_KEY_C: 
			gMessage = "Camera";
			m = (m + 1)%2;
			break;
		case GLFW_KEY_J:
			gMessage = "Ball";
			anime = 1;
			fl = 1;
			tm = 0.01f;
			yaya = 1;
			break;
		default:
			break;
		}
	}

}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickObject();
	}
}

int main(void)
{
	// initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;

	// initialize OpenGL pipeline
	initOpenGL();

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do {
		//// Measure speed
		//double currentTime = glfwGetTime();
		//nbFrames++;
		//if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
		//	// printf and reset
		//	printf("%f ms/frame\n", 1000.0 / double(nbFrames));
		//	nbFrames = 0;
		//	lastTime += 1.0;
		//}
		rotateCam();
		rotateBase();
		rotateTop();
		rotateArm1();
		rotateArm2();
		rotatePen();
		animate();
		if (animation){
			phi += 0.01;
			if (phi > 360)
				phi -= 360;
		}

		// DRAWING POINTS
		renderScene();
		//getch();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	cleanup();

	return 0;
}