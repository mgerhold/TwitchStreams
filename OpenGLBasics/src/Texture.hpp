//
// Created by coder2k on 05.06.2021.
//

#pragma once

#include "Image.hpp"
#include "expected/expected.hpp"
#include <glad/glad.h>

class Texture final {
public:
    enum class Filtering {
        Linear,
        Nearest,
    };

    Texture() = default;
    Texture(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    ~Texture();

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& other) noexcept;


    void bind(GLint textureUnit = 0U) const noexcept;
    static void unbind(GLint textureUnit) noexcept;
    void setFiltering(Filtering filtering) const noexcept;
    void setWrap(bool enabled) const noexcept;
    int getWidth() const noexcept;
    int getHeight() const noexcept;
    int getNumChannels() const noexcept;

    [[nodiscard]] static tl::expected<Texture, std::string> Create(const Image& image) noexcept;
    [[nodiscard]] static GLint getTextureUnitCount() noexcept;

private:
    static void bind(GLuint textureName, GLint textureUnit) noexcept;

private:
    static inline GLint sTextureUnitCount{ 0U };
    int mWidth{ 0U };
    int mHeight{ 0U };
    int mNumChannels{ 0U };
    GLuint mName{ 0U };

    friend class Renderer;
};
