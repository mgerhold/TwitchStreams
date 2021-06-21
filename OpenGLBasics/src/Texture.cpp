//
// Created by coder2k on 05.06.2021.
//

#include "Texture.hpp"
#include <spdlog/spdlog.h>

tl::expected<Texture, std::string> Texture::Create(const Image& image) noexcept {
    GLint colorComponentFormat;
    const int numChannels = image.getNumChannels();
    switch (numChannels) {
        case 3:
            colorComponentFormat = GL_RGB;
            break;
        case 4:
            colorComponentFormat = GL_RGBA;
            break;
        default:
            return tl::unexpected{ fmt::format("Unsupported number of channels: {}", numChannels) };
    }

    Texture result;
    glGenTextures(1, &result.mName);
    result.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, colorComponentFormat, image.getWidth(), image.getHeight(), 0, colorComponentFormat,
                 GL_UNSIGNED_BYTE, image.getData());
    glGenerateMipmap(GL_TEXTURE_2D);
    result.mWidth = image.getWidth();
    result.mHeight = image.getHeight();
    result.mNumChannels = image.getNumChannels();
    result.setFiltering(Filtering::Linear);
    result.setWrap(false);
    return result;
}

void Texture::bind(GLint textureUnit) const noexcept {
    if (textureUnit < 0 || textureUnit >= getTextureUnitCount()) {
        spdlog::error("Cannot bind texture since {} is no valid texture unit.", textureUnit);
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, mName);
}

void Texture::unbind(GLint textureUnit) noexcept {
    if (textureUnit < 0 || textureUnit >= getTextureUnitCount()) {
        spdlog::error("Cannot unbind texture since {} is no valid texture unit.", textureUnit);
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLint Texture::getTextureUnitCount() noexcept {
    if (sTextureUnitCount != 0) {
        return sTextureUnitCount;
    }
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &sTextureUnitCount);
    return sTextureUnitCount;
}

void Texture::setFiltering(Texture::Filtering filtering) const noexcept {
    // TODO: Remove unnecessary binds
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering == Filtering::Linear ? GL_LINEAR : GL_NEAREST);
}

void Texture::setWrap(bool enabled) const noexcept {
    // TODO: Remove unnecessary binds
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, enabled ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, enabled ? GL_REPEAT : GL_CLAMP_TO_EDGE);
}

Texture::Texture(Texture&& other) noexcept {
    using std::swap;
    swap(mName, other.mName);
    swap(mWidth, other.mWidth);
    swap(mHeight, other.mHeight);
    swap(mNumChannels, other.mNumChannels);
}

Texture::~Texture() {
    glDeleteTextures(1, &mName);
}

Texture& Texture::operator=(Texture&& other) noexcept {
    using std::swap;
    swap(mName, other.mName);
    swap(mWidth, other.mWidth);
    swap(mHeight, other.mHeight);
    swap(mNumChannels, other.mNumChannels);
    return *this;
}

int Texture::getWidth() const noexcept {
    return mWidth;
}

int Texture::getHeight() const noexcept {
    return mHeight;
}

int Texture::getNumChannels() const noexcept {
    return mNumChannels;
}
