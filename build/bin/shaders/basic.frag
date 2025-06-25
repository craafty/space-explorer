#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 objectColor;

out vec4 FragColor;

void main() {
    // Temporarily very bright for debugging visibility
    FragColor = vec4(objectColor * 2.0, 1.0); // Make it twice as bright
}