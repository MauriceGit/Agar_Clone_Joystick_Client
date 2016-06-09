#version 330

layout (location = 0) in vec3 vertPos;
uniform mat4 viewMatrix, projMatrix;
uniform vec3 translation;

void main(){
    gl_Position =  projMatrix * viewMatrix * vec4(vertPos+translation, 1.0);
}
