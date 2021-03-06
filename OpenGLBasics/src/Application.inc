
template<typename DerivedType>
Application<DerivedType>::Application(const std::string& title, WindowSize size, OpenGLVersion version) noexcept {
    glfwInit();
    spdlog::info("glfw initialized");
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    mWindowPtr = glfwCreateWindow(size.width, size.height, title.c_str(), nullptr, nullptr);
    if (mWindowPtr == nullptr) {
        glfwTerminate();
        mError = true;
        return;
    }
    glfwMakeContextCurrent(mWindowPtr);
    glfwSetFramebufferSizeCallback(mWindowPtr,
                                   [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); });
    spdlog::info("glfw window created");
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        glfwTerminate();
        mError = true;
        return;
    }
    glfwSwapInterval(0);
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(handleOpenGLDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    if (!glfwExtensionSupported("GL_ARB_direct_state_access")) {
        glfwTerminate();
        mError = true;
        return;
    }
}

template<typename DerivedType>
void Application<DerivedType>::run() noexcept {
    using namespace std::chrono_literals;

    static_cast<DerivedType*>(this)->setup();

    const auto frameTimeOutputInterval = 500ms;
    auto lastFrameTimeOutput = std::chrono::high_resolution_clock::now() - frameTimeOutputInterval;
    auto lastTime = std::chrono::high_resolution_clock::now();
    unsigned long numFramesDuringOutputInterval = 0UL;
    while (!glfwWindowShouldClose(mWindowPtr)) {
        static_cast<DerivedType*>(this)->update();
        glfwSwapBuffers(mWindowPtr);
        glfwPollEvents();
        ++numFramesDuringOutputInterval;
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration<double>(currentTime - lastTime).count();
        lastTime = currentTime;
        if (currentTime >= lastFrameTimeOutput + frameTimeOutputInterval) {
            const double framesPerSecond =
                    static_cast<double>(numFramesDuringOutputInterval) /
                    std::chrono::duration<double, std::ratio<1, 1>>(frameTimeOutputInterval).count();
            lastFrameTimeOutput = currentTime;
            glfwSetWindowTitle(mWindowPtr,
                               fmt::format("{} ms ({} fps)", 1000.0 / framesPerSecond, framesPerSecond).c_str());
            numFramesDuringOutputInterval = 0UL;
        }
    }
}

template<typename DerivedType>
Application<DerivedType>::~Application() {
    spdlog::info("exiting application");
    glfwDestroyWindow(mWindowPtr);
    glfwTerminate();
}

template<typename DerivedType>
void Application<DerivedType>::handleOpenGLDebugOutput(GLenum source,
                                                       GLenum type,
                                                       unsigned int id,
                                                       GLenum severity,
                                                       GLsizei,
                                                       const char* message,
                                                       const void*) {
    // ignore non-significant mError/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
        default:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
        default:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
        default:
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

template<typename DerivedType>
WindowSize Application<DerivedType>::getFramebufferSize() const {
    WindowSize result;
    glfwGetFramebufferSize(mWindowPtr, &result.width, &result.height);
    return result;
}
