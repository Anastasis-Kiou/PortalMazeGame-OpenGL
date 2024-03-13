// Avatar.h
#ifndef AVATAR_HPP
#define AVATAR_HPP

#include <glm/glm.hpp>
#include <common/model.h>
#include "RigidBody.h"
#include <glfw3.h>
#include <common/camera.h>

class Drawable;

class Avatar
{
public:
    glm::vec3 position;
    glm::mat4 modelMatrix;
    glm::vec3 rotation;
    glm::mat4 gunModelMatrix;
    glm::vec3 gunPosition;

    int modelMode;
    bool thirdPerson;

    glm::vec3 currentCameraPosition;
    glm::vec3 currentCameraPosition2;

    Avatar(Camera *camera);
    void draw(unsigned int drawable = 0);
    void update(Camera *camera);
    glm::vec3 returnPos();
    glm::vec3 returnRotation();

private:
    Drawable *avatar;
};

#endif