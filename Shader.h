#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <glew.h>
#include <glm.hpp>

using namespace std;
using namespace glm;

class Shader
{
public:
	static Shader* activeShader;
	GLuint program;
	GLuint viewID;
	GLuint projectionID;
	Shader() {};
	Shader(const char* vertexPath, const char* fragmentPath);
	virtual GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
	virtual void Use();
};

class LitShader : public Shader
{
public:
	GLuint modelID;
	LitShader() {};
	LitShader(const char* vertexPath, const char* fragmentPath);
};

class InstancedLitShader : public Shader
{
public:
	InstancedLitShader() {};
	InstancedLitShader(const char* vertexPath, const char* fragmentPath);
};