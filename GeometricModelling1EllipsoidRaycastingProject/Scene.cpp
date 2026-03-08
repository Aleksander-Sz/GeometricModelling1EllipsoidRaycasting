#include "Scene.h"

Scene::Scene(int _windowWidth, int _windowHeight, Shader _shader)
{
	//camera = Camera(windowWidth, windowHeight);
	windowWidth = _windowWidth;
	windowHeight = _windowHeight;
    shader = _shader;
	framebuffer = std::vector<uint8_t>(windowWidth * windowHeight * 3);
	wasRendered = std::vector<bool>(windowWidth * windowHeight);

	for (int i = 0; i < wasRendered.size(); i++)
	{
		wasRendered[i] = false;
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
	ellipsoid.updateSceneMatrix(sceneMatrix);
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
		if (ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f))
		{
			resetSceneMatrix();
			resetSubdivisions();
		}
		if (ImGui::SliderFloat("Yaw", &yaw, -89.0f, 89.0f))
		{
			resetSceneMatrix();
			resetSubdivisions();
		}
		if (ImGui::SliderFloat("Scale", &scale, 0.0f, 5.0f))
		{
			resetSceneMatrix();
			resetSubdivisions();
		}
	}
	ImGui::Separator();
	static aa::vec3 ellipsoidRadii(0.1f, 0.5f, 0.8f);
	if (ImGui::CollapsingHeader("Ellipsoid parameters:"))
	{
		//ImGui::InputFloat3("Center", aa::value_ptr(ellipsoid.center));
		if (ImGui::InputFloat3("Radii", aa::value_ptr(ellipsoidRadii)))
			ellipsoid.setRadii(ellipsoidRadii);
	}
	ImGui::Text("Subdivisions: %d", subdivisions);
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	ImGui::End();

	// Rendering the ellipsoid to the texture
	//aa::vec3 cameraPos = aa::vec3(sin(aa::radians(yaw)),sin(aa::radians(pitch)),cos(-aa::radians(yaw))) * 10.0f;
	aa::vec3 cameraPos(0.0f, 0.0f, -6.0f);
	std::cout << "Camera Position = ( x: " << cameraPos.x << " y: " << cameraPos.y << " z: " << cameraPos.z << "\n";
	//aa::vec3 cameraPos = aa::vec3(0.0f, sin(aa::radians(pitch)), 0.0f) * 0.5f;

	resetSceneMatrix();
	ellipsoid.updateSceneMatrix(sceneMatrix);// aa::mat4(1.0f, 1.0f, 1.0f, 1.0f));
	int chunkWidth = windowWidth / subdivisions;
	int chunkHeight = windowHeight / subdivisions;

	int totalChunks = subdivisions * subdivisions;
	int threadCount = std::thread::hardware_concurrency();//std::cout << threadCount;
	if (threadCount == 0) threadCount = 4;
	
	int chunksPerThread = totalChunks / threadCount;

	std::vector<std::thread> threads;

	if (subdivisions <= 896)
	{
		for (int t = 0; t < threadCount; t++)
		{
			int startChunk = t * chunksPerThread;
			int endChunk = (t == threadCount - 1)
				? totalChunks
				: startChunk + chunksPerThread;

			threads.emplace_back([=]()
				{
					for (int chunk = startChunk; chunk < endChunk; chunk++)
					{
						int i = chunk % subdivisions;
						int j = chunk / subdivisions;
						int globalX = i * chunkWidth;
						int globalY = j * chunkHeight;
						int pointer =
							(globalX + globalY * windowWidth);
						if (wasRendered[pointer])
						{
							continue;
						}

						int chunkCenterX = (i + 0.0f) * chunkWidth;
						int chunkCenterY = (j + 0.0f) * chunkHeight;

						float NDCChunkCenterX =
							(float)chunkCenterX / windowWidth * 2.0f - 1.0f;
						float NDCChunkCenterY =
							(float)chunkCenterY / windowHeight * 2.0f - 1.0f;

						aa::vec3 color =
							ellipsoid.getColor(
								aa::vec2(NDCChunkCenterX, NDCChunkCenterY), cameraPos);
						wasRendered[pointer] = true;

						for (int localX = 0; localX < chunkWidth; localX++)
						{
							for (int localY = 0; localY < chunkHeight; localY++)
							{
								globalX = i * chunkWidth + localX;
								globalY = j * chunkHeight + localY;

								pointer =
									(globalX + globalY * windowWidth) * 3;

								framebuffer[pointer] =
									(uint8_t)(color.r * 255.0f);
								framebuffer[pointer + 1] =
									(uint8_t)(color.g * 255.0f);
								framebuffer[pointer + 2] =
									(uint8_t)(color.b * 255.0f);
							}
						}
					}
				});
		}

		for (auto& thread : threads)
			thread.join();
	}
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0, 0,
		windowWidth,
		windowHeight,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		framebuffer.data()
	);

    shader.use();
	shader.setInt("frame", 0);
	

	plane.Draw(shader);
	ImGui::Render();
}
void Scene::resetSceneMatrix()
{
	//std::cout << "pitch: " << pitch << ", yaw: " << yaw << "\n";
	sceneMatrix = /*aa::translate(aa::vec3(0.0f, 0.0f, -2.0f)) */ aa::rotate(aa::Y, -aa::radians(yaw)) * aa::rotate(aa::X, aa::radians(pitch)) * aa::scale(aa::vec3(1.0f / scale, 1.0f / scale, 1.0f / scale));
}

void Scene::resetSubdivisions()
{
	for (int i = 0; i < wasRendered.size(); i++)
	{
		wasRendered[i] = false;
	}
	subdivisions = 14;
}
