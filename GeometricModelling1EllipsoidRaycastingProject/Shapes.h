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
	Ellipsoid(float a, float b, float c);
	aa::vec3 radii;
	aa::mat4 ellipsoidMatrix;
	aa::vec3 getColor(aa::vec2 coords);
};

#endif