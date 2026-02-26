#include "Camera.h"

Camera::Camera(int windowWidth, int windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
}
aa::mat4 Camera::view()
{
	aa::mat4 view = aa::mat4(1.0f);
	aa::vec3 direction;
	direction.x = cos(aa::radians(yaw)) * cos(aa::radians(pitch));
	direction.y = sin(aa::radians(pitch));
	direction.z = sin(aa::radians(yaw)) * cos(aa::radians(pitch));
	cameraFront = aa::normalize(direction);
	view = aa::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return view;
}
aa::mat4 Camera::projection()
{
	float e = 0.0f;
	aa::mat4 projection;
	//projection = aa::perspective(aa::radians(zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
	float r = 5.0f;
	projection = aa::mat4(aa::vec4(1.0f,0.0f,0.0f,0.0f), aa::vec4(0.0f, 1.0f, 0.0f, 0.0f), aa::vec4(-e/2/r, 0.0f, 0.0f, -1.0f/r), aa::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	return projection;
}
aa::mat4 Camera::inverseViewProjection()
{
	aa::mat4 VP = projection() * view();
	aa::mat4 invVP = aa::inverse(VP);
	return invVP;
}