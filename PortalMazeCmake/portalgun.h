#ifndef PORTALGUN_HPP
#define PORTALGUN_HPP

#include <glm/glm.hpp>
#include <common/model.h>
#include "RigidBody.h"
#include <glfw3.h>
#include <common/camera.h>
#include "avatar.h"

class Drawable;

class portalGun
{
public:
    glm::vec3 position;
    glm::mat4 modelMatrix;

    glm::vec3 currentAvatarPosition;
    void draw(unsigned int drawable = 0);
    portalGun(Avatar *avatar);
    void update(Avatar *avatar);
    glm::vec3 returnPos();

private:
    Drawable *gun;
};
#endif