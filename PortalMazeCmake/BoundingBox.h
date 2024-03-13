// BoundingBox.hpp
#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP

#include <glm/glm.hpp>
#include <cmath>
#include <algorithm>

class BoundingBox
{
public:
    BoundingBox();
    glm::vec3 min;
    glm::vec3 max;

    BoundingBox(const glm::vec3 &min, const glm::vec3 &max);
    bool checkCollision(const BoundingBox &other) const;
    glm::vec3 calculateCollisionNormal(const BoundingBox &other) const;
};

#endif
