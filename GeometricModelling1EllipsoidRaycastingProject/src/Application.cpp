#define GLAD_GL_IMPLEMENTATION
#include <GLAD/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Shader.h"
#include "../Camera.h"
#include "../Shapes.h"
#include "../Scene.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Scene* scene;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard)
		return;
	float cameraSpeed = 2.5f;
	const float cameraDisplacement = cameraSpeed * scene->deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwWindowShouldClose(window);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		scene->camera.cameraPos += cameraDisplacement * glm::normalize(scene->camera.cameraFront);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scene->camera.cameraPos -= cameraDisplacement * glm::normalize(scene->camera.cameraFront);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene->camera.cameraPos -= cameraDisplacement * glm::normalize(cross(scene->camera.cameraFront, scene->camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene->camera.cameraPos += cameraDisplacement * glm::normalize(cross(scene->camera.cameraFront, scene->camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		scene->camera.cameraPos += cameraDisplacement * glm::normalize(cross(cross(scene->camera.cameraFront, scene->camera.cameraUp), scene->camera.cameraFront));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		scene->camera.cameraPos -= cameraDisplacement * glm::normalize(cross(cross(scene->camera.cameraFront, scene->camera.cameraUp), scene->camera.cameraFront));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) // Check if ImGui wants to capture the mouse input
		return;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		scene->mouseLeftButtonPressed = true;
		scene->firstMovement = true; // Reset first movement flag when the button is pressed
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		scene->mouseLeftButtonPressed = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xOffset = xpos - scene->lastX;
	float yOffset = ypos - scene->lastY;
	if (scene->mouseLeftButtonPressed)
	{
		if (scene->firstMovement)
		{
			scene->firstMovement = false;
			scene->lastX = xpos;
			scene->lastY = ypos;
		}

		scene->lastX = xpos;
		scene->lastY = ypos;

		const float sensitivity = 0.1f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		scene->camera.yaw -= xOffset;
		scene->camera.pitch += yOffset;

		if (scene->camera.pitch > 89.0f)
			scene->camera.pitch = 89.0f;
		if (scene->camera.pitch < -89.0f)
			scene->camera.pitch = -89.0f;
	}
	scene->lastX = xpos;
	scene->lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;
	scene->camera.zoom -= (float)yoffset;
	if (scene->camera.zoom < 10.0f)
		scene->camera.zoom = 10.0f;
	if (scene->camera.zoom > 45.0f)
		scene->camera.zoom = 45.0f;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	int windowWidth = 900; // Your desired width
	int windowHeight = 900; // Your desired height

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Geometric Modelling 1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// 2. Center the window
	int monitorX, monitorY;
	glfwGetMonitorPos(monitor, &monitorX, &monitorY);

	glfwSetWindowPos(
		window,
		monitorX + (mode->width - windowWidth) / 2,
		monitorY + (mode->height - windowHeight) / 2
	);

	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0, 0, fbWidth, fbHeight);

	//ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	Shader ourShader("Shaders/VertexShader.glsl","Shaders/FragmentShader.glsl");
	Scene sceneObject = Scene(900, 900, ourShader);
	scene = &sceneObject;

	// Rendering commands here

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	unsigned int frame = 0;
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

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
				scene->Scale(scale);
			}
			ImGui::DragFloat("Rotation X", &rotationX, 1.0f, -180.0f, 180.0f, "%.0f");
			if (ImGui::Button("Apply Rotation X"))
			{
				scene->Rotate(rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
			}
			ImGui::DragFloat("Rotation Y", &rotationY, 1.0f, -180.0f, 180.0f, "%.0f");
			if (ImGui::Button("Apply Rotation Y"))
			{
				scene->Rotate(rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			ImGui::DragFloat("Rotation Z", &rotationZ, 1.0f, -180.0f, 180.0f, "%.0f");
			if (ImGui::Button("Apply Rotation Z"))
			{
				scene->Rotate(rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
			}
			ImGui::InputFloat3("Translation", glm::value_ptr(translation));
			if (ImGui::Button("Apply Translation"))
			{
				scene->Translate(translation);
			}
			if (ImGui::Button("Reset Transformations"))
			{
				scene->resetSceneMatrix();
				scale = glm::vec3(1.0f);
				rotationX = rotationY = rotationZ = 0.0f;
				translation = glm::vec3(0.0f);
			}
		}
		ImGui::Separator();
		ImGui::Text("Ellipsoid parameters:");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

		ImGui::End();

		//rendering commands here
		
		scene->DrawScene();
		// -----
		float currentFrame = glfwGetTime();
		scene->deltaTime = currentFrame - scene->lastFrame;
		scene->lastFrame = currentFrame;
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
		frame++;
	}
	glfwTerminate();
	return 0;
}