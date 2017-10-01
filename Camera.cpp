#include "Camera.h"

Camera* Camera::activeCamera = NULL;

Camera::Camera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection)
{
	this->window = window;
	this->relativePosition = relativePosition;
	this->relativeDimensions = relativeDimensions;
	glfwGetWindowSize(window, &screenWidth, &screenHeight);

	mat4 ratioMatrix(1);
	ratioMatrix[1][1] *= relativeDimensions.x;
	Projection = Projection * ratioMatrix;
	OrthoProjection = glm::ortho(0.0f, (float)1200, 0.0f, (float)800);
	this->Projection = Projection;

	View = glm::lookAt(pos, lookAt, up);
	camPosVector = pos;
	lookAtVector = lookAt;
	upVector = up;
}

void Camera::setViewport()
{
	glfwGetWindowSize(window, &screenWidth, &screenHeight);
	glViewport(relativePosition.x * screenWidth, relativePosition.y * screenHeight, relativeDimensions.x * screenWidth, relativeDimensions.y * screenHeight);
}

void Camera::setCamera(Camera* cam)
{
	activeCamera = cam;
	activeCamera->setViewport();
}

int Camera::getScreenWidth()
{
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	return width;
}

int Camera::getScreenHeight()
{
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}

GLFWwindow* Camera::getWindow()
{
	return window;
}

SphericalCamera::SphericalCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection) :
	Camera(window, relativePosition, relativeDimensions, pos, lookAt, up, Projection)
{
	distance = length(camPosVector - lookAtVector);
	maxCamPhi = 0.7;

	//camTheta = atan2(pos.z - lookAt.z, pos.x - lookAt.x);
	update();
}

void SphericalCamera::update()
{
	if (camPhi > maxCamPhi)
		camPhi = maxCamPhi;
	else if (camPhi < maxCamPhi * -1)
		camPhi = maxCamPhi * -1;

	camPosVector = distance * vec3(cos(camTheta) * cos(camPhi), sin(camPhi), sin(camTheta) * cos(camPhi)) + lookAtVector;
	upVector = vec3(-cos(camTheta) * sin(camPhi), cos(camPhi), -sin(camTheta) * sin(camPhi));
	View = lookAt(camPosVector, lookAtVector, upVector);
}

StateSpaceCamera::StateSpaceCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection) :
	Camera(window, relativePosition, relativeDimensions, pos, lookAt, up, Projection)
{
	maxCamPhi = 0.7;

	camTheta = atan2(lookAt.z - pos.z, lookAt.x - pos.x);

	update();
}

void StateSpaceCamera::translate(vec2 offset)
{
	vec3 diff(cos(camTheta), 0, sin(camTheta));

//	if (length(camPosVector + diff * vec3(offset.x, 0, offset.y)) < 15)
		camPosVector += diff * 10.0f * vec3(offset.x, 0, offset.y);
}

// Method to call if the camera needs to update its matrix
void StateSpaceCamera::update()
{
	if (camPhi > maxCamPhi)
		camPhi = maxCamPhi;
	else if (camPhi < maxCamPhi * -1)
		camPhi = maxCamPhi * -1;

	lookAtVector = camPosVector + vec3(cos(camTheta) * cos(camPhi), sin(camPhi), sin(camTheta) * cos(camPhi));

	View = lookAt(camPosVector, lookAtVector, upVector);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	Projection = perspective(45.0f, (float)width / height, 0.1f, 1000.0f);
	//OrthoProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}

MenuCamera::MenuCamera(GLFWwindow* window, vec2 relativePosition, vec2 relativeDimensions, vec3 pos, vec3 lookAt, vec3 up, mat4 Projection) :
	Camera(window, relativePosition, relativeDimensions, pos, lookAt, up, Projection)
{

}

void MenuCamera::update()
{
	/*
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	OrthoProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
	*/
	
}