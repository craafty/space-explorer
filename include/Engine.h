#pragma once

#include <epoxy/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include "Camera.h"
#include "PlanetManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class Engine {
public:
    Engine();
    ~Engine();
    
    bool Initialize(int width, int height, const std::string& title);
    void Run();
    void Shutdown();
    
private:
    GLFWwindow* m_window;
    int m_width, m_height;
    bool m_running;
    
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<PlanetManager> m_planetManager;
    GLuint m_shaderProgram;
    
    bool m_firstMouse;
    float m_lastX, m_lastY;
    bool m_wireframeMode;
    bool m_wireframeKeyPressed;
    
    // UI control
    bool m_uiMode;              // true = show UI, false = hide UI  
    bool m_mouseTogglePressed;  // prevent key repeat
    
    bool InitializeWindow(int width, int height, const std::string& title);
    bool InitializeOpenGL();
    void ProcessInput(float deltaTime);
    void Update(float deltaTime);
    void Render();
    void RenderUI();
    
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

class ShaderManager {
public:
    static GLuint LoadShader(const std::string& vertexPath, const std::string& fragmentPath);
    static std::string ReadFile(const std::string& filepath);
    
private:
    static GLuint CompileShader(GLenum type, const std::string& source);
    static bool CheckCompileErrors(GLuint shader, const std::string& type);
};