#include "Scene.h"

Scene::Scene(int windowWidth, int windowHeight, Shader _shader)
{
	camera = Camera(windowWidth, windowHeight);
    shader = _shader;
}
void Scene::DrawScene()
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
		static glm::vec3 scale(1.0f);
		static float rotationX = 0.0f;
		static float rotationY = 0.0f;
		static float rotationZ = 0.0f;
		static glm::vec3 translation(0.0f);
		ImGui::InputFloat3("Scale", glm::value_ptr(scale));
		if (ImGui::Button("Apply Scale"))
		{
			Scale(scale);
		}
		ImGui::DragFloat("Rotation X", &rotationX, 1.0f, -180.0f, 180.0f, "%.0f");
		if (ImGui::Button("Apply Rotation X"))
		{
			Rotate(rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		ImGui::DragFloat("Rotation Y", &rotationY, 1.0f, -180.0f, 180.0f, "%.0f");
		if (ImGui::Button("Apply Rotation Y"))
		{
			Rotate(rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		ImGui::DragFloat("Rotation Z", &rotationZ, 1.0f, -180.0f, 180.0f, "%.0f");
		if (ImGui::Button("Apply Rotation Z"))
		{
			Rotate(rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		ImGui::InputFloat3("Translation", glm::value_ptr(translation));
		if (ImGui::Button("Apply Translation"))
		{
			Translate(translation);
		}
		if (ImGui::Button("Reset Transformations"))
		{
			resetSceneMatrix();
			scale = glm::vec3(1.0f);
			rotationX = rotationY = rotationZ = 0.0f;
			translation = glm::vec3(0.0f);
		}
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Ellipsoid parameters:"))
	{
		ImGui::InputFloat3("Center", glm::value_ptr(ellipsoid.center));
		ImGui::InputFloat3("Radii", glm::value_ptr(ellipsoid.radii));
	}
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	ImGui::End();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.setMat4("view", camera.view());
    shader.setMat4("projection", camera.projection());
    shader.setMat4("inverseViewProjection", camera.inverseViewProjection());

    //Ellipsoid parameters
	shader.setVec3("ellipsoid.center", ellipsoid.center);
	shader.setVec3("ellipsoid.radii", ellipsoid.radii);

	plane.Draw(shader);
	ImGui::Render();
}
void Scene::Scale(glm::vec3 s)
{
    //model = glm::scale(model, s);
    glm::mat4 scaleMatrix = glm::mat4(glm::vec4(s.x, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, s.y, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, s.z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    sceneMatrix = scaleMatrix * sceneMatrix;
}
void Scene::Rotate(float angle, glm::vec3 axis)
{
    //model = glm::rotate(model, glm::radians(angle), axis);
    axis = glm::normalize(axis);
    float c = cos(glm::radians(angle));
    float s = sin(glm::radians(angle));
    glm::mat4 rotationMatrix;
    if (axis.x == 1.0f)
        rotationMatrix = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, c, s, 0.0f), glm::vec4(0.0f, -s, c, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    else if (axis.y == 1.0f)
        rotationMatrix = glm::mat4(glm::vec4(c, 0.0f, -s, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(s, 0.0f, c, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    else if (axis.z == 1.0f)
        rotationMatrix = glm::mat4(glm::vec4(c, s, 0.0f, 0.0f), glm::vec4(-s, c, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    else
    {
        std::cerr << "Rotation axis must be one of the cardinal axes (x,y,z).\n";
        return;
    }
    sceneMatrix = rotationMatrix * sceneMatrix;
}
void Scene::Translate(glm::vec3 t)
{
    //model = glm::translate(model, t);
    glm::mat4 translationMatrix = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(t.x, t.y, t.z, 1.0f));
    sceneMatrix = translationMatrix * sceneMatrix;
}
void Scene::resetSceneMatrix()
{
    sceneMatrix = glm::mat4(1.0f);
}