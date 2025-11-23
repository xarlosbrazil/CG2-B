#ifndef SYSTEM_H
#define SYSTEM_H

// Internal
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

// External Libs
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL.h>

// GLM Includes
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

// Headers
#include "Shader.h"
#include "AssetManager.h"
#include "Time.h"
#include "Mesh.h"
#include "GameObject.h"

class System
{
private:
	// Screen
	const GLint WIDTH = 600, HEIGHT = 600;
	int screenWidth, screenHeight;
	GLuint bulletVAO;
	void setupBullet();
	std::vector<GameObject> sceneObjects; // vetor para guardar os objetos da cena
	std::vector<Mesh*> meshes; // vetor para guardar os meshes criados e limpar a memória depois

public:
	GLFWwindow* window;
	Shader coreShader;

public:
	System();
	~System();

	int GLFWInit();
	int OpenGLSetup();
	int SystemSetup();

	void Run();

	void Finish();

};

#endif