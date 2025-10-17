// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "image.hpp"
#include "assetsUtils.hpp"
#include "axr/logger.h"

// ----------------------------------------- //
// External Functions
// ----------------------------------------- //

const char* axrImageGetName(const AxrImageConst_T image) {
    if (image == nullptr) {
        axrLogErrorLocation("`image` is null.");
        return "";
    }

    return image->getName().c_str();
}

AxrResult axrImageSetData(
    const AxrImage_T image,
    const uint32_t width,
    const uint32_t height,
    const AxrImageColorChannelsEnum colorChannels,
    const stbi_uc* data
) {
    if (image == nullptr) {
        axrLogErrorLocation("`image` is null.");
        return AXR_ERROR;
    }

    return image->setData(width, height, colorChannels, data);
}

// ----------------------------------------- //
// Internal Functions
// ----------------------------------------- //

// ---- Special Functions ----

AxrImage::AxrImage() = default;

AxrImage::AxrImage(const AxrImageConfig& config):
    m_Name(config.Name),
    m_FilePath(config.FilePath),
    m_Format(config.Format) {
    if (!m_FilePath.empty() && !axrFileExists(m_FilePath)) {
        axrLogErrorLocation("File path is invalid.");
    }
}

AxrImage::AxrImage(const AxrImage& src) {
    m_Name = src.m_Name;
    m_FilePath = src.m_FilePath;
    m_Format = src.m_Format;
    m_Data = src.m_Data;
}

AxrImage::AxrImage(AxrImage&& src) noexcept {
    m_Name = std::move(src.m_Name);
    m_FilePath = std::move(src.m_FilePath);
    m_Data = std::move(src.m_Data);

    m_Format = src.m_Format;

    src.m_Format = AXR_IMAGE_FORMAT_UNDEFINED;
}

AxrImage::~AxrImage() {
    cleanup();
}

AxrImage& AxrImage::operator=(const AxrImage& src) {
    if (this != &src) {
        cleanup();

        m_Name = src.m_Name;
        m_FilePath = src.m_FilePath;
        m_Format = src.m_Format;
        m_Data = src.m_Data;
    }

    return *this;
}

AxrImage& AxrImage::operator=(AxrImage&& src) noexcept {
    if (this != &src) {
        cleanup();

        m_Name = std::move(src.m_Name);
        m_FilePath = std::move(src.m_FilePath);
        m_Data = std::move(src.m_Data);

        m_Format = src.m_Format;

        src.m_Format = AXR_IMAGE_FORMAT_UNDEFINED;
    }

    return *this;
}

// ---- Public Functions ----

const std::string& AxrImage::getName() const {
    return m_Name;
}

AxrImageFormatEnum AxrImage::getFormat() const {
    return m_Format;
}

AxrResult AxrImage::setData(
    const uint32_t width,
    const uint32_t height,
    const AxrImageColorChannelsEnum colorChannels,
    const stbi_uc* data
) {
    if (data == nullptr) {
        axrLogErrorLocation("data is null.");
        return AXR_ERROR;
    }

    m_Data.Width = width;
    m_Data.Height = height;
    m_Data.ColorChannels = AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA;
    m_Data.Pixels = convertColorChannels(
        m_Data.Width,
        m_Data.Height,
        colorChannels,
        m_Data.ColorChannels,
        data
    );

    return AXR_SUCCESS;
}

bool AxrImage::isLoaded() const {
    return !m_Data.Pixels.empty();
}

AxrResult AxrImage::loadFile() const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (isLoaded()) {
        return AXR_SUCCESS;
    }

    if (m_FilePath.empty()) {
        axrLogErrorLocation("File path is empty.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    return loadImage(m_FilePath, m_Data);
}

void AxrImage::unloadFile() const {
    // Don't clear the image data if there is no original data file to be loaded again
    if (m_FilePath.empty()) return;

    m_Data.clear();
}

const std::vector<stbi_uc>& AxrImage::getPixelData() const {
    return m_Data.Pixels;
}

uint32_t AxrImage::getWidth() const {
    return m_Data.Width;
}

uint32_t AxrImage::getHeight() const {
    return m_Data.Height;
}

AxrImageColorChannelsEnum AxrImage::getColorChannels() const {
    return m_Data.ColorChannels;
}

// ---- Private Functions ----

void AxrImage::cleanup() {
    m_Name.clear();
    m_FilePath.clear();
    m_Format = AXR_IMAGE_FORMAT_UNDEFINED;
    m_Data.clear();
}

std::vector<stbi_uc> AxrImage::convertColorChannels(
    const uint32_t width,
    const uint32_t height,
    const AxrImageColorChannelsEnum srcColorChannels,
    const AxrImageColorChannelsEnum dstColorChannels,
    const stbi_uc* data
) const {
    // ---- Based off STB image library's stbi__convert_format() function. ----

    if (srcColorChannels == dstColorChannels)
        return std::vector(data, data + width * height * srcColorChannels);

    std::vector<stbi_uc> convertedData(width * height * dstColorChannels);

    for (uint32_t y = 0; y < height; ++y) {
        const stbi_uc* src = data + y * width * srcColorChannels;
        stbi_uc* dest = convertedData.data() + y * width * dstColorChannels;

#define AXR_COMBO(a,b)  ((a)*8+(b))
#define AXR_CASE(a, b) case AXR_COMBO(a, b): for(uint32_t x = 0; x < width; ++x, src += a, dest += b)

        // convert source image with srcColorChannels components to one with dstColorChannels components;
        // avoid switch per pixel, so use switch per scanline and massive macros
        switch (AXR_COMBO(srcColorChannels, dstColorChannels)) {
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_GRAY, AXR_IMAGE_COLOR_CHANNELS_GRAY_ALPHA) {
                    dest[0] = src[0];
                    dest[1] = 255;
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_GRAY, AXR_IMAGE_COLOR_CHANNELS_RGB) {
                    dest[0] = dest[1] = dest[2] = src[0];
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_GRAY, AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA) {
                    dest[0] = dest[1] = dest[2] = src[0];
                    dest[3] = 255;
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_GRAY_ALPHA, AXR_IMAGE_COLOR_CHANNELS_GRAY) {
                    dest[0] = src[0];
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_GRAY_ALPHA, AXR_IMAGE_COLOR_CHANNELS_RGB) {
                    dest[0] = dest[1] = dest[2] = src[0];
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_GRAY_ALPHA, AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA) {
                    dest[0] = dest[1] = dest[2] = src[0];
                    dest[3] = src[1];
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_RGB, AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA) {
                    dest[0] = src[0];
                    dest[1] = src[1];
                    dest[2] = src[2];
                    dest[3] = 255;
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_RGB, AXR_IMAGE_COLOR_CHANNELS_GRAY) {
                    dest[0] = toGrayScale(src[0], src[1], src[2]);
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_RGB, AXR_IMAGE_COLOR_CHANNELS_GRAY_ALPHA) {
                    dest[0] = toGrayScale(src[0], src[1], src[2]);
                    dest[1] = 255;
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA, AXR_IMAGE_COLOR_CHANNELS_GRAY) {
                    dest[0] = toGrayScale(src[0], src[1], src[2]);
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA, AXR_IMAGE_COLOR_CHANNELS_GRAY_ALPHA) {
                    dest[0] = toGrayScale(src[0], src[1], src[2]);
                    dest[1] = src[3];
                }
                break;
            AXR_CASE(AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA, AXR_IMAGE_COLOR_CHANNELS_RGB) {
                    dest[0] = src[0];
                    dest[1] = src[1];
                    dest[2] = src[2];
                }
                break;
            default: {
                axrLogErrorLocation("Unsupported channel count.");
                return {};
            }
        }
    }

#undef AXR_CASE
#undef AXR_COMBO

    return convertedData;
}

stbi_uc AxrImage::toGrayScale(const int32_t red, const int32_t green, const int32_t blue) const {
    // ---- Based off the STB image library's stbi__compute_y() function ----
    return static_cast<stbi_uc>(((red * 77) + (green * 150) + (29 * blue)) >> 8);
}

AxrResult AxrImage::loadImage(const std::string& path, Data& imageData) {
    int width;
    int height;
    int colorChannels;
    const std::filesystem::path fullPath = axrGetAbsolutePathToAsset(path);

    stbi_uc* pixelData = stbi_load(
        fullPath.string().c_str(),
        &width,
        &height,
        &colorChannels,
        STBI_rgb_alpha
    );

    if (pixelData == nullptr) {
        axrLogErrorLocation("Failed to read image data. {0}", stbi_failure_reason());
        return AXR_ERROR;
    }

    imageData.Width = width;
    imageData.Height = height;
    imageData.ColorChannels = axrToImageColorChannels(STBI_rgb_alpha);

    const size_t pixelsSize =
        static_cast<uint64_t>(imageData.Width) *
        static_cast<uint64_t>(imageData.Height) *
        imageData.ColorChannels;

    imageData.Pixels.assign(pixelData, pixelData + pixelsSize);

    stbi_image_free(pixelData);
    return AXR_SUCCESS;
}
