#include "Sandbox.hpp"
#include <cstdlib>

int main() {
    Sandbox sandbox{ "OpenGL application",
        WindowSize{ .width{ 800 }, .height{ 600 }},
        OpenGLVersion{ .major{ 4 }, .minor{ 3 }}
    };
    if (sandbox.hasError()) {
        return EXIT_FAILURE;
    }
    sandbox.run();
}
