#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec3 fragmentColor;
out vec3 fragmentPosition;
out vec2 texCoords;

uniform mat4 projectionMatrix;

void main() {
   gl_Position = projectionMatrix * vec4(aPos.x, aPos.y, 0.0, 1.0);
   fragmentPosition = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
   fragmentColor = aColor;
   texCoords = aTexCoords;
}