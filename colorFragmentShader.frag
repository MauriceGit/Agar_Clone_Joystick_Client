#version 330

in vec3 normal;
out vec4 Color;
uniform vec3 colorIn;

void main(){

    vec3 light = vec3(13,7,19);

    float dot = dot(light, normal);
    float shininess = 0.001;

    vec3 specColor = colorIn * 2.0;

    float factor = max(0.6 +   dot/25.0   , 0);

    specColor = mix(specColor, colorIn, factor);

    Color = vec4(specColor, 1.0);
}
