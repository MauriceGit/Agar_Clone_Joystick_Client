#version 330

in vec3 normal;
in vec3 pos;
in vec3 lightPos;
out vec4 Color;
uniform vec3 colorIn;
uniform int isLight;

void main(){



    vec3 l = normalize(lightPos - pos);
    //vec3 e = normalize(-pos);
    //vec3 r = normalize(-reflect(l, normal));

    vec3 specularColor = colorIn*3;
    float dotProduct = max(dot(normal,l), 0.0);
    vec3 specular = specularColor * pow(dotProduct, 8.0);
    specular = clamp(specular, 0.0, 1.0);

    vec3 diffuseColor = colorIn*2;
    vec3 diffuse  = diffuseColor * max(dot(normal, l), 0);
    //diffuse = clamp(diffuse, 0.0, 0.3);
    diffuse = clamp(diffuse, 0.0, 1.0);

    vec3 diffuseColorNeg = colorIn*3;
    vec3 diffuseNeg  = diffuseColorNeg * max(dot(-normal, l), 0);
    diffuseNeg = clamp(diffuseNeg, 0.0, 1.0);
    diffuseNeg = vec3(1)-diffuseNeg;

    vec3 ambient = colorIn / 1.5;

    vec3 colorOut = mix(diffuseNeg, diffuse, 0.5);

    //Color = vec4(ambient + diffuse + specular + colorIn, 1.0);
    Color = vec4(diffuseNeg/4 + diffuse/4 + ambient/4 + specular/4 + colorIn/3, 1.0);

    if (isLight == 1) {
        Color = vec4(1, 1, 0, 1);
    }
    //Color = vec4(specular, 1.0);

    /*vec3 lightVec = lightPos - pos;
    float dot1 = dot(lightVec, normal);

    vec3 specColor = colorIn * 1.5;
    float factor = min(max(pow(dot1/1.0, 0.7), 0), 1);
    vec3 specColorOut = mix(specColor, colorIn, -factor);

    vec3 final = specColorOut + colorIn;

    final.r = final.r >= 10 ? colorIn.r : final.r;
    final.g = final.g >= 0.1 ? colorIn.g : final.g;
    final.b = final.b >= 0.1 ? colorIn.b : final.b;

    Color = vec4(final, 1.0);*/
}
