#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <PortalMazeCmake/BoundingBox.h>

class Camera {
public:
    GLFWwindow* window;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    // Initial position : on +Z
    glm::vec3 position;
   //glm::vec3 previousPosition;

    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 up;
    // Initial horizontal angle : toward -Z
    float horizontalAngle;
    // Initial vertical angle : none
    float verticalAngle;
    // Field of View
    float FoV;
    float speed; // units / second
    float mouseSpeed;
    float fovSpeed;
    int mode;
    float rotAngle;
    float deltaTime;
    bool doesMove;
    BoundingBox cameraBox;
//    static const int gridSizeX = 10; // You can adjust the values accordingly
//    static const int gridSizeY = 10;
//    static const int gridSizeZ = 10;
//

    Camera(GLFWwindow* window);
   // std::vector<BoundingBox> &wallBox; // Reference to wallBox

    void update();
    void updateBoundingBox();
    void handleCollision(BoundingBox wallBox);
    void setDirection(glm::vec3 directionVector);
    void updatePos(glm::vec3 newPos);
    void setProjectionMatrix(float aspect, float near, float far);
    void initializeWithRotation(float rotationAngle);
   // void updateSpatialGrid();
    glm::vec3 returnRotation();
};

#endif
