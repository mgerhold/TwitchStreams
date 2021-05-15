//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include "Application.hpp"
#include "VertexBufferObject.hpp"
#include "ShaderProgram.hpp"
#include "VertexArrayObject.hpp"
#include "ElementBufferObject.hpp"

class Sandbox : public Application {
public:
    using Application::Application;

private:
    void setup() override;
    void update() override;
    void processInput() noexcept;
    void render() noexcept;
    void setupShaders() noexcept;

private:
    VertexBufferObject vertexBufferObject;
    ShaderProgram shaderProgram;
    VertexArrayObject vertexArrayObject;
    ElementBufferObject elementBufferObject;
};


