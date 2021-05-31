//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include "Application.hpp"
#include "ShaderProgram.hpp"
#include "VertexBuffer.hpp"

class Sandbox final : public Application<Sandbox> {
public:
    using Application::Application;

private:
    void setup() noexcept;
    void update() noexcept;

    void processInput() noexcept;
    void render() noexcept;
    void setupShaders() noexcept;

private:
    VertexBuffer mVertexBuffer;
    ShaderProgram mShaderProgram;

    friend class Application;
};


