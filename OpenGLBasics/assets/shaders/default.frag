#version 430 core

in vec3 fragmentPosition;
in vec4 fragmentColor;
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    vec4 color = texture(uTexture, texCoords);
    //vec4 color = fragmentColor;
    if (color.a == 0.0) {
        discard;
    }
    FragColor = color;
}