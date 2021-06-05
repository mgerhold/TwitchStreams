//
// Created by coder2k on 05.06.2021.
//

#pragma once

#include "expected/expected.hpp"
#include <memory>
#include <filesystem>
#include <string>

class Image final {
public:
    Image(const Image&) = delete;
    Image(Image&& other) noexcept;
    ~Image();

    Image& operator=(const Image&) = delete;
    Image& operator=(Image&& other) noexcept;

    [[nodiscard]] static tl::expected<Image, std::string> LoadFromFile(const std::filesystem::path& filename, int numChannels = 0);

    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] int getNumChannels() const;
    [[nodiscard]] unsigned char *getData() const;

private:
    Image() = default;

private:
    int mWidth{ 0 };
    int mHeight{ 0 };
    int mNumChannels{ 0 };
    unsigned char* mData{ nullptr };
};


