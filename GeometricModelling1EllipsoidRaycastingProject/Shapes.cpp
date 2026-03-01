#include "Shapes.h"

//Plane class functions
Plane::Plane()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	float vertices[] = {
		-1.0f, -1.0f,  0.0f,
		-1.0f,  1.0f,  0.0f,
		 1.0f, -1.0f,  0.0f,
		 1.0f,  1.0f,  0.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3
	};
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}
void Plane::Draw(Shader& shader)
{
	shader.use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

//Ellipsoid class functions
Ellipsoid::Ellipsoid(float a, float b, float c)
{
	radii = aa::vec3(a, b, c);
	ellipsoidMatrix = aa::mat4(a, b, c, 1);
}
void Ellipsoid::setRadii(aa::vec3 _radii)
{
	radii = _radii;
	dirty = true;
}
void Ellipsoid::updateSceneMatrix(aa::mat4 _sceneMatrix)
{
	sceneMatrix = _sceneMatrix;
	PrepareForDrawing();
}
void Ellipsoid::PrepareForDrawing()
{
	ellipsoidMatrix = aa::mat4(1 / radii.x / radii.x, 1 / radii.y / radii.y, 1 / radii.z / radii.z, -1);

	//calculating the final matrix = (M^-1)^T * D * M^-1
	aa::mat4 D = ellipsoidMatrix;
	aa::mat4 M = aa::inverse(sceneMatrix);
	Q = aa::transpose(M) * D * M;
	A = Q[0][0];
	B = Q[0][1];
	C = Q[0][2];
	D = Q[0][3];
	E = Q[1][1];
	F = Q[1][2];
	G = Q[1][3];
	H = Q[2][2];
	I = Q[2][3];
	J = Q[3][3];
}
aa::vec3 Ellipsoid::getColor(aa::vec2 v)
{
	if (dirty)
		PrepareForDrawing();
	aa::vec3 backgroundColor(0.1f, 0.5f, 0.3f);

	// now let's create the quadratic equasion based on this matrix
	//		| A B C D |				
	//	Q =	| B E F G |				v^T * Q * v = 0
	//		| C F H I |				
	//		| D G I J |				
	//
	//		Hz^2 + (2Cx+2Fy+2I)z + (Ax^2+Ey^2+2Bxy+2Dx+2Gy+J) = 0
	//		C1 * z^2 + C2 * z + C3 = 0
	//	Let's start by calculating the coefficients of this quadratic equation
	float C1, C2, C3;
	C1 = H;
	C2 = 2.0f * (C * v.x + F * v.y + I);
	C3 = A * v.x * v.x + E * v.y * v.y + 2.0f * B * v.x * v.y + 2.0f * D * v.x + 2.0f * G * v.y + J;

	if (fabs(C1) < 1e-6f)
		return backgroundColor;

	float delta = C2 * C2 - 4.0f * C1 * C3;
	//std::cout << "delta: " << delta << "\n";
	if (delta < 0.0f)
		return backgroundColor;
	float deltaSqrt = sqrt(delta);
	float z1 = (-C2 - deltaSqrt) / 2.0f / C1;
	float z2 = (-C2 + deltaSqrt) / 2.0f / C1;

	float solution = z1;
	if (z2 > z1)
		solution = z2;
	float x = v.x;
	float y = v.y;
	float z = solution;

	aa::vec3 normal;

	normal.x = A * x + B * y + C * z + D;
	normal.y = B * x + E * y + F * z + G;
	normal.z = C * x + F * y + H * z + I;
	normal = aa::normalize(normal);
	
	return normal;

	aa::vec3 ambient(0.1f, 0.1f, 0.1f), diffuse(0.5f, 0.5f, 0.0f), specular(1.0f, 1.0f, 1.0f);
	aa::vec3 p(x, y, z);
	aa::vec3 lightPos = aa::vec3(10.0f, 10.0f, 0.0f);
	aa::vec3 cameraPos = aa::vec3(0.0f, 1.0f, -3.0f);
	aa::vec3 lightDirection = aa::normalize(lightPos - p);
	aa::vec3 viewDirection = aa::normalize(cameraPos - p);
	float diff = std::fmax(dot(normal, lightDirection), 0.0f);
	aa::vec3 R = reflect(-lightDirection, normal);
	float spec = pow(std::fmax(dot(R, viewDirection), 0), 8.0f);
	return
		ambient +
		diffuse * diff +
		specular * spec;
}