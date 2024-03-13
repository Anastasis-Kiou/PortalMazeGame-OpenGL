// Include C++ headers
#include <iostream>
#include <string>
#include <stack>
#include <cmath>
#include <random>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
#include <common/light.h>
#include "avatar.h"
#include "BoundingBox.h"
#include "portalgun.h"
#include "Portal.h"

using namespace std;
using namespace glm;

#define PORTAL_WIDTH 512      // 857
#define PORTAL_HEIGHT 512 * 2 // 1080

void initialize();
void createContext();
void mainLoop();
void free();
void GenerateMaze();

#define W_WIDTH 2048
#define W_HEIGHT 768 * 2
#define TITLE "PortalMaze"

// #define M_PI 3.14159
#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

// Creating a structure to store the material parameters of an object
struct Material
{
    vec4 Ka;
    vec4 Kd;
    vec4 Ks;
    float Ns;
};

vector<BoundingBox> wallBox;
vector<vec3> maze;
vector<BoundingBox> portalBox;
BoundingBox exitBox;
char generated[31][31]; // Generate a 31x31 maze

vec3 exitPos;
const float wallSize = 2.5f;
const float mapXSize = 32.0f * wallSize, mapZSize = 32.0f * wallSize;

// Global Variables
GLFWwindow *window;
Camera *camera, *cameraPortal1, *cameraPortal2;    // Main Camera, and the 2 portal Cameras
Light *light;                                      // Light Source
GLuint shaderProgram, depthProgram, portalProgram; // Shader Programs
Drawable *plane;                                   // The floor
GLuint depthFBO;                                   // Framebuffers for the depth program
GLuint depthTexture;
Portal *portal1; // Portals
Portal *portal2;

Avatar *avatarModel; // The player avatar and his gun model
portalGun *portalGunModel;

GLuint texturePlane; // The texture on the wals

Drawable *earth;  // The light source object to draw
Drawable *sphere; // The sphere, representing the exit

vector<vec3> earthVertices, earthNormals;
vector<vec2> earthUVs;

Drawable *cube; // The cube that I use for the walls

float planeY = -1.3; // The ground is in this y axis value
float size = 74.0f;  // The size of the ground

// locations for shaderProgram
// View, Model and Projection Matrixes in Shaders
GLuint viewMatrixLocation;
GLuint projectionMatrixLocation;
GLuint modelMatrixLocation;
// Material and Light properties
GLuint KaLocation, KdLocation, KsLocation, NsLocation;
GLuint LaLocation, LdLocation, LsLocation;
GLuint lightPositionLocation;
GLuint lightPowerLocation;
GLuint lightVPLocation;
// Textures
GLuint diffuseColorSampler;
GLuint specularColorSampler;
GLuint useTextureLocation, usePortalTextureLocation;
GLuint depthMapSampler;

// locations for depthProgram
GLuint shadowViewProjectionLocation;
GLuint shadowModelLocation;

// locations for portalProgram
// locations for shaderProgram
// View, Model and Projection Matrixes in Shaders
GLuint viewMatrixLocationPortal;
GLuint projectionMatrixLocationPortal;
GLuint modelMatrixLocationPortal;
// Textures
GLuint diffuseColorSamplerPortal;
GLuint specularColorSamplerPortal;
GLuint portalTexture;
GLuint doorTexture;

// Portal framebuffers, textures and depth textures for the camera's viewpoint
GLuint portalFBOs[2];
GLuint portalTextures[2];
GLuint portalDepthTextures[2];
int portalExists = 0;

// Some Materials that can be uploaded as a texture to an Object
const Material polishedSilver{
    vec4{0.23125, 0.23125, 0.23125, 1},
    vec4{0.2775, 0.2775, 0.2775, 1},
    vec4{0.773911, 0.773911, 0.773911, 1},
    89.6f};

const Material turquoise{
    vec4{0.1, 0.18725, 0.1745, 0.8},
    vec4{0.396, 0.74151, 0.69102, 0.8},
    vec4{0.297254, 0.30829, 0.306678, 0.8},
    12.8f};

const Material beige{
    vec4{0.64f, 0.58f, 0.5f, 0.8f}, // Ambient color (RGB + alpha)
    vec4{0.64f, 0.58f, 0.5f, 0.8f}, // Diffuse color (RGB + alpha)
    vec4{0.5f, 0.5f, 0.5f, 0.8f},   // Specular color (RGB + alpha)
    12.8f                           // Shininess
};

const Material mossyRocksMaterial{
    vec4{0.2f, 0.2f, 0.2f, 1.0f}, // Ambient
    vec4{0.4f, 0.6f, 0.3f, 1.0f}, // Diffuse (greenish color for moss)
    vec4{0.1f, 0.1f, 0.1f, 1.0f}, // Specular (adjust as needed)
    32.0f                         // Shininess (adjust as needed)
};

const Material bloodRedMaterial{
    vec4{0.3f, 0.2f, 0.2f, 1.0f}, // Ambient color
    vec4{0.5f, 0.3f, 0.3f, 1.0f}, // Diffuse color
    vec4{0.6f, 0.4f, 0.4f, 1.0f}, // Specular color
    50.0f                         // Shininess
};

const Material bloodRedMaterial2{
    vec4{0.2, 0.0, 0.0, 1.0}, // Ambient color (Ka)
    vec4{0.8, 0.0, 0.0, 1.0}, // Diffuse color (Kd)
    vec4{0.5, 0.0, 0.0, 1.0}, // Specular color (Ks)
    10.0f                     // Shininess (Ns)
};

// Calculating Aspect Ratio for the portal Camera
float calculateAspectRatio(int textureWidth, int textureHeight)
{
    // Ensure that both width and height are non-zero to avoid division by zero
    if (textureHeight != 0)
    {
        // Calculate and return the aspect ratio
        return static_cast<float>(textureWidth) / static_cast<float>(textureHeight);
    }
    else
    {
        // Return a default aspect ratio or handle the error as appropriate for your application
        return 1.0f; // Default aspect ratio (1:1)
    }
}

// Global Variable aspectRatio
float aspectRatio = calculateAspectRatio(PORTAL_WIDTH, PORTAL_HEIGHT);

// Creating a function to upload (make uniform) the light parameters to the shader program
void uploadLight(const Light &light)
{
    glUniform4f(LaLocation, light.La.r, light.La.g, light.La.b, light.La.a);
    glUniform4f(LdLocation, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
    glUniform4f(LsLocation, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
    glUniform3f(lightPositionLocation, light.lightPosition_worldspace.x,
                light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
    glUniform1f(lightPowerLocation, light.power);
}

// Creating a function to upload the material parameters of a model to the shader program
void uploadMaterial(const Material &mtl)
{
    glUniform4f(KaLocation, mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
    glUniform4f(KdLocation, mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
    glUniform4f(KsLocation, mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
    glUniform1f(NsLocation, mtl.Ns);
}

// Function that creates the basic uniform variables, shader programs, drawables and framebuffers/textures
void createContext()
{
    // The main Program
    shaderProgram = loadShaders("ShadowMapping.vertexshader", "ShadowMapping.fragmentshader");

    // Depth Program
    depthProgram = loadShaders("Depth.vertexshader", "Depth.fragmentshader");

    portalProgram = loadShaders("portal.vertexshader", "portal.fragmentshader");

    // Load the sphere that will represent exit and light source
    loadOBJWithTiny("earth.obj", earthVertices, earthUVs, earthNormals);

    // Get pointers to uniforms
    // --- shaderProgram ---
    projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
    viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
    modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");
    // for phong lighting
    KaLocation = glGetUniformLocation(shaderProgram, "mtl.Ka");
    KdLocation = glGetUniformLocation(shaderProgram, "mtl.Kd");
    KsLocation = glGetUniformLocation(shaderProgram, "mtl.Ks");
    NsLocation = glGetUniformLocation(shaderProgram, "mtl.Ns");
    LaLocation = glGetUniformLocation(shaderProgram, "light.La");
    LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
    LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
    lightPositionLocation = glGetUniformLocation(shaderProgram, "light.lightPosition_worldspace");
    lightPowerLocation = glGetUniformLocation(shaderProgram, "light.power");

    diffuseColorSampler = glGetUniformLocation(shaderProgram, "diffuseColorSampler");
    specularColorSampler = glGetUniformLocation(shaderProgram, "specularColorSampler");

    //-----portalShaderProgram------//
    projectionMatrixLocationPortal = glGetUniformLocation(portalProgram, "P");
    viewMatrixLocationPortal = glGetUniformLocation(portalProgram, "V");
    modelMatrixLocationPortal = glGetUniformLocation(portalProgram, "M");
    // for phong lighting

    diffuseColorSamplerPortal = glGetUniformLocation(portalProgram, "diffuseColorSampler");
    specularColorSamplerPortal = glGetUniformLocation(portalProgram, "specularColorSampler");

    // Texture Enabler
    useTextureLocation = glGetUniformLocation(shaderProgram, "useTexture");

    usePortalTextureLocation = glGetUniformLocation(shaderProgram, "usePortalTexture");

    // locations for shadow rendering
    depthMapSampler = glGetUniformLocation(shaderProgram, "shadowMapSampler");

    lightVPLocation = glGetUniformLocation(shaderProgram, "lightVP");

    // --- depthProgram ---
    shadowViewProjectionLocation = glGetUniformLocation(depthProgram, "VP");
    shadowModelLocation = glGetUniformLocation(depthProgram, "M");

    // Load the Wall Texture
    texturePlane = loadBMP("mossywall.bmp");

    // Load the Wall Object
    cube = new Drawable("cube.obj");

    // Create the Floor (Plane)
    vector<vec3> floorVertices = {
        vec3(-size - 2, planeY, -size - 2),
        vec3(-size - 2, planeY, size),
        vec3(size, planeY, size),
        vec3(size, planeY, size),          //
        vec3(size, planeY, -size - 2),     //
        vec3(-size - 2, planeY, -size - 2) //
    };

    // plane normals
    vector<vec3> floorNormals = {
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f)};

    // plane uvs
    vector<vec2> floorUVs = {
        vec2(0.0f, 0.0f),
        vec2(0.0f, 1.0f),
        vec2(1.0f, 1.0f),
        vec2(1.0f, 1.0f),
        vec2(1.0f, 0.0f),
        vec2(0.0f, 0.0f),
    };

    plane = new Drawable(floorVertices, floorUVs, floorNormals);

    // Generate the framebuffers
    glGenFramebuffers(1, &depthFBO);
    // Binding the framebuffer, all changes bellow will affect the binded framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

    // We need a texture to store the depth image
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // Set color to set out of border
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Check if the framebuffer is OK
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        glfwTerminate();
        throw runtime_error("Frame buffer not initialized correctly");
    }

    //-----PORTAL FRAMEBUFFERS --------//
    for (int i = 0; i < 2; ++i)
    {
        // Create framebuffer
        glGenFramebuffers(1, &portalFBOs[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, portalFBOs[i]);

        // Create texture
        glGenTextures(1, &portalTextures[i]);
        glBindTexture(GL_TEXTURE_2D, portalTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, PORTAL_WIDTH, PORTAL_HEIGHT, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Create and attach trxture for depth
        glGenTextures(1, &portalDepthTextures[i]);
        glBindTexture(GL_TEXTURE_2D, portalDepthTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, PORTAL_WIDTH, PORTAL_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        // Attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, portalDepthTextures[i], 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, portalTextures[i], 0);

        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            throw std::runtime_error("Frame buffer not initialized correctly");
        }
    }

    // Binding the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Inversing the sphere Normals so that the earth object reflects light as a light source
    vector<vec3> newEarthNormals;
    for (size_t i = 0; i < earthNormals.size(); i++)
    {
        vec3 newNormal = -earthNormals[i];
        newEarthNormals.push_back(newNormal);
    }
    // Exit Object
    sphere = new Drawable(earthVertices, earthUVs, earthNormals);

    // Light Source
    earth = new Drawable(earthVertices, earthUVs, newEarthNormals);
}

void free()
{
    // Delete Shader Programs
    glDeleteProgram(shaderProgram);
    glDeleteProgram(depthProgram);
    glDeleteProgram(portalProgram);

    glfwTerminate();
}

void depth_pass(mat4 viewMatrix, mat4 projectionMatrix)
{

    // Setting viewport to shadow map size
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    // Binding the depth framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

    // Cleaning the framebuffer depth information (stored from the last render)
    glClear(GL_DEPTH_BUFFER_BIT);

    // Selecting the new shader program that will output the depth component
    glUseProgram(depthProgram);

    // sending the view and projection matrix to the shader
    mat4 view_projection = projectionMatrix * viewMatrix;
    glUniformMatrix4fv(shadowViewProjectionLocation, 1, GL_FALSE, &view_projection[0][0]);

    // Bind the walls to the depth program
    mat4 cellModelMatrix;
    mat4 scaleMatrix = scale(mat4(1.0f), vec3(wallSize, 3.0f, wallSize));

    cube->bind();
    for (auto w : maze)
    {
        cellModelMatrix = translate(mat4(), w) * scaleMatrix;
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &cellModelMatrix[0][0]);
        cube->draw();
    }

    // same for sphere
    mat4 exitModelMatrix;
    scaleMatrix = scale(mat4(1.0f), vec3(0.6));
    uploadMaterial(bloodRedMaterial2);
    glUniform1i(useTextureLocation, 0);
    exitModelMatrix = translate(mat4(1.0f), exitPos);
    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &exitModelMatrix[0][0]);
    sphere->bind();
    sphere->draw();
    // same for plane
    mat4 modelMatrix = mat4(1.0f);
    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
    plane->bind();
    plane->draw();

    //----------AVATAR + GUN------------//
    avatarModel->update(camera);
    vec3 position = camera->position;
    mat4 avatarModelMatrix = avatarModel->modelMatrix;
    scaleMatrix = scale(mat4(1.0f), vec3(0.2));
    vec3 directionNow = camera->direction;
    vec3 zAlways = vec3(0, 0, 1);
    vec3 rotationAxis = cross(zAlways, directionNow);

    // Calculate the angle between the current direction and the original direction the avatar was facing
    float angleZ = acos(dot(directionNow, zAlways));

    angleZ = angleZ * (180.0f / M_PI);
    float angleGun = angleZ;

    // Check if the angle is + or - using a reference y axis
    vec3 referenceVector = vec3(0, 1, 0);
    float dotProduct = dot(rotationAxis, referenceVector);
    if (dotProduct < 0.0f)
    {
        angleZ = -angleZ; // Reverse the angle if the rotation is counterclockwise
    }

    avatarModelMatrix = rotate(avatarModelMatrix, radians(angleZ), vec3(0, camera->position.y, 0));

    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &avatarModelMatrix[0][0]);
    avatarModel->draw();
    portalGunModel->update(avatarModel);
    mat4 gunModelMatrix = portalGunModel->modelMatrix;
    vec3 gunFinalPosition = vec3(avatarModel->position.x + 0.8 * cos(radians((angleZ))), avatarModel->position.y + 1.6, avatarModel->position.z - 0.8 * sin(radians(angleZ)));
    scaleMatrix = scale(mat4(1.0f), vec3(0.005));
    gunModelMatrix = translate(gunModelMatrix, gunFinalPosition) * scaleMatrix;
    // rotate the gun based on the avatar position
    gunModelMatrix = rotate(gunModelMatrix, radians(90.0f), vec3(0, 1, 0));
    gunModelMatrix = rotate(gunModelMatrix, radians(angleZ), vec3(0, camera->position.y, 0));

    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &gunModelMatrix[0][0]);
    portalGunModel->draw();

    // binding the default framebuffer again
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void portal_pass(int i, mat4 viewMatrix, mat4 projectionMatrix)
{
    // Binding the portal framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, portalFBOs[i]);

    // Cleaning the portal framebuffer color and depth information
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Selecting the shader program for portal rendering
    glUseProgram(shaderProgram);

    // Sending the view and projection matrix to the portal shader
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(depthMapSampler, 0);
    uploadLight(*light);
    mat4 lightVP = light->lightVP();

    glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &lightVP[0][0]);

    // ----------------------------------------------------------------- //
    // --------------------- Drawing scene objects --------------------- //
    // ----------------------------------------------------------------- //

    //-------------FLOOR--------------//
    // Upload Floor Material
    uploadMaterial(bloodRedMaterial);

    // upload the model matrix
    mat4 planeModelMatrix = mat4();
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &planeModelMatrix[0][0]);

    glUniform1i(useTextureLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texturePlane);
    glUniform1i(diffuseColorSampler, 1);
    plane->bind();
    plane->draw();

    //--------------WALLS-------------//
    mat4 cellModelMatrix;
    mat4 scaleMatrix = scale(mat4(1.0f), vec3(wallSize, 3.0f, wallSize));
    // Upload Wall Texture
    uploadMaterial(mossyRocksMaterial);
    glUniform1i(useTextureLocation, 1);
    cube->bind();
    for (auto w : maze)
    {
        cellModelMatrix = translate(mat4(), w) * scaleMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &cellModelMatrix[0][0]);
        cube->draw();
    }

    //--------EXIT------------//
    mat4 exitModelMatrix;
    scaleMatrix = scale(mat4(1.0f), vec3(0.6));
    uploadMaterial(bloodRedMaterial2);
    glUniform1i(useTextureLocation, 0);
    exitModelMatrix = translate(mat4(1.0f), exitPos);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &exitModelMatrix[0][0]);
    sphere->bind();
    sphere->draw();

    //----------AVATAR + GUN------------//
    avatarModel->update(camera);
    vec3 position = camera->position;
    mat4 avatarModelMatrix = avatarModel->modelMatrix;
    scaleMatrix = scale(mat4(1.0f), vec3(0.2));
    vec3 directionNow = camera->direction;
    vec3 zAlways = vec3(0, 0, 1);
    vec3 rotationAxis = cross(zAlways, directionNow);

    // Calculate the angle between the current direction and the original direction the avatar was facing
    float angleZ = acos(dot(directionNow, zAlways));

    angleZ = angleZ * (180.0f / M_PI);
    float angleGun = angleZ;

    // Check if the angle is + or - using a reference y axis
    vec3 referenceVector = vec3(0, 1, 0);
    float dotProduct = dot(rotationAxis, referenceVector);
    if (dotProduct < 0.0f)
    {
        angleZ = -angleZ; // Reverse the angle if the rotation is counterclockwise
    }

    avatarModelMatrix = rotate(avatarModelMatrix, radians(angleZ), vec3(0, camera->position.y, 0));

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &avatarModelMatrix[0][0]);
    avatarModel->draw();
    portalGunModel->update(avatarModel);
    mat4 gunModelMatrix = portalGunModel->modelMatrix;
    vec3 gunFinalPosition = vec3(avatarModel->position.x + 0.8 * cos(radians((angleZ))), avatarModel->position.y + 1.6, avatarModel->position.z - 0.8 * sin(radians(angleZ)));
    scaleMatrix = scale(mat4(1.0f), vec3(0.005));
    gunModelMatrix = translate(gunModelMatrix, gunFinalPosition) * scaleMatrix;
    // rotate the gun based on the avatar position
    gunModelMatrix = rotate(gunModelMatrix, radians(90.0f), vec3(0, 1, 0));
    gunModelMatrix = rotate(gunModelMatrix, radians(angleZ), vec3(0, camera->position.y, 0));

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &gunModelMatrix[0][0]);
    portalGunModel->draw();

    //----------LIGHT SOURCE-----------//
    mat4 earth1ModelMatrix = translate(mat4(), light->lightPosition_worldspace);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &earth1ModelMatrix[0][0]);

    earth->bind();
    earth->draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix)
{

    // Bind a frameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, W_WIDTH, W_HEIGHT);

    // Clearing color and depdepth info
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Selecting shader program
    glUseProgram(shaderProgram);
    glUniform1i(usePortalTextureLocation, 0);

    // Making view and projection matrices uniform to the shader program
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    // uploading the light parameters to the shader program

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(depthMapSampler, 0);
    uploadLight(*light);
    mat4 lightVP = light->lightVP();

    glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &lightVP[0][0]);

    // ----------------------------------------------------------------- //
    // --------------------- Drawing scene objects --------------------- //
    // ----------------------------------------------------------------- //

    //-------------FLOOR--------------//
    // Upload Floor Material
    uploadMaterial(bloodRedMaterial);

    // upload the model matrix
    mat4 planeModelMatrix = mat4();
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &planeModelMatrix[0][0]);

    glUniform1i(useTextureLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texturePlane);
    glUniform1i(diffuseColorSampler, 1);
    plane->bind();
    plane->draw();

    //--------------WALLS-------------//
    mat4 cellModelMatrix;
    mat4 scaleMatrix = scale(mat4(1.0f), vec3(wallSize, 3.0f, wallSize));
    // Upload Wall Texture
    // uploadMaterial(mossyRocksMaterial);
    glUniform1i(useTextureLocation, 1);
    cube->bind();
    for (auto w : maze)
    {
        cellModelMatrix = translate(mat4(), w) * scaleMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &cellModelMatrix[0][0]);
        cube->draw();
    }

    //--------EXIT------------//
    mat4 exitModelMatrix;
    scaleMatrix = scale(mat4(1.0f), vec3(0.02));
    uploadMaterial(bloodRedMaterial2);
    glUniform1i(useTextureLocation, 0);
    exitModelMatrix = translate(mat4(1.0f), exitPos);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &exitModelMatrix[0][0]);
    sphere->bind();
    sphere->draw();
    if (exitBox.checkCollision(camera->cameraBox))
    {
        camera->updatePos(exitPos - vec3(12.0f, 0.0f, 0.0f));
    }

    //----------AVATAR + GUN------------//
    avatarModel->update(camera);
    vec3 position = camera->position;
    mat4 avatarModelMatrix = avatarModel->modelMatrix;
    scaleMatrix = scale(mat4(1.0f), vec3(0.2));
    vec3 directionNow = camera->direction;
    vec3 zAlways = vec3(0, 0, 1);
    vec3 rotationAxis = cross(zAlways, directionNow);

    // Calculate the angle between the current direction and the original direction the avatar was facing
    float angleZ = acos(dot(directionNow, zAlways));

    angleZ = angleZ * (180.0f / M_PI);
    float angleGun = angleZ;

    // Check if the angle is + or - using a reference y axis
    vec3 referenceVector = vec3(0, 1, 0);
    float dotProduct = dot(rotationAxis, referenceVector);
    if (dotProduct < 0.0f)
    {
        angleZ = -angleZ; // Reverse the angle if the rotation is counterclockwise
    }

    avatarModelMatrix = rotate(avatarModelMatrix, radians(angleZ), vec3(0, camera->position.y, 0));

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &avatarModelMatrix[0][0]);
    avatarModel->draw();
    portalGunModel->update(avatarModel);
    mat4 gunModelMatrix = portalGunModel->modelMatrix;
    vec3 gunFinalPosition = vec3(avatarModel->position.x + 0.8 * cos(radians((angleZ))), avatarModel->position.y + 1.6, avatarModel->position.z - 0.8 * sin(radians(angleZ)));
    scaleMatrix = scale(mat4(1.0f), vec3(0.005));
    gunModelMatrix = translate(gunModelMatrix, gunFinalPosition) * scaleMatrix;
    // Rotate the gun based on the avatar position
    gunModelMatrix = rotate(gunModelMatrix, radians(90.0f), vec3(0, 1, 0));
    gunModelMatrix = rotate(gunModelMatrix, radians(angleZ), vec3(0, camera->position.y, 0));

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &gunModelMatrix[0][0]);
    portalGunModel->draw();

    //----------LIGHT SOURCE-----------//
    uploadMaterial(beige);
    glUniform1i(useTextureLocation, 0);

    mat4 earth1ModelMatrix = translate(mat4(), light->lightPosition_worldspace);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &earth1ModelMatrix[0][0]);

    earth->bind();
    earth->draw();

    //-------------PORTAL-------------//
    glDisable(GL_CULL_FACE);

    mat4 cameraPortal2View;
    mat4 cameraPortal2Proj;
    vec3 portalNormal;
    // Use portal Program
    glUseProgram(portalProgram);

    glUniformMatrix4fv(viewMatrixLocationPortal, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionMatrixLocationPortal, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Activate the texture, which holds the camera's viewpoint
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, portalTextures[0]);
    glUniform1i(diffuseColorSamplerPortal, 3);
    portal1->update(angleZ);
    mat4 portalModelMatrix1 = portal1->portalModelMatrix;
    glUniform1i(useTextureLocation, 1);
    glUniform1i(usePortalTextureLocation, 1);
    glUniformMatrix4fv(modelMatrixLocationPortal, 1, GL_FALSE, &portalModelMatrix1[0][0]);
    portal1->draw();
    glUniform1i(usePortalTextureLocation, 0);

    vec3 portalNormal2 = portal2->getNormal(); // Get the normal vector of the portal
    // cout << portal2->portalPosition.x << ", " << portal2->portalPosition.y << ", " << portal2->portalPosition.z << "\n";
    cameraPortal1->position = portal2->portalPosition + vec3(0.0f, 1.5f, 0.0f);
    // Set the Direction the Camera is looking
    cameraPortal1->setDirection(vec3(0.625f, 0.1f, -0.999999f));

    // cout << portalNormal2.x << ", " << portalNormal2.y << ", " << portalNormal2.z << "\n";

    // Update the portal Camera
    cameraPortal1->update();

    mat4 cameraPortal1View = cameraPortal1->viewMatrix;
    mat4 cameraPortal1Proj = cameraPortal1->projectionMatrix;
    // Create the Bounding Box for the portal that will be used for teleportation
    BoundingBox portal1Box(portal1->portalPosition - vec3(1.5f, 3.0f, 0.0f) * 0.5f * 1.3f, portal1->portalPosition + vec3(1.5f, 3.0f, 0.0f) * 0.5f * 1.3f);

    // Update the existing portals number
    int portalExistance = portal1->mode;
    portalExists = portalExistance;
    mat4 portalModelMatrix2 = mat4();

    // If the first portal exists, create the second one with the same routine
    if (portalExistance == 1)
    {
        portal2->update(angleZ);
        portalModelMatrix2 = portal2->portalModelMatrix;
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, portalTextures[1]);
        glUniform1i(diffuseColorSamplerPortal, 4);

        portalNormal = portal1->getNormal(); // Get the normal vector of the portal
        glUniformMatrix4fv(modelMatrixLocationPortal, 1, GL_FALSE, &portalModelMatrix2[0][0]);
        glUniform1i(usePortalTextureLocation, 1);

        portal2->draw();
        glUniform1i(usePortalTextureLocation, 0);

        cameraPortal2->position = portal1->portalPosition + vec3(0.0f, 1.5f, 0.0f);

        cameraPortal2->setDirection(vec3(0.625f, 0.1f, -0.999999f));
        cameraPortal2->update();

        // cout << portalNormal.x << ", " << portalNormal.y << ", " << portalNormal.z << "\n";

        cameraPortal2View = cameraPortal2->viewMatrix;
        cameraPortal2Proj = cameraPortal2->projectionMatrix;
    }
    glEnable(GL_CULL_FACE);
    BoundingBox portal2Box(portal2->portalPosition - vec3(1.5f, 3.0f, 0.0f) * 0.5f * 1.3f, portal2->portalPosition + vec3(1.5f, 3.0f, 0.0f) * 0.5f * 1.3f);

    glUniform1i(usePortalTextureLocation, 0);
    // If both portals exist, render the scene for each portal camera, pass it to the other portal, and check for teleportation
    if (portal1->mode == 1 && portal2->mode == 1)
    {
        portal1->adjustUv(aspectRatio);
        portal2->adjustUv(aspectRatio);

        portal_pass(0, cameraPortal1View, cameraPortal1Proj);
        portal_pass(1, cameraPortal2View, cameraPortal2Proj);
        // Change the uvs and aspect Ratio of the portal texture
        // cout << "Portal1: " << portal1->portalPosition.x << ", " << portal1->portalPosition.y << ", " << portal1->portalPosition.z << "\n";
        if (portal1Box.checkCollision(camera->cameraBox))
        {
            camera->updatePos(portal2->portalPosition - 3.0f * portalNormal);
        }
        if (portal2Box.checkCollision(camera->cameraBox))
        {
            camera->updatePos(portal1->portalPosition - 3.0f * portalNormal2);
        }
    }
}

void mainLoop()
{
    // Generate the Maze
    GenerateMaze();
    // Create the boundingBox for the exit
    exitBox = BoundingBox(exitPos - vec3(0.5f), exitPos + vec3(0.5f));
    // Update the lightsource once
    light->update();
    mat4 light_proj = light->projectionMatrix;
    mat4 light_view = light->viewMatrix;

    // Enable depth program
    glCullFace(GL_FRONT);
    depth_pass(light_view, light_proj);
    glCullFace(GL_BACK);

    vec3 initialDir;
    vec3 currentDir;

    // Paint the Sky a deep purple
    glClearColor(0.2f, 0.0f, 0.3f, 1.0f);

    do
    {
        light->update();
        light_proj = light->projectionMatrix;
        light_view = light->viewMatrix;
        // Create Depth Buffer
        glCullFace(GL_FRONT);
        depth_pass(light_view, light_proj);
        glCullFace(GL_BACK);

        // Getting camera information
        camera->update();
        camera->updateBoundingBox();

        mat4 projectionMatrix = camera->projectionMatrix;
        mat4 viewMatrix = camera->viewMatrix;
        // cout << camera->direction.x << ", " << camera->direction.y << ", " << camera->direction.z << "\n";

        // Check for collision with walls
        for (int i = 0; i < wallBox.size(); i++)
        {
            camera->handleCollision(wallBox[i]);
        }
        // Call the function to implement lighting and draw the objects
        lighting_pass(viewMatrix, projectionMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
}

void GenerateMaze()
{
    // Generate Random Directions because rand() % 4 was not random enough
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dirDistribution(0, 3);
    BoundingBox wallBoundingBox;

    maze.clear();

    for (size_t i = 0; i < 31; i++)
        for (size_t j = 0; j < 31; j++)
            generated[i][j] = '#';

    stack<pair<int, int>> frontier; // Current cell the generator is working on

    stack<pair<int, int>> path; // Path that the generator has taken

    // Start generating from 29, 15
    frontier.push({29, 15});
    path.push({29, 15});

    while (!path.empty())
    {
        pair<int, int> cell = frontier.top();
        frontier.pop();
        // Check if the cell is inside the map
        if (cell.first < 0 || cell.first > 30 || cell.second < 0 || cell.second > 30)
            continue;

        // Choose a Random Direction
        int dir = dirDistribution(gen);

        // Choose the cell that the generator will check next
        for (size_t i = 0; i < 4; i++)
        {
            pair<int, int> cell2;

            switch (dir)
            {
            // Move UP
            case 0:
                cell2 = {cell.first - 2, cell.second};
                break;
            // Move Down
            case 1:
                cell2 = {cell.first + 2, cell.second};
                break;
            // Move Left
            case 2:
                cell2 = {cell.first, cell.second - 2};
                break;
            // Move Right
            case 3:
                cell2 = {cell.first, cell.second + 2};
                break;
            }

            // Check that the following cell is within bounds
            if (cell2.first >= 0 && cell2.first <= 30 && cell2.second >= 0 && cell2.second <= 30)
            {
                // Check if we have previously visited the same cell
                if (generated[cell2.first][cell2.second] == '#')
                {
                    // Mark it as visited
                    generated[cell2.first][cell2.second] = '.';

                    // Create a passage to that cell
                    generated[(cell.first + cell2.first) / 2][(cell.second + cell2.second) / 2] = '.';

                    // Add cell2 to the frontier and path
                    frontier.push(cell2);
                    path.push(cell2);
                    break;
                }
            }
            // If the cell that was chosen was invalid, increment direction by 1 and try again
            dir++;
            dir = dir % 4; // Normalize in range (0,3)
        }
        // If we hit a DEAD END
        if (frontier.empty())
        {
            // Store if the generator found a cell that it has already visited
            bool found = false;
            while (!path.empty() && !found)
            {
                pair<int, int> c = path.top();
                path.pop();

                // Choose a random direction to go to
                int direction = dirDistribution(gen);

                for (size_t i = 0; i < 4; i++)
                {
                    pair<int, int> c2;
                    switch (direction)
                    {
                    case 0:
                        c2 = {c.first - 2, c.second};
                        break;
                    case 1:
                        c2 = {c.first + 2, c.second};
                        break;
                    case 2:
                        c2 = {c.first, c.second - 2};
                        break;
                    case 3:
                        c2 = {c.first, c.second + 2};
                        break;
                    }

                    // Make sure that the cell we are going is withing bounds
                    if (c2.first >= 0 && c2.first <= 30 && c2.second >= 0 && c2.second <= 30)
                    {
                        // Check if it has been visited
                        if (generated[c2.first][c2.second] == '#')
                        {
                            generated[c2.first][c2.second] = '.';
                            generated[(c.first + c2.first) / 2][(c.second + c2.second) / 2] = '.';

                            frontier.push(c2);
                            path.push(c2);
                            found = true;
                            break;
                        }
                    }

                    direction++;
                    direction = direction % 4;
                }
            }
        }
    }

    generated[29][15] = '@'; // Player Position

    generated[1][15] = 'E'; // Mark the exit in the maze data

    // Read the maze data
    for (int i = 0; i < 31; i++)
    {
        for (int j = 0; j < 31; j++)
        {
            switch (generated[i][j])
            {
                // Wall
            case '#':
                maze.push_back(vec3(wallSize * 2.0f * i - 31.0f * wallSize, 0.5f, wallSize * 2.0f * j - 31.0f * wallSize));
                break;
            // Exit
            case 'E':
                exitPos = vec3(wallSize * 2.0f * i - 31.0f * wallSize, 2.0f, wallSize * 2.0f * j - 31.0f * wallSize);
                break;
            // Player
            case '@':
                camera->position = vec3(wallSize * 2.0f * i - 31.0f * wallSize, 0.0f, wallSize * 2.0f * j - 31.0f * wallSize);
                break;
            }
        }
    }

    // Generate BoundingBoxes for each wall
    for (auto w : maze)
    {
        BoundingBox wallBoundingBox(w - vec3(wallSize, 1.5f, wallSize) * 0.5f * 1.5f,
                                    w + vec3(wallSize, 1.5f, wallSize) * 0.5f * 1.5f);

        wallBox.push_back(wallBoundingBox);
    }
}

void initialize()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(W_WIDTH / 2, W_HEIGHT / 2, TITLE, NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
                                   " If you have an Intel GPU, they are not 3.3 compatible." +
                                   "Try the 2.1 version.\n"));
    }
    glfwMakeContextCurrent(window);

    // Start GLEW extension handler
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, W_WIDTH / 4, W_HEIGHT / 4);

    // Gray background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // enable texturing and bind the depth texture
    glEnable(GL_TEXTURE_2D);

    // Log
    logGLParameters();

    // Create camera
    camera = new Camera(window);
    camera->doesMove = true;

    // Create the first portal Camera
    cameraPortal1 = new Camera(window);
    cameraPortal1->FoV = 50.0f;
    cameraPortal1->doesMove = false;

    // Create the second portal Camera
    cameraPortal2 = new Camera(window);
    cameraPortal2->FoV = 50.0f;
    cameraPortal2->doesMove = false;

    // Change their projection Matrixes to better fit the portal
    cameraPortal1->setProjectionMatrix(aspectRatio, 2.0f, 50.0f);
    cameraPortal2->setProjectionMatrix(aspectRatio, 2.0f, 50.0f);

    // Create the avatar
    avatarModel = new Avatar(camera);

    // Create the gun model
    portalGunModel = new portalGun(avatarModel);

    // Create the portals
    portal1 = new Portal(window, avatarModel, 1);
    portal2 = new Portal(window, avatarModel, 2);

    // Creating a custom light
    light = new Light(window,
                      vec4{1, 1, 1, 1},
                      vec4{1, 1, 1, 1},
                      vec4{1, 1, 1, 1},
                      vec3{0, 60, 0},
                      100.0f);
}

int main(void)
{
    try
    {
        initialize();
        createContext();
        mainLoop();
        free();
    }
    catch (exception &ex)
    {
        cout << ex.what() << endl;
        getchar();
        free();
        return -1;
    }

    return 0;
}
