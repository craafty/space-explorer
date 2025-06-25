cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(SpaceExplorer)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/OpenGL-Test/include)

# Add ImGui
add_subdirectory(third_party/imgui)

file(GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/src/*.cpp 
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/**/*.cpp
    ${CMAKE_SOURCE_DIR}/src/**/*.c)
    
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/OpenGL-Test/include
    third_party/imgui
    third_party/imgui/backends
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} 
        OpenGL::GL
        glfw
        glm::glm
        imgui
    )
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        glfw
        glm::glm
        imgui
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
    )
else()
    target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        glfw
        glm::glm
        imgui
        ${CMAKE_DL_LIBS}
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin)