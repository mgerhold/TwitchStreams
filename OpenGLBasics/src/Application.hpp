//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include "WindowSize.hpp"
#include "OpenGLVersion.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <concepts>

template<typename DerivedType>
class Application {
public:
    Application(const std::string& title, WindowSize size, OpenGLVersion version) noexcept;
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    virtual ~Application();
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    void run() noexcept;
    [[nodiscard]] bool hasError() const { return error; };
    [[nodiscard]] WindowSize getFramebufferSize() const;

protected:
    [[nodiscard]] inline GLFWwindow * getGLFWWindowPointer() const noexcept { return windowPtr; }

private:
    static void handleOpenGLDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                                        GLsizei /* length */, const char *message, const void* /* userParam */);

private:
    GLFWwindow* windowPtr;
    bool error{ false };
};

#include "Application.inc"
