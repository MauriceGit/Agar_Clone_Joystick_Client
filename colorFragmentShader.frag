#version 330

out vec4 Color;
uniform vec3 colorIn;

void main(){
    Color = vec4(colorIn, 1.0);
}
