#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

using namespace glm;
//std::vector<std::vector<std::vector<std::vector<BoundingBox>>>> spatialGrid;


Camera::Camera(GLFWwindow *window) : window(window), cameraBox(glm::vec3(0.0f), glm::vec3(0.0f))
{
    position = vec3(0, 3, 5);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    FoV = 75.0f;
    speed = 10.0f;
    mouseSpeed = 0.001f;
    fovSpeed = 2.0f;
    mode = 2;
    doesMove = false;

}

void Camera::update()
{
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();
    
    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    deltaTime = float(currentTime - lastTime);
    
    // Get mouse position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // Reset mouse position for next frame
    glfwSetCursorPos(window, width / 2, height / 2);
    
    vec3 currentDir = direction;
    //cursor position
    if (doesMove){
        horizontalAngle += mouseSpeed * float(width / 2 - xPos);
        verticalAngle += mouseSpeed * float(height / 2 - yPos);
        
        // use spherical coordinates
        direction = vec3(
                         cos(verticalAngle) * sin(horizontalAngle),
                         sin(verticalAngle),
                         cos(verticalAngle) * cos(horizontalAngle));
    }
        float angle = acos(dot(currentDir, direction));
        rotAngle = angle;
    
        // Right vector
        right = vec3(
                     sin(horizontalAngle - 3.14f / 2.0f),
                     0,
                     cos(horizontalAngle - 3.14f / 2.0f));
        
        // Up vector
        up = cross(right, direction);
    
    if (doesMove){
        
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            mode = 1;
        }
        
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS || mode == 2)
        {
            position.y = 1.8;
            mode = 2;
        }
        
        // Move forward
        if (mode == 1){
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                position += direction * deltaTime * speed;
            }
            // Move backward
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                position -= direction * deltaTime * speed;
            }
            // Strafe right
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                position += right * deltaTime * speed;
            }
            // Strafe left
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                position -= right * deltaTime * speed;
            }
            
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            {
                FoV -= fovSpeed;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                FoV += fovSpeed;
            }
        }
        
        // HAVE GRAVITY
        else if (mode == 2)
        {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                position += vec3(direction.x * deltaTime * speed, 0, direction.z * deltaTime * speed);
                
            }
            // Move backward
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                position -= vec3(direction.x * deltaTime * speed, 0, direction.z * deltaTime * speed);
            }
            // Strafe right
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                position += right * deltaTime * speed;
            }
            // Strafe left
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                position -= right * deltaTime * speed;
            }
            
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            {
                FoV -= fovSpeed;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                FoV += fovSpeed;
            }
        }
    }
    projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    viewMatrix = lookAt(
                        position,
                        position + normalize(direction),
                        up);
    //*/
    
    
    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}

vec3 Camera::returnRotation()
{
    // Assuming that 'direction' is the forward vector of the camera
    glm::vec3 forward = normalize(direction);

    // Calculate the avatar's rotation based on the camera's right and up vectors
    float pitch = glm::degrees(asin(-direction.y));
    float yaw = glm::degrees(atan2(direction.x, direction.z));

    return glm::vec3(pitch, yaw, 0.0f); // Assuming no roll for simplicity

}

void Camera::updateBoundingBox()
{
    vec3 cameraSize = vec3(3.0f, 2.8f, 3.0f);
    cameraBox.min = position - cameraSize * 0.5f;
    cameraBox.max = position + cameraSize * 0.5f;
}


void Camera::handleCollision(BoundingBox wallBox)
{
    if (cameraBox.checkCollision(wallBox))
    {
        vec3 collisionNormal = cameraBox.calculateCollisionNormal(wallBox);

        //position -= direction * deltaTime * speed;
        position -= collisionNormal *deltaTime * speed;
        updateBoundingBox();
    }
}

void Camera::setDirection(vec3 directionVector) {
    // Calculate the direction vector from the camera position to the target point
    vec3 targetDirection = normalize(directionVector - position);

    //// Calculate the horizontal angle
    horizontalAngle = atan2(targetDirection.z, targetDirection.x);

    // Calculate the vertical angle
    verticalAngle = asin(targetDirection.y);
    direction = directionVector;
}


void Camera::updatePos(vec3 newPos)
{
    position = newPos;
    updateBoundingBox();
}

void Camera::setProjectionMatrix(float aspect, float near, float far)
{
    projectionMatrix = perspective(radians(FoV), aspect, near, far);
}

void Camera::initializeWithRotation(float rotationAngle)
{
    // Set the initial direction of the camera
    float initialHorizontalAngle = horizontalAngle;
    float initialVerticalAngle = verticalAngle;

    // Apply rotation to the initial direction
    initialHorizontalAngle += radians(rotationAngle);

    // Calculate the initial direction vector
    direction = vec3(
        cos(initialVerticalAngle) * sin(initialHorizontalAngle),
        sin(initialVerticalAngle),
        cos(initialVerticalAngle) * cos(initialHorizontalAngle)
    );

    // Calculate the right and up vectors
    right = vec3(
        sin(initialHorizontalAngle - pi<float>() / 2.0f),
        0,
        cos(initialHorizontalAngle - pi<float>() / 2.0f)
    );
    up = cross(right, direction);

    // Calculate the view matrix
    viewMatrix = lookAt(
        position,
        position + direction,
        up
    );
}
