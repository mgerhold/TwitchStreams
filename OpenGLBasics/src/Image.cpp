//
// Created by coder2k on 05.06.2021.
//

#include "Image.hpp"
#include "stb_image/stb_image.h"
#include <spdlog/spdlog.h>

tl::expected<Image, std::string> Image::LoadFromFile(const std::filesystem::path& filename, int numChannels) {
    if (!std::filesystem::exists(filename)) {
        return tl::unexpected{ fmt::format("File not found: {}", filename.string()) };
    }
    Image result;
    // TODO: handle unicode filenames under Windows (see stb_image.h, line 181,
    //       and https://en.cppreference.com/w/cpp/filesystem/path/string)
    stbi_set_flip_vertically_on_load(true);
    result.mData = stbi_load(filename.string().c_str(),
                                    &result.mWidth,
                                    &result.mHeight,
                                    &result.mNumChannels,
                                    numChannels);
    if (result.mData == nullptr) {
        return tl::unexpected{ fmt::format("stb_image failed to load image: {}", stbi_failure_reason()) };
    }
    if (numChannels != 0) {
        result.mNumChannels = numChannels;
    }
    return result;
}

Image::Image(Image &&other) noexcept {
    using std::swap;
    swap(mWidth, other.mWidth);
    swap(mHeight, other.mHeight);
    swap(mNumChannels, other.mNumChannels);
    swap(mData, other.mData);
}

Image::~Image() {
    stbi_image_free(mData);
}

Image &Image::operator=(Image &&other) noexcept {
    using std::swap;
    swap(mWidth, other.mWidth);
    swap(mHeight, other.mHeight);
    swap(mNumChannels, other.mNumChannels);
    swap(mData, other.mData);
    return *this;
}

int Image::getWidth() const {
    return mWidth;
}

int Image::getHeight() const {
    return mHeight;
}

int Image::getNumChannels() const {
    return mNumChannels;
}

unsigned char *Image::getData() const {
    return mData;
}
