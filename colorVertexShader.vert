#version 330

layout (location = 0) in vec3 vertPos;
uniform mat4 viewMatrix, projMatrix;
uniform vec3 translation;
uniform float mass;
uniform vec3 light;

out vec3 normal;
out vec3 pos;
out vec3 lightPos;

void main(){

    float radius = max(1, sqrt(mass / 3.1415));
    normal = normalize(vertPos);
    vec3 tmpPos = (vertPos * radius) + translation;
    pos = tmpPos;
    lightPos = light;

    gl_Position = projMatrix * viewMatrix * vec4(tmpPos, 1.0);
}
