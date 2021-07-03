#version 430 core

in vec3 fragmentPosition;
in vec4 fragmentColor;
in vec2 texCoords;
flat in uint texIndex;

out vec4 FragColor;

layout (binding = 0) uniform sampler2D uTextures[32];

void main() {
    //vec4 color = texture(uTextures[texIndex], texCoords);
    vec4 color = vec4(texCoords.x, texCoords.y, 0.0, 1.0);
    /*vec4 color;
    if (texIndex == 0) {
        color = vec4(1.0, 0.0, 0.0, 1.0);
    } else if (texIndex == 1) {
        color = vec4(0.0, 1.0, 0.0, 1.0);
    } else {
        color = vec4(0.0, 0.0, 1.0, 1.0);
    }
    color = vec4(float(texIndex) / 10000.0, float(texIndex) / 10000.0, float(texIndex) / 10000.0, 1.0);*/
    //vec4 color = fragmentColor;
    if (color.a == 0.0) {
        discard;
    }
    FragColor = color;
}