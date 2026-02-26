#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "Camera.h"
#include "Shapes.h"
#include "AlexAlgebra.h"

class Scene
{
public:
	Scene(int windowWidth, int windowHeight, Shader shader);
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float lastX = 400, lastY = 300;
	bool firstMovement = true;
	bool mouseLeftButtonPressed = false;
	double mouseLeftPressTime;
	aa::vec2 mouseLeftPressPosition;
	Camera camera;
	Shader shader;
	Plane plane;
	Ellipsoid ellipsoid = Ellipsoid(0.1f, 1.0f, 0.3f, 0.0f, 0.0f, 0.0f);
	void DrawScene();
	void Scale(aa::vec3 s);
	void Rotate(float angle, aa::vec3 axis);
	void Translate(aa::vec3 t);
	void resetSceneMatrix();
private:
	aa::mat4 sceneMatrix = aa::mat4(1.0f);
};

#endif