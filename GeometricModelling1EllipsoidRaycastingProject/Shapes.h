#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include "Shader.h"
#include "Camera.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <math.h>

class Plane
{
public:
	Plane();
	void Draw(Shader& shader);
protected:
	unsigned int VAO, VBO, EBO;
};
class Ellipsoid
{
public:
	Ellipsoid(float a, float b, float c);
	aa::vec3 getColor(aa::vec2 v);
	void updateSceneMatrix(aa::mat4 _sceneMatrix);
	void setRadii(aa::vec3 _radii);
private:
	aa::mat4 sceneMatrix;
	aa::vec3 radii;
	aa::mat4 ellipsoidMatrix;
	bool dirty = true;
	void PrepareForDrawing();
	aa::mat4 Q;
	float A, B, C, D, E, F, G, H, I, J;
};

#endif