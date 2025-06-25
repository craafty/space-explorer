#include "displayManager.h"
#include <iostream>
#include <thread>
#include <chrono>

GLFWwindow* DisplayManager::window = nullptr;
const int DisplayManager::WIDTH = 1280;
const int DisplayManager::HEIGHT = 720;
const int DisplayManager::FPS_CAP = 144;
const char* DisplayManager::TITLE = "Space Explorer";

long DisplayManager::lastFrameTime = getCurrentTime();
float DisplayManager::delta = 0.0f;

void DisplayManager::createDisplay() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    // Start in flight mode with cursor hidden
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Epoxy initializes automatically when first OpenGL function is called
    // No manual initialization needed

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  // Standard depth testing
    
    // Disable face culling for now - planets need to be visible from inside
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);
}

void DisplayManager::updateDisplay() {
    glfwSwapBuffers(window);
    glfwPollEvents();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FPS_CAP));

    long currentFrameTime = getCurrentTime();
    delta = (currentFrameTime - lastFrameTime)/1000.0f;
    lastFrameTime = currentFrameTime;
}

bool DisplayManager::isCloseRequested() {
    return glfwWindowShouldClose(window);
}

float DisplayManager::getFrameTimeSeconds() {
    return delta;
}

void DisplayManager::closeDisplay() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

long DisplayManager::getCurrentTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}