#define _CRT_SECURE_NO_WARNINGS
#include "../../common/tiny_obj_loader.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
// render
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../common/uniform.h"
#include "../../common/shader_glsl.h"
#include "../../common/tiny_jpeg_decoder.h"
#include <GL/glut.h>
#include <GL/GLAux.h>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "texture.h"
#include <Windows.h>
#define REF_WIDTH  800//1024
#define REF_HEIGHT  600//768
GLSLShaderManager * _shaderManager = new GLSLShaderManager("shaders");
std::vector<GLuint> VBOs;
std::vector<GLuint> NBOs;
std::vector<GLuint> IBOs;
std::vector<size_t> Indices_nums;
std::vector<GLuint> UVBOs;
std::vector<int> materialIDs;
std::vector<GLuint> Textures;

glm::mat4 P;
glm::mat4x4 V;
glm::mat4x4 M;
glm::mat4x4 MVP;

static void PrintInfo(const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials)
{
	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	for (size_t i = 0; i < shapes.size(); i++) {
		printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
		printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
		printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
		assert((shapes[i].mesh.indices.size() % 3) == 0);
		for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
			printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3 * f + 0], shapes[i].mesh.indices[3 * f + 1], shapes[i].mesh.indices[3 * f + 2], shapes[i].mesh.material_ids[f]);
		}

		printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
		assert((shapes[i].mesh.positions.size() % 3) == 0);
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			printf("  v[%ld] = (%f, %f, %f)\n", v,
				shapes[i].mesh.positions[3 * v + 0],
				shapes[i].mesh.positions[3 * v + 1],
				shapes[i].mesh.positions[3 * v + 2]);
		}
	}

	for (size_t i = 0; i < materials.size(); i++) {
		printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
		printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
		printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
		printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
		printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
		printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
		printf("  material.Ns = %f\n", materials[i].shininess);
		printf("  material.Ni = %f\n", materials[i].ior);
		printf("  material.dissolve = %f\n", materials[i].dissolve);
		printf("  material.illum = %d\n", materials[i].illum);
		printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
		printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
		printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
		printf("  material.map_Ns = %s\n", materials[i].normal_texname.c_str());
		std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
		std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
		for (; it != itEnd; it++) {
			printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
		}
		printf("\n");
	}
}

void init()
{
	if (!_shaderManager->startup()){
		printf("init: failed to create shaderManager");
		return;
	}
	_shaderManager->loadProgram("shader", 0, 0, 0, "shader");
}

static bool
createGPUbuffers(
	std::vector<tinyobj::shape_t> shapes,
	std::vector<tinyobj::material_t> materials)
{
	GLuint VBO;
	GLuint NBO;
	GLuint IBO;
	size_t Indices_num;
	int materialID;
	GLuint UVBO;

	for (size_t i = 0; i < shapes.size(); i++) {
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &NBO);
		glGenBuffers(1, &IBO);
		glGenBuffers(1, &UVBO);
		// 一个mesh 可以有多个material ？？？
		if (shapes[i].mesh.material_ids.size() > 1)
		{
			materialID = shapes[i].mesh.material_ids[0];
		}
		else{
			materialID = -1;
		}
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (shapes[i].mesh.positions.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.positions.size(),
				&(shapes[i].mesh.positions[0]), GL_STATIC_DRAW);
		}
		else{
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.positions.size(),
				NULL, GL_STATIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER, NBO);
		if (shapes[i].mesh.normals.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.normals.size(),
				&(shapes[i].mesh.normals[0]), GL_STATIC_DRAW);
		}
		else{
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.normals.size(),
				NULL, GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		if (shapes[i].mesh.indices.size() > 0){
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * shapes[i].mesh.indices.size(), 
				&(shapes[i].mesh.indices[0]), GL_STATIC_DRAW);
		}
		else {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * shapes[i].mesh.indices.size(), 
				NULL, GL_STATIC_DRAW);
		}
		Indices_num = shapes[i].mesh.indices.size();
		glBindBuffer(GL_ARRAY_BUFFER, UVBO);
		if (shapes[i].mesh.texcoords.size() > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*shapes[i].mesh.texcoords.size(),
				&(shapes[i].mesh.texcoords[0]), GL_STATIC_DRAW);
		}
		else {
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*shapes[i].mesh.texcoords.size(),
				NULL, GL_STATIC_DRAW);
		}
		VBOs.push_back(VBO);
		NBOs.push_back(NBO);
		IBOs.push_back(IBO);
		Indices_nums.push_back(Indices_num);
		UVBOs.push_back(UVBO);
		materialIDs.push_back(materialID);
	}

	for (size_t i = 0; i < materials.size(); i++){
		// get file name 
		std::string flag = "\\";
		std::string::size_type position;
		position = materials[i].ambient_texname.rfind(flag);
		std::string fileName;
		if (position != materials[i].ambient_texname.npos)
		{
			fileName = materials[i].ambient_texname.substr(position+1);
		}
		else{
			fileName.empty();
		}
		if (fileName.size() == 0)
		{
			Textures.push_back(-1);
			continue;
		}
		Textures.push_back(LoadJPEG(materials[i].ambient_texname.c_str()));

	}

	// create textures

	std::cout << "create GPU buffer ok" << std::endl;
	return true;
}
static bool
LoadObj(
const char* filename,
const char* basepath = NULL)
{
	std::cout << "Loading " << filename << std::endl;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err = tinyobj::LoadObj(shapes, materials, filename, basepath);

	if (!err.empty()) {
		std::cerr << err << std::endl;
		return false;
	}

	//PrintInfo(shapes, materials);
	std::cout << "now create GPU buffer" << std::endl;
	createGPUbuffers(shapes, materials);

	return true;
}
void display()
{
	static float angle = 0;
	angle += 0.01;
	glm::vec3 center = glm::vec3(0.0, 60.0, 0.0);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	//glm::vec3 eye = glm::vec3(1000.0 * sin(angle), 60.0, 1000.0*cos(angle));
	glm::vec3 eye = glm::vec3(200.0 * sin(angle), 100.0, 200.0*cos(angle));
	//glm::vec3 eye = glm::vec3(1.0 * sin(angle), 80.0, 2.0*cos(angle));
	V = glm::lookAt(eye, center, up);
	P = glm::perspective(45.0f, (GLfloat)REF_WIDTH / (GLfloat)REF_HEIGHT, 0.1f, 100000.f);
	glViewport(0, 0, REF_WIDTH, REF_HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(_shaderManager->program[0].handle);
	M = glm::mat4x4();
	MVP = P*V*M;
	glm::mat4x4 MV = V*M;
	glUniformMatrix4fv(_shaderManager->program[0].getUniformLocation("gWorld"),
		1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(_shaderManager->program[0].getUniformLocation("MV"),
		1, GL_FALSE, &MV[0][0]);
	glUniform1i(_shaderManager->program[0].getUniformLocation("myTextureSampler"), 0);
	for (size_t i = 0; i < VBOs.size(); i++) {
		if (materialIDs[i] == -1 || Textures[materialIDs[i]] == -1)// no texture
		{// base color; useTexture = false;
			glUniform1i(_shaderManager->program[0].getUniformLocation("useTexture"), 0);
			glUniform4fv(_shaderManager->program[0].getUniformLocation("baseColor"), 1,
				&glm::vec4(0.0, 1.0, 0.0, 0.0)[0]);
		}
		else{
			// useTexture = true;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Textures[materialIDs[i]]);
			glUniform1i(_shaderManager->program[0].getUniformLocation("useTexture"), 1);
		}
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, UVBOs[i]);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
			);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOs[i]);
		glDrawElements(GL_TRIANGLES, Indices_nums[i], GL_UNSIGNED_INT, 0);
		// 顶点个数
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}


	glutSwapBuffers();

}

int main(int argc, char ** argv)
{
	//freopen("stdout.txt", "w", stdout);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 10);
	glutInitWindowSize(REF_WIDTH, REF_HEIGHT);
	glutCreateWindow(argv[0]);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0);
	if (glewInit()){
		printf("Unable to initialize GLEW ....\n");
		return 1;
	}
	if (argc > 1) {
		const char* basepath = NULL;
		if (argc > 2) {
			basepath = argv[2];
		}
		assert(true == LoadObj(argv[1], basepath));
	}
	else {
		std::cerr << "error no obj file" << std::endl;
		return -1;
	}
	init();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	//glutReshapeFunc(ReSizeGLScene);
	//glutMouseFunc(MouseFunc);
	//glutMotionFunc(MouseMotion);

	glutMainLoop();

	return 0;

}
