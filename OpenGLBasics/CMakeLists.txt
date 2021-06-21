cmake_minimum_required(VERSION 3.19)
project(OpenGLBasics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    # enable code analysis
    set_property(GLOBAL PROPERTY VS_GLOBAL_EnableCppCoreCheck true)
    set_property(GLOBAL PROPERTY VS_GLOBAL_CodeAnalysisRuleSet CppCoreCheckRules.ruleset)
    set_property(GLOBAL PROPERTY VS_GLOBAL_RunCodeAnalysis true)

    # remove /W3 from defaults
    string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REGEX REPLACE "-W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

set (TARGET_NAME OpenGLBasics)

add_executable(${TARGET_NAME} src/main.cpp src/Application.hpp src/Application.inc src/strong_type/strong_type.hpp src/WindowSize.hpp src/OpenGLVersion.hpp src/Sandbox.cpp src/Sandbox.hpp src/GLDataUsagePattern.hpp src/ShaderProgram.cpp src/ShaderProgram.hpp src/VertexAttributeDefinition.hpp src/GlUtils.cpp src/GlUtils.hpp src/VertexBuffer.cpp src/VertexBuffer.hpp src/expected/expected.hpp src/hash/hash.hpp src/hash/hash.cpp src/stb_image/stb_image.h src/StbImageInclude.cpp src/Image.cpp src/Image.hpp src/Texture.cpp src/Texture.hpp src/Renderer.cpp src/Renderer.hpp)

# set warning levels
if (MSVC)
    target_compile_options(${TARGET_NAME} PUBLIC /W4 /permissive)
else()
    target_compile_options(${TARGET_NAME} PUBLIC -Wall -Wextra -pedantic)
endif()

target_compile_definitions(${TARGET_NAME} PUBLIC "$<$<CONFIG:DEBUG>:DEBUG_BUILD>")
set_property(TARGET ${TARGET_NAME} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# set binary filenames
set_target_properties( ${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${TARGET_NAME}-debug )
set_target_properties( ${TARGET_NAME} PROPERTIES OUTPUT_NAME_RELWITHDEBINFO ${TARGET_NAME}-relwithdebinfo )
set_target_properties( ${TARGET_NAME} PROPERTIES OUTPUT_NAME_RELEASE ${TARGET_NAME}-release )
set_target_properties( ${TARGET_NAME} PROPERTIES OUTPUT_NAME_MINSIZEREL ${TARGET_NAME}-minsizerel )

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE glfw)

find_package(glad CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE glad::glad)

find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE Microsoft.GSL::GSL)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE glm::glm)