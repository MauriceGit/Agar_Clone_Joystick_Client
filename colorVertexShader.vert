#version 330

layout (location = 0) in vec3 vertPos;
uniform mat4 viewMatrix, projMatrix;
uniform vec3 translation;
uniform float mass;

void main(){

    float radius = max(1, sqrt(mass / 3.1415));
    vec3 pos = (vertPos * radius) + translation;

    gl_Position =  projMatrix * viewMatrix * vec4(pos, 1.0);
}
