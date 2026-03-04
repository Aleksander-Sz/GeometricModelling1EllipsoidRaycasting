#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "Camera.h"
#include "Shapes.h"
#include "AlexAlgebra.h"
#include <thread>

class Scene
{
public:
	Scene(int _windowWidth, int _windowHeight, Shader shader);
	int windowWidth, windowHeight;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float lastX = 400, lastY = 300;
	bool firstMovement = true;
	bool mouseLeftButtonPressed = false;
	double mouseLeftPressTime;
	aa::vec2 mouseLeftPressPosition;
	//Camera camera;
	Shader shader;
	Plane plane;
	Ellipsoid ellipsoid = Ellipsoid(0.1f, 0.2f, 0.3f);
	std::vector<uint8_t> framebuffer;
	std::vector<bool> wasRendered;
	GLuint texture;
	void DrawScene(unsigned int subdivisions);
	float pitch, yaw;
	float zoom;
	void resetSceneMatrix();
	unsigned int subdivisions = 7;
	void resetSubdivisions();
private:
	aa::mat4 sceneMatrix = aa::mat4(1.0f);
};

#endif