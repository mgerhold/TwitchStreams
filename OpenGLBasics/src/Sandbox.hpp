//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include "Application.hpp"
#include "ShaderProgram.hpp"
#include "VertexBuffer.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"
#include <vector>

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
    std::vector<ShaderProgram> mShaderPrograms;
    std::vector<Texture> mTextures;
    Renderer mRenderer;

    friend class Application;
};
