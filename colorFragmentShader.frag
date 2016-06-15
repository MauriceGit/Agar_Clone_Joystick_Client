#version 330

in vec3 normal;
in vec3 pos;
in vec3 lightPos;
out vec4 Color;
uniform vec3 colorIn;
uniform int isLight;

void main(){
    vec3 l = normalize(lightPos - pos);

    vec3 specularColor = colorIn*3;
    float dotProduct = max(dot(normal,l), 0.0);
    vec3 specular = specularColor * pow(dotProduct, 8.0);
    specular = clamp(specular, 0.0, 1.0);

    vec3 diffuseColor = colorIn*2;
    vec3 diffuse  = diffuseColor * max(dot(normal, l), 0);
    diffuse = clamp(diffuse, 0.0, 1.0);

    vec3 diffuseColorNeg = colorIn*3;
    vec3 diffuseNeg  = diffuseColorNeg * max(dot(-normal, l), 0);
    diffuseNeg = clamp(diffuseNeg, 0.0, 1.0);
    diffuseNeg = vec3(1)-diffuseNeg;

    vec3 ambient = colorIn / 1.5;

    if (isLight == 1) {
        Color = vec4(1, 1, 0, 1);
    } else {
        Color = vec4(diffuseNeg/4 + diffuse/4 + ambient/4 + specular/4 + colorIn/3, 1.0);
    }
}
