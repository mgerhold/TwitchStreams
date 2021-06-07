#version 430 core

in vec3 fragmentColor;
in vec3 fragmentPosition;
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    //FragColor = vec4(fragmentColor.r, fragmentColor.g, fragmentColor.b, 1.0f);
    vec4 color = texture(uTexture, texCoords);
    if (color.a == 0.0) {
        discard;
    }
    FragColor = color;
}