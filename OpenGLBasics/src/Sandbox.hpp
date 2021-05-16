//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include "Application.hpp"
#include "VertexBufferObject.hpp"
#include "ShaderProgram.hpp"
#include "VertexArrayObject.hpp"
#include "ElementBufferObject.hpp"

class Sandbox final : public Application<Sandbox> {
public:
    using Application::Application;
    void setup();
    void update();

private:
    void processInput() noexcept;
    void render() noexcept;
    void setupShaders() noexcept;

private:
    VertexBufferObject vertexBufferObject;
    ShaderProgram shaderProgram;
    VertexArrayObject vertexArrayObject;
    ElementBufferObject elementBufferObject;
};


