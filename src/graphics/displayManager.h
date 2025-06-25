#pragma once
#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

class DisplayManager {
public:
    static void createDisplay();
    static void updateDisplay();
    static void closeDisplay();
    static bool isCloseRequested();
    static int getWidth() { return WIDTH; }
    static int getHeight() { return HEIGHT; }
    static GLFWwindow* getWindow() { return window; }
    static float getFrameTimeSeconds();

private:
    static GLFWwindow* window;
    static const int WIDTH;
    static const int HEIGHT;
    static const int FPS_CAP;
    static const char* TITLE;

    static long lastFrameTime;
    static float delta;

    static long getCurrentTime();
};