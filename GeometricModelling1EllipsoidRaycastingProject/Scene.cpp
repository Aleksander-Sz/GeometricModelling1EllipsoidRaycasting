#include "Scene.h"

Scene::Scene(int windowWidth, int windowHeight, Shader _shader)
{
	camera = Camera(windowWidth, windowHeight);
    shader = _shader;
}
void Scene::DrawScene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.setMat4("view", camera.view());
    shader.setMat4("projection", camera.projection());
	plane.Draw(shader);
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