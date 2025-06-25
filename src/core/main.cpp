#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/shader.h"
#include "graphics/displayManager.h"
#include "entities/camera.h"
#include "entities/planet.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <unistd.h> // For usleep

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera - positioned to see 25-unit radius Earth
Camera camera(glm::dvec3(50.0, 0.0, 0.0)); // 50 units away from 25-unit radius Earth
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// UI and controls
bool wireframeMode = true;  // Start with wireframe to see LOD
bool wireframeKeyPressed = false;
bool mouseMode = true;      // true = flight mode (cursor hidden), false = GUI mode (cursor visible)  
bool mouseTogglePressed = false;
bool uiMode = true;         // true = show UI, false = hide UI - start with UI visible
bool uiTogglePressed = false;

int main()
{
    // Initialize display
    DisplayManager::createDisplay();
    
    GLFWwindow* window = DisplayManager::getWindow();
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        return -1;
    }
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Start in flight mode with cursor captured
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set 144 FPS limit
    glfwSwapInterval(0); // Disable VSync for manual FPS control

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    
    // Start with wireframe mode to see LOD
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Initialize camera to look at planet
    camera.Front = glm::normalize(glm::dvec3(-1.0, 0.0, 0.0)); // Look toward origin
    camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
    camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
    
    // Check OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    // build and compile shaders
    Shader shaderProgram("shaders/basic.vert", "shaders/basic.frag");

    // Create just Earth for now
    std::vector<std::unique_ptr<Planet>> planets;
    
    // Earth at origin with proper scale
    PlanetData earthData = {
        "Earth", 
        25.0f,                  // 25 unit radius
        glm::vec3(0.0f, 0.0f, 0.0f),  // At origin
        glm::vec3(0.3f, 0.6f, 1.0f),  // Blue color
        10.0f                   // Rotation speed: 10 degrees per second
    };
    planets.push_back(std::make_unique<Planet>(earthData));
    
    // Debug: Verify planet creation
    std::cout << "Created " << planets.size() << " planets" << std::endl;
    if (!planets.empty()) {
        std::cout << "Earth radius: " << planets[0]->GetRadius() << std::endl;
        std::cout << "Earth position: " << planets[0]->GetPosition().x << ", " << planets[0]->GetPosition().y << ", " << planets[0]->GetPosition().z << std::endl;
    }

    // render loop
    while (!DisplayManager::isCloseRequested())
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render - space background
        glClearColor(0.0f, 0.0f, 0.05f, 1.0f);  // Dark blue space background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Re-enable depth testing
        glEnable(GL_DEPTH_TEST);
        
        // use shader
        shaderProgram.use();
        
        // Set up matrices with proper near/far planes
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                                               (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                               0.1f, 1000.0f); // Near: 0.1, Far: 1000
        glm::mat4 view = camera.GetViewMatrix();
        
        // Set global uniforms
        GLint projLoc = glGetUniformLocation(shaderProgram.ID, "projection");
        GLint viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        GLint lightPosLoc = glGetUniformLocation(shaderProgram.ID, "lightPos");
        GLint lightColorLoc = glGetUniformLocation(shaderProgram.ID, "lightColor");
        GLint viewPosLoc = glGetUniformLocation(shaderProgram.ID, "viewPos");
        
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(lightPosLoc, 0.0f, 0.0f, 0.0f); // Sun at origin
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 0.9f); // Slightly warm white light
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(glm::vec3(camera.Position)));
        
        // Camera debug removed for cleaner output
        
        // Update and render planets
        // Render planets
        
        // Triangle removed - sphere is working!
        
        for (auto& planet : planets) {
            planet->Update(glm::vec3(camera.Position), deltaTime);
            planet->Render(shaderProgram.ID, view, projection, glm::vec3(camera.Position));
        }

        // Render ImGui
        if (uiMode) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Solar System Debug Window
            ImGui::Begin("Space Explorer - Solar System Debug");
            
            // Camera information
            ImGui::Text("Camera Position: %.1f, %.1f, %.1f", 
                       camera.Position.x, camera.Position.y, camera.Position.z);
            ImGui::Text("Distance from Origin: %.1f units", 
                       glm::length(camera.Position));
            ImGui::Text("Camera Yaw: %.1f degrees", camera.Yaw);
            ImGui::Text("Camera Pitch: %.1f degrees", camera.Pitch);
            ImGui::Text("Camera Front: %.3f, %.3f, %.3f", 
                       camera.Front.x, camera.Front.y, camera.Front.z);
            
            // Quick positioning buttons
            if (ImGui::Button("Reset Camera to Default")) {
                camera.Position = glm::dvec3(50.0, 0.0, 0.0);
                camera.Yaw = 180.0f;  // Look toward origin
                camera.Pitch = 0.0f;
                camera.UpdateCameraVectors();
            }
            ImGui::SameLine();
            if (ImGui::Button("Look at Origin")) {
                // Calculate direction to origin
                glm::dvec3 direction = glm::normalize(-camera.Position);
                camera.Yaw = glm::degrees(atan2(direction.z, direction.x));
                camera.Pitch = glm::degrees(asin(direction.y));
                camera.UpdateCameraVectors();
            }
            
            if (ImGui::Button("Very Close to Earth")) {
                camera.Position = glm::dvec3(27.0, 0.0, 0.0); // 2 units above 25 radius Earth surface
                camera.Yaw = 180.0f;  // Look toward origin
                camera.Pitch = 0.0f;
                camera.UpdateCameraVectors();
            }
            
            ImGui::Separator();
            
            // Planet information
            int totalTriangles = 0;
            int totalNodes = 0;
            for (const auto& planet : planets) {
                totalTriangles += planet->GetTriangleCount();
                totalNodes += planet->GetActiveNodeCount();
                
                ImGui::Text("%s:", planet->GetData().name.c_str());
                ImGui::Text("  Radius: %.1f km", planet->GetRadiusKm());
                ImGui::Text("  Distance: %.1f km", glm::length(planet->GetPosition()) / 1000.0);
                ImGui::Text("  Triangles: %d", planet->GetTriangleCount());
                ImGui::Text("  Nodes: %d", planet->GetActiveNodeCount());
                
                if (ImGui::Button(("Go to " + planet->GetData().name).c_str())) {
                    glm::vec3 planetPos = planet->GetPosition();
                    // Position camera at good viewing distance
                    float distance = planet->GetRadius() + 10.0f; // 10 units above surface
                    camera.Position = glm::dvec3(planetPos) + glm::dvec3(distance, 0.0, 0.0);
                    
                    // Make camera look at the planet
                    glm::dvec3 direction = glm::normalize(glm::dvec3(planetPos) - camera.Position);
                    camera.Yaw = glm::degrees(atan2(direction.z, direction.x));
                    camera.Pitch = glm::degrees(asin(direction.y));
                    camera.UpdateCameraVectors();
                }
                ImGui::Separator();
            }
            
            ImGui::Text("Total Triangles: %d", totalTriangles);
            ImGui::Text("Total Nodes: %d", totalNodes);
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            
            // Controls
            ImGui::Separator();
            ImGui::Text("Controls:");
            ImGui::Text("WASD - Move");
            ImGui::Text("Space/Shift - Up/Down");
            ImGui::Text("Ctrl - Fast travel");
            ImGui::Text("Q - Toggle mouse");
            ImGui::Text("F - Wireframe");
            ImGui::Text("TAB - Toggle UI");
            
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        DisplayManager::updateDisplay();
        
        // Manual FPS limiting to 144 FPS
        static double lastTime = glfwGetTime();
        static int frameCount = 0;
        frameCount++;
        
        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= 1.0/144.0) {
            lastTime = currentTime;
        } else {
            // Sleep to maintain 144 FPS
            double sleepTime = (1.0/144.0) - (currentTime - lastTime);
            if (sleepTime > 0) {
                usleep((int)(sleepTime * 1000000));
            }
        }
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    DisplayManager::closeDisplay();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
        
    // Speed modifiers
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.MovementSpeed = 100.0f;  // Fast movement
    } else {
        camera.MovementSpeed = 10.0f;   // Normal movement speed 10
    }
    
    // Mouse mode toggle with Q key
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !mouseTogglePressed) {
        mouseMode = !mouseMode;
        if (mouseMode) {
            // Flight mode - hide cursor and enable camera movement
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true; // Reset mouse to avoid sudden camera jump
        } else {
            // GUI mode - show cursor and disable camera movement
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        mouseTogglePressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
        mouseTogglePressed = false;
    }
    
    // UI toggle with TAB key
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !uiTogglePressed) {
        uiMode = !uiMode;
        uiTogglePressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        uiTogglePressed = false;
    }
    
    // Wireframe toggle
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !wireframeKeyPressed) {
        wireframeMode = !wireframeMode;
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        wireframeKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
        wireframeKeyPressed = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // Only process mouse movement in flight mode
    if (!mouseMode) return;
    
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}