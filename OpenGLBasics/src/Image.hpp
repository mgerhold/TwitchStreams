//
// Created by coder2k on 05.06.2021.
//

#pragma once

#include "expected/expected.hpp"
#include "stb_image/stb_image.h"
#include <memory>
#include <filesystem>
#include <string>

class Image final {
public:
    Image(const Image&) = delete;
    Image(Image&& other) noexcept;

    Image& operator=(const Image&) = delete;
    Image& operator=(Image&& other) noexcept;

    [[nodiscard]] static tl::expected<Image, std::string> LoadFromFile(const std::filesystem::path& filename,
                                                                       int numChannels = 0) noexcept;

    [[nodiscard]] int getWidth() const noexcept;
    [[nodiscard]] int getHeight() const noexcept;
    [[nodiscard]] int getNumChannels() const noexcept;
    [[nodiscard]] unsigned char* getData() const noexcept;

private:
    struct Deleter {
        void operator()(unsigned char* const data);
    };
    using Pointer = std::unique_ptr<unsigned char, Deleter>;

private:
    Image() = default;

private:
    int mWidth{ 0 };
    int mHeight{ 0 };
    int mNumChannels{ 0 };
    Pointer mData{ nullptr };
};
