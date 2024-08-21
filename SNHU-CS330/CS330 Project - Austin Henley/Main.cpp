/*
 * OpenGL Code for Module 6-5 Milestone
 *
 * Name: Austin Henley
 * Date: 06/10/2023
 *
 * Structure sourced by CS330 Tutorial and rewritten for this assignment
 *
 * 5/27 : Header file edited to allow up and down movements and camera control added
 * 6/2  : stb image added to allow textures
 * 6/10 : Sourced new shaders for lighting abilities, Implemented new lights into scene
 * 6/10 : Added objects to scene and implemented code for creating cube and full sphere - added 2 items to scene
 * 6/17 : Finished scene, seperated shaders and shape creation functions into their own files
 *      : Created directional light to represent sun from nearby window
 *
 */

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "shapes.h"
#include "shader_source.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

using namespace std;

// Unnamed namespace for organization
namespace {
    const char* const WINDOW_TITLE = "Module 6 - Milestone - Austin Henley";
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Meshes for objects
    GLMesh gTorusMesh;
    GLMesh gHalfSphereMesh;
    GLMesh gCylinderMesh;
    GLMesh gPlaneMesh;
    GLMesh gFidgetMesh;
    GLMesh g3DSMesh;
    GLMesh gLightSphereMesh;
    GLMesh gCarmexCapMesh;
    GLMesh gCarmexTubeMesh;
    GLMesh gCarmexBottomMesh;

    GLFWwindow* gWindow = nullptr;

    GLfloat defaultColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Textures for objects
    GLuint gTextureYellow;
    GLuint gTextureSmiley;
    GLuint gTextureCounter;
    GLuint gTextureFidget;
    GLuint gTextureCarmex;
    GLuint gTexture3DS;
    glm::vec2 gUVScale(1.0f, 1.0f);

    // Shader Programs
    GLuint gProgramId;
    GLuint gLightProgramId;

    // global variable for color
    GLint objectColorLoc;

    // variable for changing to ortho view
    bool orthoView = false;

    // camera
    Camera gCamera(glm::vec3(0.0f, 2.0f, 10.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;
}

/* Function prototypes for user-defined functions:
 * - UInitialize: Initializes the program and creates the window.
 * - UResizeWindow: Handles resizing of the window.
 * - UProcessInput: Processes user input.
 * - URender: Renders the graphics on the screen.
 * - UCreateShaderProgram: Creates the shader program from vertex and fragment shader sources.
 * - UDestroyShaderProgram: Cleans up the shader program.
 * - UMousePositionCallback: function that activates when mouse is moved - allowing response to the movement
 * - UMouseScrollCallback: function that activates when scroll wheel is moved - allowing response to movement
 */

bool UInitialize(int, char* [], GLFWwindow** window); //Initialize application and create
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void URender();
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
bool UCreateTexture(const char* filename, GLuint& textureId, bool clamp, bool flip);
void UDestroyTexture(GLuint textureId);
void flipImageVertically(unsigned char* image, int width, int height, int channels);

int main(int argc, char* argv[]) {


    // Initialize window
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;


    // Create shader program
    if (!UCreateShaderProgram(lightVertexShaderSource, lightFragmentShaderSource, gLightProgramId))
        return EXIT_FAILURE;

    // Create shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Load textures for yellow color
    const char* texFilename = "resources/textures/YellowMetal.jpg";
    if (!UCreateTexture(texFilename, gTextureYellow, true, false))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Load textures for smiley face
    texFilename = "resources/textures/Smiley.png";
    if (!UCreateTexture(texFilename, gTextureSmiley, true, false))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Load textures for background plane
    texFilename = "resources/textures/Counter.jpg";
    if (!UCreateTexture(texFilename, gTextureCounter, true, false))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Load textures for fidget cube
    texFilename = "resources/textures/GreenPlastic.jpg";
    if (!UCreateTexture(texFilename, gTextureFidget, true, false))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Load textures for fidget cube
    texFilename = "resources/textures/Carmex.png";
    if (!UCreateTexture(texFilename, gTextureCarmex, true, false))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Load textures for fidget cube
    texFilename = "resources/textures/3DSTexture.jpg";
    if (!UCreateTexture(texFilename, gTexture3DS, true, false))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gProgramId, "uTextureBase"), 0);
    // We set the texture as texture unit 1
    glUniform1i(glGetUniformLocation(gProgramId, "uTextureExtra"), 1);

    // Assign objectColorLoc globally
    objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");

    // Change background color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    createPlane(gPlaneMesh, 15.0f, 15.0f); // Create plane mesh
    createHalfSphere(gHalfSphereMesh, 1.0, 32); // Create Sphere Mesh
    createTorus(gTorusMesh, 1.0, 0.3, 128, 64); // Create Torus Mesh
    createCylinder(gCylinderMesh, 0.2f, 64, 1.0f); // Create cylinder mesh - using silver color set before
    createCylinder(gCarmexTubeMesh, 0.2f, 64, 1.3f);
    createCylinder(gCarmexCapMesh, 0.2f, 64, 0.3f);
    createCylinder(gCarmexBottomMesh, 0.2f, 64, 0.01f);
    createCube(gFidgetMesh, 1.0, 1.0, 1.0); // Create fidget cube mesh
    createCube(g3DSMesh, 5.0, 0.25, 3.0); // Create 3DS cube mesh
    createFullSphere(gLightSphereMesh, 1.0, 32); // Create mesh for light spheres


    // Render loop
    while (!glfwWindowShouldClose(gWindow)) {

        // per-frame timing
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;


        // Input
        UProcessInput(gWindow);

        // Render each frame
        URender();
        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gTorusMesh);
    UDestroyMesh(gHalfSphereMesh);
    UDestroyMesh(gCylinderMesh);
    UDestroyMesh(gPlaneMesh);
    UDestroyMesh(gFidgetMesh);
    UDestroyMesh(g3DSMesh);
    UDestroyMesh(gLightSphereMesh);

    // Release Texture Data
    UDestroyTexture(gTextureYellow);
    UDestroyTexture(gTextureSmiley);
    UDestroyTexture(gTextureCounter);
    UDestroyTexture(gTextureFidget);
    UDestroyTexture(gTextureCarmex);
    UDestroyTexture(gTexture3DS);

    // Release shader program
    UDestroyShaderProgram(gProgramId);
    UDestroyShaderProgram(gLightProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Function called to render a frame
void URender() {

    // Positions and colors of the lights in the scene
    glm::vec3 lightPos1 = glm::vec3(-12.0f, 12.0f, 0.0f);
    GLfloat light1Color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec3 lightPos2 = glm::vec3(12.0f, 12.0f, 0.0f);
    GLfloat light2Color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // Directional light
    glm::vec4 light3Color = glm::vec4(0.8f, 0.7f, 0.6f, 1.0f);
    GLfloat light3Direction[3] = { 0.78f, 0.08f, -0.62f };

    // Enable Z-Depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the size of the window
    int width, height;
    glfwGetFramebufferSize(gWindow, &width, &height);

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // If orthoview is true, creates ortho view
    if (orthoView == GL_TRUE)
    {
        float scale = 100;
        float nearPlane = -10.0f;
        float farPlane = 10.0f;
        projection = glm::ortho(-(width / scale), width / scale, -(height / scale), height / scale, nearPlane, farPlane);
    }

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Set the shader program to be used
    glUseProgram(gProgramId);

    // Set the uniform variables for model, view, and projection matrices
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projectionLoc = glGetUniformLocation(gProgramId, "projection");

    // Set the uniform variables for light and view positions
    GLint ambStrLoc = glGetUniformLocation(gProgramId, "ambientStrength");
    GLint ambColLoc = glGetUniformLocation(gProgramId, "ambientColor");
    GLint light1ColLoc = glGetUniformLocation(gProgramId, "light1Color");
    GLint light1PosLoc = glGetUniformLocation(gProgramId, "light1Position");
    GLint light2ColLoc = glGetUniformLocation(gProgramId, "light2Color");
    GLint light2PosLoc = glGetUniformLocation(gProgramId, "light2Position");
    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint dirLightCol = glGetUniformLocation(gProgramId, "light3Color");
    GLint dirLightPos = glGetUniformLocation(gProgramId, "light3Direction");
    GLint dirLightInt = glGetUniformLocation(gProgramId, "light3Intensity");
    GLint specIntLoc = glGetUniformLocation(gProgramId, "specularIntensity");
    GLint highlghtSzLoc = glGetUniformLocation(gProgramId, "highlightSize");
    GLint light1IntensityLoc = glGetUniformLocation(gProgramId, "light1Intensity");
    GLint light2IntensityLoc = glGetUniformLocation(gProgramId, "light2Intensity");

    // Set the uniform veriables for texture and color information
    GLint uTextureBaseLoc = glGetUniformLocation(gProgramId, "uTextureBase");
    GLint uTextureExtraLoc = glGetUniformLocation(gProgramId, "uTextureExtra");
    GLint multipleTexturesLoc = glGetUniformLocation(gProgramId, "multipleTextures");
    GLint singleTextureLoc = glGetUniformLocation(gProgramId, "singleTexture");

    // Change light Intensity
    glUniform1f(light1IntensityLoc, 0.3f);
    glUniform1f(light2IntensityLoc, 0.3f);

    // Change ambient light strength and color
    glUniform1f(ambStrLoc, 0.5f);
    glUniform3f(ambColLoc, 0.5f, 0.5f, 0.5f);

    // Adjust light Position and color
    glUniform3f(light1ColLoc, light1Color[0], light1Color[1], light1Color[2]);
    glUniform3f(light1PosLoc, lightPos1.x, lightPos1.y, lightPos1.z);
    glUniform3f(light2ColLoc, light2Color[0], light2Color[1], light2Color[2]);
    glUniform3f(light2PosLoc, lightPos2.x, lightPos2.y, lightPos2.z);

    // Adjust directional light parameters
    glUniform3f(dirLightCol, light3Color.r, light3Color.g, light3Color.b);
    glUniform3f(dirLightPos, light3Direction[0], light3Direction[1], light3Direction[2]);
    glUniform1f(dirLightInt, 0.4f);

    // set general specular intensity
    glUniform1f(specIntLoc, 1.0f);

    //set general specular highlight size
    glUniform1f(highlghtSzLoc, 7.0f);
    glUniform2f(glGetUniformLocation(gProgramId, "uvScale"), gUVScale.x, gUVScale.y);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // set viewPosition uniform in the shader (replace "gProgramId" with your actual program ID)
    GLint viewPosLoc = glGetUniformLocation(gProgramId, "viewPosition");
    glUniform3f(viewPosLoc, gCamera.Position.x, gCamera.Position.y, gCamera.Position.z);


    // Create plane for Countertop
    // ==========================================================================================================

    // Render the Plane with texture
    glm::mat4 modelPlane = glm::mat4(1.0f); // Render the plane
    modelPlane = glm::translate(modelPlane, glm::vec3(0.0f, -0.30f, 0.0f)); // position the cylinder
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPlane));
    glBindVertexArray(gPlaneMesh.vao);

    // Inform the shader we are using one texture
    glUniform1i(singleTextureLoc, GL_TRUE);

    // Bind the counter texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureCounter);

    // Draw the plane
    glDrawElements(GL_TRIANGLES, gPlaneMesh.nIndices, GL_UNSIGNED_INT, 0);

    // unbind for future objects
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Inform the shader that we are no longer using textures
    glUniform1i(singleTextureLoc, GL_FALSE);

    // Create Fidget Toy
    // ==========================================================================================================

    // Create Cube for Fidget toy
    glm::mat4 modelCube = glm::mat4(1.0f); // Render the cube
    modelCube = glm::translate(modelCube, glm::vec3(-2.0f, 0.15f, 3.0f)); // Move cube over
    modelCube = glm::scale(modelCube, glm::vec3(0.8f)); // Scale the cube to reasonable size
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCube));
    glBindVertexArray(gFidgetMesh.vao);

    // Inform the shader we are using one texture
    glUniform1i(singleTextureLoc, GL_TRUE);

    // Bind the diget cube texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureFidget);

    glDrawArrays(GL_TRIANGLES, 0, gFidgetMesh.nVertices);

    // unbind for future objects
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Inform the shader that we are no longer using textures
    glUniform1i(singleTextureLoc, GL_FALSE);

    // Create cylinder mesh with color - First button
    GLfloat ButtonColor[4] = { 0.0f, 0.5f, 0.0f, 1.0f }; // Darker Green
    glm::mat4 modelButtons = glm::mat4(1.0f); // Render the cylinder for button
    modelButtons = glm::translate(modelButtons, glm::vec3(-1.7f, 0.32f, 2.8f)); // Move cylinder up
    modelButtons = glm::scale(modelButtons, glm::vec3(.3f)); // Shrink cylinder
    modelButtons = glm::rotate(modelButtons, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the y-axis
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelButtons));
    glUniform4fv(objectColorLoc, 1, ButtonColor);
    glBindVertexArray(gCylinderMesh.vao);
    glDrawElements(GL_TRIANGLES, gCylinderMesh.nIndices, GL_UNSIGNED_INT, 0);

    // Second button
    glm::mat4 modelButton2 = glm::mat4(1.0f);
    modelButton2 = glm::translate(modelButton2, glm::vec3(-1.7f, 0.0f, 2.8f)); // Move button down
    modelButton2 = glm::scale(modelButton2, glm::vec3(0.3f)); // Shrink button
    modelButton2 = glm::rotate(modelButton2, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the y-axis
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelButton2));
    glUniform4fv(objectColorLoc, 1, ButtonColor);
    glBindVertexArray(gCylinderMesh.vao);
    glDrawElements(GL_TRIANGLES, gCylinderMesh.nIndices, GL_UNSIGNED_INT, 0);

    // third button
    glm::mat4 modelButton3 = glm::mat4(1.0f);
    modelButton3 = glm::translate(modelButton3, glm::vec3(-1.7f, 0.0f, 3.16f)); // Move button to the left
    modelButton3 = glm::scale(modelButton3, glm::vec3(0.3f)); // Shrink button
    modelButton3 = glm::rotate(modelButton3, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the y-axis
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelButton3));
    glUniform4fv(objectColorLoc, 1, ButtonColor);
    glBindVertexArray(gCylinderMesh.vao);
    glDrawElements(GL_TRIANGLES, gCylinderMesh.nIndices, GL_UNSIGNED_INT, 0);

    // fourth button
    glm::mat4 modelButton4 = glm::mat4(1.0f);
    modelButton4 = glm::translate(modelButton4, glm::vec3(-1.7f, 0.32f, 3.16f)); // Move button to the left and below
    modelButton4 = glm::scale(modelButton4, glm::vec3(0.3f)); // Shrink button
    modelButton4 = glm::rotate(modelButton4, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the y-axis
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelButton4));
    glUniform4fv(objectColorLoc, 1, ButtonColor);
    glBindVertexArray(gCylinderMesh.vao);
    glDrawElements(GL_TRIANGLES, gCylinderMesh.nIndices, GL_UNSIGNED_INT, 0);

    // Create the Nintendo 3DS Object
    // ==========================================================================================================

    //set specular intensity and highlight size for the 3DS
    glUniform1f(specIntLoc, 8.0f);
    glUniform1f(highlghtSzLoc, 4.0f);

    // Inform the shader we are using one texture
    glUniform1i(singleTextureLoc, GL_TRUE);

    // Bind the 3DS texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexture3DS);

    // Create Bottom Screen for Nintendo device
    glm::mat4 model3DS = glm::mat4(1.0f); // Render the cube
    model3DS = glm::translate(model3DS, glm::vec3(-2.0f, 0.08f, -3.0f)); // Move cube over
    model3DS = glm::rotate(model3DS, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the y-axis
    model3DS = glm::rotate(model3DS, glm::radians(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)); // Rotate around the y-axis
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3DS));
    glBindVertexArray(g3DSMesh.vao);
    glDrawArrays(GL_TRIANGLES, 0, g3DSMesh.nVertices);

    // Reset the model matrix
    model3DS = glm::mat4(1.0f);

    // Apply new transformation for the second screen
    model3DS = glm::translate(model3DS, glm::vec3(-2.0f, -0.17f, -3.0f)); // Move new cube over
    model3DS = glm::rotate(model3DS, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the y-axis
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3DS));
    glDrawArrays(GL_TRIANGLES, 0, g3DSMesh.nVertices);

    // unbind for future objects
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Inform the shader that we are no longer using textures
    glUniform1i(singleTextureLoc, GL_FALSE);

    //Reset Speculer parameters
    glUniform1f(specIntLoc, 1.0f);
    glUniform1f(highlghtSzLoc, 7.0f);

    // Create the Service Bell Object
    // ==========================================================================================================

    // Render the Torus with color
    glm::mat4 modelTorus = glm::mat4(1.0f); // Render the torus
    modelTorus = glm::rotate(modelTorus, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the x-axis
    modelTorus = glm::translate(modelTorus, glm::vec3(3.0f, 0.0f, 0.0f)); // Move Torus over
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTorus));
    glUniform4fv(objectColorLoc, 1, defaultColor); // Set default color (black)
    glBindVertexArray(gTorusMesh.vao);
    glDrawArrays(GL_TRIANGLES, 0, gTorusMesh.nIndices);

    // Render the half-sphere with textures
    glm::mat4 modelSphere = glm::mat4(1.0f); // Render the sphere
    modelSphere = glm::scale(modelSphere, glm::vec3(1.2f)); // Scale the sphere down to fit inside the torus
    modelSphere = glm::rotate(modelSphere, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the x-axis
    modelSphere = glm::rotate(modelSphere, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the z-axis
    modelSphere = glm::translate(modelSphere, glm::vec3(0.0f, -2.5f, 0.0f)); // Move half-sphere into place
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSphere));
    glBindVertexArray(gHalfSphereMesh.vao);

    // Inform the shader that we have multiple textures for the half-sphere
    glUniform1i(multipleTexturesLoc, GL_TRUE);

    // Bind the yellow texture to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureYellow);
    glUniform1i(uTextureBaseLoc, 0);

    // Bind the smiley texture to texture unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureSmiley);
    glUniform1i(uTextureExtraLoc, 1);

    // Draw the half-sphere
    glDrawElements(GL_TRIANGLES, gHalfSphereMesh.nIndices, GL_UNSIGNED_INT, 0);

    // Unbind textures
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Inform the shader we are no longer using textures
    glUniform1i(multipleTexturesLoc, GL_FALSE);

    // Create cylinder mesh with color
    GLfloat cylinderColor[4] = { 0.75f, 0.75f, 0.75f, 1.0f }; // Color: silver
    glm::mat4 modelCylinder = glm::mat4(1.0f); // Render the cylinder
    modelCylinder = glm::translate(modelCylinder, glm::vec3(3.0f, 1.0f, 0.0f)); // Move cylinder up
    modelCylinder = glm::scale(modelCylinder, glm::vec3(.8f)); // Shrink cylinder
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCylinder));
    glUniform4fv(objectColorLoc, 1, cylinderColor);
    glBindVertexArray(gCylinderMesh.vao);
    glDrawElements(GL_TRIANGLES, gCylinderMesh.nIndices, GL_UNSIGNED_INT, 0);



    // Create lip balm
    // ==========================================================================================================

    // Set colors for cap and bottom
    GLfloat capColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat bottomColor[4] = { 0.7529f, 0.7529f, 0.7529f, 1.0f };

    // Create cylinder mesh with color
    glm::mat4 modelLipBalm = glm::mat4(1.0f); // Render the cylinder
    modelLipBalm = glm::translate(modelLipBalm, glm::vec3(1.0f, 0.0f, 2.2f)); // Move cylinder up
    modelLipBalm = glm::scale(modelLipBalm, glm::vec3(1.4f)); // Shrink cylinder
    modelLipBalm = glm::rotate(modelLipBalm, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the z-axis
    modelLipBalm = glm::rotate(modelLipBalm, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the z-axis
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelLipBalm));
    glUniform4fv(objectColorLoc, 1, cylinderColor);
    glBindVertexArray(gCarmexTubeMesh.vao);

    // Inform the shader we are using one texture
    glUniform1i(singleTextureLoc, GL_TRUE);

    // Bind the counter texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureCarmex);

    glDrawElements(GL_TRIANGLES, gCarmexTubeMesh.nIndices, GL_UNSIGNED_INT, 0);

    // unbind for future objects
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Inform the shader that we are no longer using textures
    glUniform1i(singleTextureLoc, GL_FALSE);

    // Create cap mesh with color
    glm::mat4 modelCap = glm::mat4(1.0f); // Render the cap
    modelCap = glm::translate(modelCap, glm::vec3(1.0f, 0.0f, 1.2f)); // Move cap up
    modelCap = glm::scale(modelCap, glm::vec3(1.42f)); // Shrink cap
    modelCap = glm::rotate(modelCap, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the z-axis
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCap));
    glUniform4fv(objectColorLoc, 1, capColor);
    glBindVertexArray(gCarmexCapMesh.vao);
    glDrawElements(GL_TRIANGLES, gCarmexCapMesh.nIndices, GL_UNSIGNED_INT, 0);

    // Create bottom mesh with color
    glm::mat4 modelBottom = glm::mat4(1.0f); // Render the cap
    modelBottom = glm::translate(modelBottom, glm::vec3(1.0f, 0.0f, 3.12f)); // Move cap up
    modelBottom = glm::scale(modelBottom, glm::vec3(1.42f)); // Shrink cap
    modelBottom = glm::rotate(modelBottom, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around the z-axis
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelBottom));
    glUniform4fv(objectColorLoc, 1, bottomColor);
    glBindVertexArray(gCarmexBottomMesh.vao);
    glDrawElements(GL_TRIANGLES, gCarmexBottomMesh.nIndices, GL_UNSIGNED_INT, 0);

    // Create Light spheres
    // ==========================================================================================================

    // Set the shader to be used
    glUseProgram(gLightProgramId);

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gLightProgramId, "model");
    viewLoc = glGetUniformLocation(gLightProgramId, "view");
    projectionLoc = glGetUniformLocation(gLightProgramId, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gLightSphereMesh.vao);

    // First Light Sphere
    glm::mat4 modelSphere1 = glm::mat4(1.0f);
    modelSphere1 = glm::translate(modelSphere1, lightPos1); // Adjusted position (Matches light)
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSphere1));
    glUniform4fv(glGetUniformLocation(gLightProgramId, "lampColor"), 1, light1Color); // Adjusted color (Matches light)
    glDrawElements(GL_TRIANGLES, gLightSphereMesh.nIndices, GL_UNSIGNED_INT, 0);

    // Second Light Sphere
    glm::mat4 modelSphere2 = glm::mat4(1.0f);
    modelSphere2 = glm::translate(modelSphere2, lightPos2); // Adjusted position (Matches light)
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSphere2));
    glUniform4fv(glGetUniformLocation(gLightProgramId, "lampColor"), 1, light2Color); // Adjusted color (Matches light)
    glDrawElements(GL_TRIANGLES, gLightSphereMesh.nIndices, GL_UNSIGNED_INT, 0);


    glBindVertexArray(0);
    glUseProgram(0);

    glfwSwapBuffers(gWindow);
    glfwPollEvents();
}

/*
 * Initialize necessary components
 *
 */
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // GLEW: initialize
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Display GPU OpenGL version
    std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    return true;
}


/*
 * Generate and load the texture
 * Sourced from module tutorials
 *
 */
bool UCreateTexture(const char* filename, GLuint& textureId, bool clamp, bool flip)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        if (flip)
        {
            flipImageVertically(image, width, height, channels);
        }

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        if (clamp == true)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // horizontal wrapping
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // vertical wrapping
        }

        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // horizontal wrapping
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // vertical wrapping
        }


        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}

/*
 * Delete Textures
 *
 */
void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}

/*
 * Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
 *
 */
void UProcessInput(GLFWwindow* window) {
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        orthoView = !orthoView;

}

/*
 * GLFW: whenever the window size changes (by OS or user resize) this callback function executes
 *
 */
void UResizeWindow(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

/*
 * glfw: whenever the mouse moves, this callback is called
 *
 */
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}

/*
 * glfw: whenever the mouse scroll wheel scrolls, this callback is called
 *
 */
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

/*
 * Create Shader Program
 *
 */
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrieve the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader and print compilation errors (if any)
    glCompileShader(vertexShaderId);
    // Check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }

    glCompileShader(fragmentShaderId); // Compile the fragment shader
    // Check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }

    // Attach compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId); // Links the shader program
    // Check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }

    glUseProgram(programId); // Use the shader program

    return true;
}

/*
 * delete Shader Program
 *
 */
void UDestroyShaderProgram(GLuint programId) {
    glDeleteProgram(programId);
}

/*
 * Flip images to match OpenGL orientation
 *
 */
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}
