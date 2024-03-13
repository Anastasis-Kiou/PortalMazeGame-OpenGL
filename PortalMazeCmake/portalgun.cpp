#include "portalgun.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glfw3.h>

#include <common/Shader.h>
#include <common/model.h>
#include <common/camera.h>

using namespace glm;

portalGun::portalGun(Avatar *avatar)
{
    gun = new Drawable("gun.obj");
    position = avatar->gunPosition;
}

glm::vec3 rotateY(const glm::vec3 &v, float angle)
{
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    return glm::vec3(
        cosAngle * v.x + sinAngle * v.z,
        v.y,
        -sinAngle * v.x + cosAngle * v.z);
}

void portalGun::update(Avatar *avatar)
{
    mat4 scaleMatrix = scale(mat4(1.0f), vec3(0.005));
    position = avatar->position;
    modelMatrix = translate(mat4(), vec3(position.x + 0.8, position.y + 1.6, position.z - 0.3));
    modelMatrix = mat4();
    //   std::cout << position.x << ", " << position.y << ", " << position.z << "\n";
}

void portalGun::draw(unsigned int drawable)
{
    gun->bind();
    gun->draw();
}

vec3 portalGun::returnPos()
{
    return position;
}