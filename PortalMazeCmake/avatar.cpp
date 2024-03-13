#include "avatar.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glfw3.h>

#include <common/Shader.h>
#include <common/model.h>
#include <common/camera.h>

using namespace glm;

Avatar::Avatar(Camera *camera)
{
    avatar = new Drawable("Avatar.obj");
    position = camera->position;
    rotation = camera->returnRotation();
    modelMode = 2;
    vec3 currentCameraPosition;
    vec3 currentCameraPosition2;
}

void Avatar::update(Camera *camera)
{

    mat4 scaleMatrix = scale(mat4(1.0f), vec3(0.15));
    rotation = camera->returnRotation();

    // Mode 2 is when the avatar is following the camera
    if (camera->mode == 2)
    { // Check if we already were in mode 2 or we switched from mode 1
        if (modelMode == 1)
        { // Translate the avatar in the position it was when I pressed 1 (currentCameraPosition2)
            position = vec3(currentCameraPosition2.x - 2, currentCameraPosition2.y - 2, currentCameraPosition2.z - 2);
            modelMatrix = translate(mat4(1.0f), position) * scaleMatrix;
            camera->position = currentCameraPosition2; // Change the camera so that it goes back to where it was
            modelMode = 2;
        }
        else if (modelMode == 2) // Avatar follows camera
        {
            position = vec3(camera->position.x, camera->position.y - 3.1, camera->position.z);
            modelMatrix = translate(mat4(1.0f), position) * scaleMatrix;
        }
        currentCameraPosition = camera->position; // Keep updating the currentPosition
    }
    if (camera->mode == 1)
    { // Avatar stops following camera, so no more updating the modelMatrix
        modelMode = 1;
        currentCameraPosition2 = currentCameraPosition; // Store the position the avatar was when 1 was pressed
    }

    // Gun Coordinates
    // vec3 gunOffset = vec3(0.2f, 2.0f, 0.2f);
    // vec3 localGunOffset = vec3(
    //     gunOffset.x * cos(rotation.y) - gunOffset.z * sin(rotation.y),
    //     gunOffset.y,
    //     gunOffset.x * sin(rotation.y) + gunOffset.z * cos(rotation.y));

    // gunPosition = position + camera->right * localGunOffset.x + camera->up * localGunOffset.y + camera->direction * localGunOffset.z;

    // gunModelMatrix = translate(mat4(1.0f), gunPosition);
}

void Avatar::draw(unsigned int drawable)
{
    avatar->bind();
    avatar->draw();
}

vec3 Avatar::returnPos()
{
    return position;
}

vec3 Avatar::returnRotation()
{
    return rotation;
}