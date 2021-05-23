//
// Created by coder2k on 15.05.2021.
//

#include "Sandbox.hpp"
#include <string>

void Sandbox::setup() {
    setupShaders();

    /*vertexArrayObject.bind();
    vertexBufferObject.bind();*/
    const std::vector<GLfloat> vertices {
            // position         // color
            -0.9f, 0.9f,        1.0f, 0.0f, 0.0f, // top left 0
            -0.9f, 0.1f,        0.0f, 1.0f, 0.0f, // bottom left 1
            -0.1f, 0.1f,        0.0f, 0.0f, 1.0f, // bottom right 2
            -0.1f, 0.9f,        0.0f, 1.0f, 1.0f, // top right 3

            0.1f, -0.1f,        1.0f, 0.0f, 1.0f, // top left 4
            0.1f, -0.9f,        1.0f, 1.0f, 0.0f, // bottom left 5
            0.9f, -0.9f,        1.0f, 1.0f, 1.0f, // bottom right 6
            0.9f, -0.1f,        1.0f, 0.0f, 0.0f, // top right 7
    };
    const std::vector<GLuint> indices {
            0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4,
    };
    vertexBuffer.bind();
    vertexBuffer.setVertexAttributeLayout(VertexAttributeDefinition{2, GL_FLOAT, false},
                                          VertexAttributeDefinition{3, GL_FLOAT, false});
    vertexBuffer.submitData(vertices, GLDataUsagePattern::StaticDraw);
    //vertexBufferObject.submitData(vertices, GLDataUsagePattern::StaticDraw);
    elementBufferObject.submitData(indices, GLDataUsagePattern::StaticDraw);
    /*vertexArrayObject.setVertexAttributes(
            VertexAttributeDefinition{ 2, GL_FLOAT, false },
            VertexAttributeDefinition{ 3, GL_FLOAT, false }
        );*/
    shaderProgram.bind();
}

void Sandbox::update() {
    processInput();
    render();
}

void Sandbox::processInput() noexcept {
    if (glfwGetKey(getGLFWWindowPointer(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(getGLFWWindowPointer(), true);
    }
}

void Sandbox::render() noexcept {
    glClearColor(73.f / 255.f, 54.f / 255.f, 87.f / 255.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, gsl::narrow_cast<GLsizei>(elementBufferObject.count()),
            GL_UNSIGNED_INT, nullptr);
}

void Sandbox::setupShaders() noexcept {
    using namespace std::literals::string_view_literals;
    constexpr std::string_view vertexShaderSource = "#version 430 core\n"
                                                    "layout (location = 0) in vec2 aPos;\n"
                                                    "layout (location = 1) in vec3 aColor;\n"
                                                    "out vec3 fragmentColor;\n"
                                                    "void main()\n"
                                                    "{\n"
                                                    "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
                                                    "   fragmentColor = aColor;\n"
                                                    "}"sv;
    constexpr std::string_view fragmentShaderSource = "#version 430 core\n"
                                                      "in vec3 fragmentColor;\n"
                                                      "out vec4 FragColor;\n"
                                                      "\n"
                                                      "void main()\n"
                                                      "{\n"
                                                      "    FragColor = vec4(fragmentColor.r, fragmentColor.g, fragmentColor.b, 1.0f);\n"
                                                      "}"sv;
    shaderProgram.compile(vertexShaderSource, fragmentShaderSource);
}
