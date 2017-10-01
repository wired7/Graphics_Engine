#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h> 
#include <glfw3.h>
#include "GraphicsObject.h"
#include "Shader.h"
#include <thread>
#include <chrono>

using namespace std;
using namespace glm;

GLFWwindow* window;

// GLEW and GLFW initialization. Projection and View matrix setup
int init() {
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1200, 800, "BOIDS", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	//Check version of OpenGL and print
	std::printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


	// White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glPointSize(3.0f);

	Shader::activeShader = new InstancedLitShader("shaders\\InstancedVertexShader.VERTEXSHADER", "shaders\\InstancedFragmentShader.FRAGMENTSHADER");

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	srand(time(NULL));

	return 1;
}

void drawingLoop()
{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Shader::activeShader->Use();
			glUniformMatrix4fv(Shader::activeShader->projectionID, 1, GL_FALSE, &(Camera::activeCamera->Projection[0][0]));
			glUniformMatrix4fv(Shader::activeShader->viewID, 1, GL_FALSE, &(Camera::activeCamera->View[0][0]));
		}
	
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwWindowShouldClose(window) == 0);
}

void updateLoop()
{
	stateSpace->update();
}

int main()
{
	if (init() < 0)
		return -1;

	thread update(updateLoop);

	drawingLoop();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}