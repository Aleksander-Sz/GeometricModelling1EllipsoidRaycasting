#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include "Shader.h"
#include "Camera.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

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
	Ellipsoid(float a, float b, float c, float x0, float y0, float z0);
	glm::vec3 center;
	glm::vec3 radii;
};

#endif