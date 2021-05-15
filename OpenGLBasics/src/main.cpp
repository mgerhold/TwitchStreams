#include "Sandbox.hpp"
#include <cstdlib>
#include <memory>

int main() {
    Sandbox sandbox{
        WindowTitle{ "Meine OpenGL-Anwendung" },
        WindowSize{ .width{ 800 }, .height{ 600 }},
        OpenGLVersion{ .major{ 4 }, .minor{ 3 }}
    };
    if (sandbox.hasError()) {
        return EXIT_FAILURE;
    }
    sandbox.run();
}
