#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include "shader.h"

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <irrKlang/irrKlang.h>

#include "common.h"
#include "gameobjectchildren.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

using namespace irrklang;

//Prototypes

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ProcessInputs(GLFWwindow* window);
void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void Update();
void Render(map<string, GameObject*> renderList, Shader& shader);
vector<Timer> timers;
Timer playerJumpTimer(.1f, Timer::Stop);
glm::vec3 RandomPos(int x = 1, int y = 1, int z = 1);

//Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Global Variables
bool rerollRNG = false;
bool rPressed = false;
float jumpVelocity = 1000;
float jumpHold = 0;
Shader* shaderPointer;
bool fPressed = false;

int startSceneID;
Scene* currentScene;

//Time Variables
float deltaTime = 0.0f;
float deltaTimeCap = 0.015f;
float limitedDT = 0.0f;
float lastFrame = 0.0f;
float currentFrame = (float)glfwGetTime();
static const double limitFPS = 0.01666666666666667;

//Camera Variables
glm::mat4 projection;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH/2.0f, lastY = SCR_HEIGHT/2.0f;
bool firstMouse = true;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
   // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, Key_Callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // set rand seed
    srand(time(0));
    // set opengl states
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Set up irrKlang for audio
    // ------------------------------------
    ISoundEngine* engine = createIrrKlangDevice();
    if (!engine) {
        cout << "sound engine failed to start" << endl;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shader.vs", "shader.fs");
    shaderPointer = &ourShader;
    Shader lightShader("simpleShader.vs", "simpleShader.fs");

    Primitives playerPrim;
    Model playerMesh = playerPrim.CreateModel(Primitives::Cube);

    GameObject player(&playerMesh, glm::vec3(0.f), glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0), "player");
    GameObject cameraGO("camera");
    cameraGO.scale = glm::vec3(0.f);

    camera.playerChar = &player;
    camera.playerChar->gravity = false;
    currentScene = new Scene("Current");

    glm::vec3 pointLightPos[4] = {
        glm::vec3(1.0f,  -6.2f,  1.0f),
        glm::vec3(2.0f, -6.3f, 2.0f),
        glm::vec3(-2.0f,  -6.0f, -2.0f),
        glm::vec3(-1.0f,  -6.0f, -1.0f)
    };

    

    ourShader.use();

    ourShader.setVec3("dirLight.direction", -0.2f, -1.f, -.3f);
    ourShader.setVec3("dirLight.ambient", 0.7f, 0.7f, 0.7f);
    ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    for (unsigned int i = 0; i < 4; i++) {
        ourShader.setVec3(("pointLights[" + to_string(i) + "].position"), pointLightPos[i]);
        ourShader.setVec3(("pointLights[" + to_string(i) + "].ambient"), .05f, .05f, .05f);
        ourShader.setVec3(("pointLights[" + to_string(i) + "].diffuse"), .4f, .4f, .8f);
        ourShader.setVec3(("pointLights[" + to_string(i) + "].specular"), 1.f, 1.f, 1.f);

        ourShader.setFloat(("pointLights[" + to_string(i) + "].constant"), 1.f);
        ourShader.setFloat(("pointLights[" + to_string(i) + "].linear"), .09f);
        ourShader.setFloat(("pointLights[" + to_string(i) + "].quadratic"), .032f);
    }

    projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
    ourShader.setMat4("projection", projection);

    float angle = 20;

    double lastTime = glfwGetTime();
    int numFrames = 0;
    int numUpdates = 0;
    float timer = lastTime;

    bool triggerOnce = false;
    bool active = false;
    //Set up scenes
    scenes["Test"] = new TestScene("Test");
    scenes["Procedural"] = new ProceduralScene("Procedural");

    if (!scenes.empty()) {
        currentScene = LoadScene(currentScene, "Test", &camera);
    }
    else cout << "No Scene To Load" << endl;
    // render loop
    // -----------
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window))
    {
        // TIME LOGIC
        currentFrame = (float)glfwGetTime();
        limitedDT += (currentFrame - lastTime) / limitFPS;
        deltaTime = (currentFrame - lastTime);
        if (deltaTime < deltaTimeCap) {
            deltaTime = deltaTimeCap;
        }
        if (limitedDT > 2.0) {
            limitedDT = 1.0;
        }
        lastFrame = currentFrame;
        lastTime = currentFrame;
        
        //cout << deltaTime << ": DT" << endl << limitedDT << ": LDT" << endl;
        // input
        // -----
        ProcessInputs(window);
        numFrames++;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //setup variables
        float sinScale = sin((float)glfwGetTime());

        ourShader.use();

        ourShader.setFloat("material.shininess", 64.0f);
        ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        for (unsigned int i = 0; i < 4; i++) {
            pointLightPos[i] = glm::vec3(sinScale * i, sinScale + i, (sinScale * i) * -1);
            ourShader.setVec3(("pointLights[" + to_string(i) + "].position"), pointLightPos[i]);
        }

        // - Only update at 60 frames / s
        if (limitedDT >= 1.0) {
            limitedDT--;
            Update();   // - Update function
            numUpdates++;
        }
        // - Render at maximum possible frames
        // camera transformations
        camera.updateCameraVectors();
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);
        Render(currentScene->sceneGOs, ourShader); // - Render function

        numFrames++;

        // - Reset after one second
        if (glfwGetTime() - timer > 1.0) {
            timer++;
            //std::cout << "FPS: " << numFrames << " Updates:" << numUpdates << std::endl;
            //cout << limitedDT << ", " << deltaTime << endl;
            numUpdates = 0;
            numFrames = 0;
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void Update() {
    float angle;
    angle = currentFrame * 50;
    for (auto it = currentScene->sceneGOs.begin(); it != currentScene->sceneGOs.end(); it++) {
        for (auto jit = currentScene->sceneGOs.begin(); jit != currentScene->sceneGOs.end(); jit++) {
            /*if (it->second->name != "player" && jit->second->name != "player")*/ it->second->Collisions(jit->second, deltaTime);
            //if (jit->second != it->second && (it->second->name != "player" || jit->second->name != "player")) it->second->HandleCollision(0, jit->second);
            /*if (j <= i) {
                //Skip
            }
            else {
                if (!sceneGOs[i]->staticObj) sceneGOs[i]->Collisions(sceneGOs[j], deltaTime);
            }*/

        }
        it->second->Update(deltaTime);

        if (it->second->name == "player") {
            //camera.Position = sceneGOs[i]->position;
            //cout << sceneGOs[i]->velocity.y << endl;
        }
        else if (it->second->name == "box") {
            it->second->Rotate(0.f, 0.f, angle);
        }
        else if (it->second->name == "maxwell") {
            it->second->Rotate(0.f, -90.f, angle);
        }
    }

    for (unsigned int i = 0; i < timers.size(); i++) {
        timers[i].Update(deltaTime);
    }
    if (playerJumpTimer.isStarted) {
        if (!playerJumpTimer.isFinished) {
            playerJumpTimer.Update(deltaTime);
            
            if (jumpHold > 0.005) {
                jumpHold = 0.005;
            }
            camera.playerChar->velocity.y += (jumpVelocity * deltaTime) * jumpHold;
            cout << camera.playerChar->position.y << endl;
        }
        else {
            playerJumpTimer.isFinished = false;
            playerJumpTimer.isStarted = false;
            camera.playerChar->gravity = true;
        }
    }
}

void Render(map<string, GameObject*> renderList, Shader& shader) {
    std::map<float, GameObject*> sortedTransparencies;

    for (auto it = renderList.begin(); it != renderList.end(); it++) {
        if (it->second->transparent) {
            float distance = glm::length(camera.Position - it->second->position);
            sortedTransparencies[distance] = it->second;
        }
        else {
            it->second->Render(shader);
        }
    }
    for (std::map<float, GameObject*>::reverse_iterator i = sortedTransparencies.rbegin(); i != sortedTransparencies.rend(); ++i) {
        i->second->Render(shader);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        rPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && rPressed) {
        rPressed = false;
    }
}

void ProcessInputs(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (camera.playerChar->gravity == false && !playerJumpTimer.isStarted) {
            //Jump reaches about 0.8 in height
            jumpHold = 0;
            playerJumpTimer.isStarted = true;
        }
        else {
            jumpHold += 0.06 * deltaTime;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.Zoom = 0;
        projection = glm::perspective(glm::radians(90.0f - camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        if (shaderPointer != nullptr) shaderPointer->setMat4("projection", projection);
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        camera.playerChar->gravity = !camera.playerChar->gravity;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        currentScene->PassDataToScene();
        fPressed = true;
    }
    if (fPressed && glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
        cout << "F Released" << endl;
        currentScene->PassDataToScene(1);
        fPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (sceneLoaded) {
            currentScene = LoadScene(currentScene, "Test", &camera);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (sceneLoaded) {
            //currentScene = LoadScene(currentScene, "Level1", &camera);
        }
    }
}



void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {

    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll((float)(yoffset));
    projection = glm::perspective(glm::radians(90.0f - camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
    if (shaderPointer != nullptr) shaderPointer->setMat4("projection", projection);
}

glm::vec3 RandomPos(int x, int y, int z) {
    float rX, rY, rZ;
    rX = (float)(rand() % x + 1) - (int)(x / 2);
    rY = (float)(rand() % y + 1) - (int)(y / 2);
    rZ = (float)(rand() % z + 1) - (int)(z / 2);

    return glm::vec3(rX, rY, rZ);
}