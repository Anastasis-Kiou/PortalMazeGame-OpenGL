#include "Portal.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glfw3.h>

#include <common/Shader.h>
#include <common/model.h>
#include <common/camera.h>

using namespace glm;
using namespace std;

Portal::Portal(GLFWwindow *windowL, Avatar *avatarl, int usage)
{
    use = usage;
    avatar = avatarl;
    window = windowL;
    portalPosition = vec3(0, 0, 0);
    // Vertices for the rectangle
    std::vector<glm::vec3> portalVertices = {
        glm::vec3(-1.5f, 3.0f, 0.0f),
        glm::vec3(-1.5f, -3.0f, 0.0f),
        glm::vec3(1.5f, -3.0f, 0.0f),
        glm::vec3(1.5f, -3.0f, 0.0f),
        glm::vec3(-1.5f, 3.0f, 0.0f),
        glm::vec3(1.5f, 3.0f, 0.0f)};

    // UV coordinates for the rectangle
    std::vector<glm::vec3> portalNormals = {
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)};

    // UV coordinates for the rectangle (linear mapping)
    std::vector<glm::vec2> portalUVs = {
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f)};
    portal = new Drawable(portalVertices, portalUVs, portalNormals);
    mode = 0;
}

void Portal::update(float angle)
{
    // Flags that store which portal is active
    static bool qKeyWasPressed = false;
    static bool eKeyWasPressed = false;
    int qKeyState = glfwGetKey(window, GLFW_KEY_Q);

    int eKeyState = glfwGetKey(window, GLFW_KEY_E);

    // Toggle portal state only if the Q key was just pressed
    if (qKeyState == GLFW_PRESS && !qKeyWasPressed)
    {
        qKeyWasPressed = true;

        // Toggle portal state
        if (mode == 0)
        {
            float angleRot = angle;
            mode = 1;
            mat4 scaleMatrix = scale(mat4(), vec3(0.9));
            portalPosition = avatar->position + vec3(2 * sin(radians(angleRot)), 2.5f, 2 * cos(radians(angleRot)));
            portalModelMatrix = translate(mat4(), portalPosition) * scaleMatrix;
            // cout << "PORTAL ENABLED \n";
        }
        else if (mode == 1)
        {
            mode = 0;
            portalModelMatrix = mat4();
            // cout << "PORTAL DISABLED \n";
        }
    }
    else if (qKeyState == GLFW_RELEASE)
    {
        // Reset the flag when the Q key is released
        qKeyWasPressed = false;
    }
    if (use == 2)
    {
        if (eKeyState == GLFW_PRESS && !eKeyWasPressed)
        {
            eKeyWasPressed = true;

            // Toggle portal state
            if (mode == 0)
            {
                float angleRot = angle;
                mode = 1;
                mat4 scaleMatrix = scale(mat4(), vec3(0.9));
                portalPosition = avatar->position + vec3(2 * sin(radians(angleRot)), 2.5f, 2 * cos(radians(angleRot)));
                portalModelMatrix = translate(mat4(), portalPosition) * scaleMatrix;
                // cout << "PORTAL ENABLED \n";
            }
            else if (mode == 1)
            {
                mode = 0;
                portalModelMatrix = mat4();
                // cout << "PORTAL DISABLED \n";
            }
        }
        else if (eKeyState == GLFW_RELEASE)
        {
            // Reset the flag when the Q key is released
            eKeyWasPressed = false;
        }
    }
}

void Portal::draw()
{
    portal->bind();
    portal->draw();
}

vec3 Portal::getNormal()
{
    // Define two vectors representing two sides of the rectangle
    vec3 v1 = vec3(-1.5f, -3.0f, 0.0f) - vec3(-1.5f, 3.0f, 0.0f);
    vec3 v2 = vec3(1.5f, -3.0f, 0.0f) - vec3(-1.5f, -3.0f, 0.0f);

    // Calculate the cross product of v1 and v2 to obtain the normal vector
    vec3 normal = cross(v1, v2);

    // Normalize the normal vector
    normal = normalize(normal);

    return normal;
}

vec3 Portal::getCenter()
{
    vec3 sum = vec3(0.0f);
    for (const auto &vertex : portalVertices)
    {
        // Transform each vertex to world space using the portal's model matrix
        vec4 worldVertex = portalModelMatrix * vec4(vertex, 1.0f);
        sum += vec3(worldVertex);
    }

    // Calculate the average position of the vertices
    vec3 center = sum / static_cast<float>(portalVertices.size());
    return center;
}

// Adjust the UVs of the portal to better fit the camera texture
void Portal::adjustUv(float aspectRatio)
{
    for (auto uv : portalUVs)
    {
        uv.x *= aspectRatio;
    }
}