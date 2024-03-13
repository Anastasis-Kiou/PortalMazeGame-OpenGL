// BoundingBox.cpp
#include "BoundingBox.h"

BoundingBox::BoundingBox() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {} // Implementation of default constructor

BoundingBox::BoundingBox(const glm::vec3 &min, const glm::vec3 &max) : min(min), max(max) {}

// Function that returns true if there is a collision found
bool BoundingBox::checkCollision(const BoundingBox &other) const
{
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
}

// Function that calculates the normal direction that the player will be pushed back if he hits a boundingBox
glm::vec3 BoundingBox::calculateCollisionNormal(const BoundingBox &other) const
{
    glm::vec3 collisionNormal(0.0f);

    float overlapX = std::min(max.x, other.max.x) - std::max(min.x, other.min.x);
    float overlapY = std::min(max.y, other.max.y) - std::max(min.y, other.min.y);
    float overlapZ = std::min(max.z, other.max.z) - std::max(min.z, other.min.z);

    if (overlapX < overlapY && overlapX < overlapZ)
    {
        collisionNormal.x = (max.x > other.max.x) ? -1.0f : 1.0f;
    }
    else if (overlapY < overlapX && overlapY < overlapZ)
    {
        collisionNormal.y = (max.y > other.max.y) ? -1.0f : 1.0f;
    }
    else
    {
        collisionNormal.z = (max.z > other.max.z) ? -1.0f : 1.0f;
    }

    return glm::normalize(collisionNormal);
}
