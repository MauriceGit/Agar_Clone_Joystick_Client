#version 330

in vec3 normal;
in vec3 pos;
in vec3 lightPos;
out vec4 Color;
uniform vec3 colorIn;

void main(){



    vec3 l = normalize(lightPos - pos);
    vec3 e = normalize(-pos);
    vec3 r = normalize(-reflect(l, normal));

    vec3 specular = vec3(1,0,0) * pow(max(dot(r,e), 0.0), 0.1 * 0.8);
    specular = clamp(specular, 0.0, 1.0);

    vec3 diffuse  = vec3(0,1,0) * max(dot(normal, l), 0);
    diffuse = clamp(diffuse, 0.0, 1.0);

    vec3 ambient = vec3(0,0,1);

    Color = vec4(ambient + diffuse + specular + colorIn, 1.0);

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
