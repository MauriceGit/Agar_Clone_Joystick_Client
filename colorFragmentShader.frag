#version 330

in vec3 normal;
out vec4 Color;
uniform vec3 colorIn;
uniform vec3 light;

void main(){

    float dot1 = dot(light, normal);
    float shininess = 0.001;
    vec3 specColor = colorIn * 2.0;
    float factor = max(1.1 +   dot1/25.0   , 0);
    specColor = mix(specColor, colorIn, factor);

    Color = vec4(specColor, 1.0);
}
