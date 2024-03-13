// Portal.h
#ifndef PORTAL_HPP
#define PORTAL_HPP

#include <glm/glm.hpp>
#include <common/model.h>
#include "RigidBody.h"
#include <glfw3.h>
#include <common/camera.h>
#include "avatar.h"

class Drawable;
using namespace glm;
using namespace std;

class Portal
{
public:
    GLFWwindow *window;
    vector<vec3> portalVertices;
    vector<vec2> portalUVs;
    vector<vec3> portalNormals;
    glm::vec3 portalPosition;
    Avatar *avatar;
    Portal(GLFWwindow *windowL, Avatar *avatarl, int usage);
    int mode;
    glm::mat4 portalModelMatrix;
    void update(float angle);
    void draw();
    int use;
    vec3 getNormal();
    vec3 getCenter();
    void adjustUv(float aspectRatio);

private:
    Drawable *portal;
};
#endif