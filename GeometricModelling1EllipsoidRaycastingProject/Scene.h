#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "Camera.h"
#include "Shapes.h"

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
	glm::vec2 mouseLeftPressPosition;
	Camera camera;
	Shader shader;
	Plane plane;
	void DrawScene();
	void Scale(glm::vec3 s);
	void Rotate(float angle, glm::vec3 axis);
	void Translate(glm::vec3 t);
	void resetSceneMatrix();
private:
	glm::mat4 sceneMatrix = glm::mat4(1.0f);
};

#endif