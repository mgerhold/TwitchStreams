//
// Created by coder2k on 15.05.2021.
//

#pragma once


#include "strong_type/strong_type.hpp"
#include "WindowSize.hpp"
#include "OpenGLVersion.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

using WindowTitle = strong::type<std::string, struct WindowTitle_>;

class Application {
public:
    Application(const WindowTitle& title, WindowSize size, OpenGLVersion version) noexcept;
    Application(const Application&) = delete;
    Application(Application&&) = delete;

    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    void run() noexcept;
    [[nodiscard]] bool hasError() const { return error; };

    virtual ~Application();

protected:
    [[nodiscard]] inline GLFWwindow * getGLFWWindowPointer() const noexcept { return windowPtr; }

private:
    virtual void setup() = 0;
    virtual void update() = 0;

private:
    GLFWwindow* windowPtr;
    bool error{ false };
};


