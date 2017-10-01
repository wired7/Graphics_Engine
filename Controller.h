#pragma once
#include <functional>
#include "Camera.h"
#include "Context.h"

class Controller
{
public:
	static void setController(Controller*);
	void(*key_callback)(GLFWwindow*, int, int, int, int) = NULL;
	void(*scroll_callback)(GLFWwindow*, double, double) = NULL;
	void(*mouse_callback)(GLFWwindow*, int, int, int) = NULL;
	void(*mousePos_callback)(GLFWwindow*, double, double) = NULL;
	void(*windowResize_callback)(GLFWwindow*, int, int) = NULL;

	static glm::vec2 mouseScreenToGUICoords(GLFWwindow* window, double x, double y);
};

class ContextController : public Controller
{
private:
	Context* context;
	ContextController();
	~ContextController();
	static void kC(GLFWwindow*, int, int, int, int);
	static void sC(GLFWwindow*, double, double);
	static void mC(GLFWwindow*, int, int, int);
	static void mPC(GLFWwindow*, double, double);
	static void wRC(GLFWwindow*, int, int);
public:
	static Controller* getController();
};