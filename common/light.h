#include <glm/glm.hpp>

class Light
{
public:
    GLFWwindow *window;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 lightPosition_worldspace;

    glm::vec4 La;
    glm::vec4 Ld;
    glm::vec4 Ls;

    float power;

    float nearPlane;
    float farPlane;

    float lightSpeed;
    glm::vec3 direction;
    
    bool orthoProj;

    glm::vec3 targetPosition;

    Light(GLFWwindow *window,
          glm::vec4 init_La,
          glm::vec4 init_Ld,
          glm::vec4 init_Ls,
          glm::vec3 init_position,
          float init_power);

    void update();
    glm::mat4 lightVP();
};
