#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec4 fragmentColor;
out vec3 fragmentPosition;
out vec2 texCoords;

uniform mat4 projectionMatrix;

void main() {
   gl_Position = projectionMatrix * vec4(aPos.x, aPos.y, 0.0, 1.0);
   fragmentPosition = aPos;
   fragmentColor = aColor;
   texCoords = aTexCoords;
}