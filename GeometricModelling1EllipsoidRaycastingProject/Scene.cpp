#include "Scene.h"

Scene::Scene(int windowWidth, int windowHeight, Shader _shader)
{
	camera = Camera(windowWidth, windowHeight);
    shader = _shader;
	framebuffer = std::vector<uint8_t>(windowWidth * windowHeight * 3);

	uint8_t color = 0;
	for (int i = 0; i < framebuffer.size(); i+=2)
	{
		color++;
		color = color % 256;
		framebuffer[i] = color;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		windowWidth,
		windowHeight,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		framebuffer.data()
	);
}
void Scene::DrawScene(unsigned int subdivisions)
{
	//ImGui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Menu");
	ImGui::Text("Use WASD to move, mouse to look around, scroll to zoom.");
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Scene Transformations"))
	{
		static aa::vec3 scale(1.0f);
		static float rotationX = 0.0f;
		static float rotationY = 0.0f;
		static float rotationZ = 0.0f;
		static aa::vec3 translation(0.0f);
		ImGui::InputFloat3("Scale", aa::value_ptr(scale));
		if (ImGui::Button("Apply Scale"))
		{
			Scale(scale);
		}
		ImGui::DragFloat("Rotation X", &rotationX, 1.0f, -180.0f, 180.0f, "%.0f");
		if (ImGui::Button("Apply Rotation X"))
		{
			Rotate(rotationX, aa::vec3(1.0f, 0.0f, 0.0f));
		}
		ImGui::DragFloat("Rotation Y", &rotationY, 1.0f, -180.0f, 180.0f, "%.0f");
		if (ImGui::Button("Apply Rotation Y"))
		{
			Rotate(rotationY, aa::vec3(0.0f, 1.0f, 0.0f));
		}
		ImGui::DragFloat("Rotation Z", &rotationZ, 1.0f, -180.0f, 180.0f, "%.0f");
		if (ImGui::Button("Apply Rotation Z"))
		{
			Rotate(rotationZ, aa::vec3(0.0f, 0.0f, 1.0f));
		}
		ImGui::InputFloat3("Translation", aa::value_ptr(translation));
		if (ImGui::Button("Apply Translation"))
		{
			Translate(translation);
		}
		if (ImGui::Button("Reset Transformations"))
		{
			resetSceneMatrix();
			scale = aa::vec3(1.0f);
			rotationX = rotationY = rotationZ = 0.0f;
			translation = aa::vec3(0.0f);
		}
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Ellipsoid parameters:"))
	{
		//ImGui::InputFloat3("Center", aa::value_ptr(ellipsoid.center));
		ImGui::InputFloat3("Radii", aa::value_ptr(ellipsoid.radii));
	}
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	ImGui::End();

	// Rendering the ellipsoid to the texture
	int chunkWidth = camera.windowWidth / subdivisions;
	int chunkHeight = camera.windowHeight / subdivisions;

	for (int i = 0; i < subdivisions; i++)
	{
		for (int j = 0; j < subdivisions; j++)
		{
			int chunkCenterX = (i + 0.5f) * chunkWidth;
			int chunkCenterY = (j + 0.5f) * chunkHeight;
			aa::vec3 color = ellipsoid.getColor(aa::vec2(chunkCenterX, chunkCenterY));
			for (int localX = 0; localX < chunkWidth; localX++)
			{
				for (int localY = 0; localY < chunkHeight; localY++)
				{
					int globalX = i * chunkWidth + localX;
					int globalY = j * chunkHeight + localY;

					int pointer = (globalX + globalY * camera.windowWidth) * 3;

					framebuffer[pointer] = (uint8_t)color.r;
					framebuffer[pointer + 1] = (uint8_t)color.g;
					framebuffer[pointer + 2] = (uint8_t)color.b;
				}
			}
		}
	}

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // default is 4; set to 1 if width*3 is not multiple of 4
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0, 0,
		camera.windowWidth,
		camera.windowHeight,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		framebuffer.data()
	);

    shader.use();
	shader.setInt("frame", 0);
	

	plane.Draw(shader);
	ImGui::Render();
}
void Scene::Scale(aa::vec3 s)
{
    //model = aa::scale(model, s);
    aa::mat4 scaleMatrix = aa::mat4(aa::vec4(s.x, 0.0f, 0.0f, 0.0f), aa::vec4(0.0f, s.y, 0.0f, 0.0f), aa::vec4(0.0f, 0.0f, s.z, 0.0f), aa::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    sceneMatrix = scaleMatrix * sceneMatrix;
}
void Scene::Rotate(float angle, aa::vec3 axis)
{
    //model = aa::rotate(model, aa::radians(angle), axis);
    axis = aa::normalize(axis);
    float c = cos(aa::radians(angle));
    float s = sin(aa::radians(angle));
    aa::mat4 rotationMatrix;
    if (axis.x == 1.0f)
        rotationMatrix = aa::mat4(aa::vec4(1.0f, 0.0f, 0.0f, 0.0f), aa::vec4(0.0f, c, s, 0.0f), aa::vec4(0.0f, -s, c, 0.0f), aa::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    else if (axis.y == 1.0f)
        rotationMatrix = aa::mat4(aa::vec4(c, 0.0f, -s, 0.0f), aa::vec4(0.0f, 1.0f, 0.0f, 0.0f), aa::vec4(s, 0.0f, c, 0.0f), aa::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    else if (axis.z == 1.0f)
        rotationMatrix = aa::mat4(aa::vec4(c, s, 0.0f, 0.0f), aa::vec4(-s, c, 0.0f, 0.0f), aa::vec4(0.0f, 0.0f, 1.0f, 0.0f), aa::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    else
    {
        std::cerr << "Rotation axis must be one of the cardinal axes (x,y,z).\n";
        return;
    }
    sceneMatrix = rotationMatrix * sceneMatrix;
}
void Scene::Translate(aa::vec3 t)
{
    //model = aa::translate(model, t);
    aa::mat4 translationMatrix = aa::mat4(aa::vec4(1.0f, 0.0f, 0.0f, 0.0f), aa::vec4(0.0f, 1.0f, 0.0f, 0.0f), aa::vec4(0.0f, 0.0f, 1.0f, 0.0f), aa::vec4(t.x, t.y, t.z, 1.0f));
    sceneMatrix = translationMatrix * sceneMatrix;
}
void Scene::resetSceneMatrix()
{
    sceneMatrix = aa::mat4(1.0f);
}