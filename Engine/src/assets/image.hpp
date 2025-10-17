#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/assets.h"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <string>
#include <vector>

// ----------------------------------------- //
// STB Headers
// ----------------------------------------- //
#include <stb_image.h>

/// Axr Image
class AxrImage {
public:
    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Default Constructor
    AxrImage();
    /// Constructor
    /// @param config Image config
    AxrImage(const AxrImageConfig& config);
    /// Copy Constructor
    /// @param src Source AxrImage to copy from
    AxrImage(const AxrImage& src);
    /// Move Constructor
    /// @param src Source AxrImage to move from
    AxrImage(AxrImage&& src) noexcept;

    // ---- Destructor ----

    /// Destructor
    ~AxrImage();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrImage to copy from
    AxrImage& operator=(const AxrImage& src);
    /// Move Assignment Operator
    /// @param src Source AxrImage to move from
    AxrImage& operator=(AxrImage&& src) noexcept;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Get the image name
    /// @returns The name of the image
    [[nodiscard]] const std::string& getName() const;
    /// Get the image format
    /// @returns The image format
    [[nodiscard]] AxrImageFormatEnum getFormat() const;

    /// Set the image data
    /// @param width Image width
    /// @param height Image height
    /// @param colorChannels Image number of color channels
    /// @param data Image data. Stored from left-to-right, top-to-bottom. Each pixel contains a value for each 'colorChannel', stored with 8-bits
    /// per channel, in the following order: 1=Y, 2=YA, 3=RGB, 4=RGBA. (Y is monochrome color.)
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setData(uint32_t width, uint32_t height, AxrImageColorChannelsEnum colorChannels, const stbi_uc* data);

    // ---- For Internal Use ----
    // These functions are only to be used internally in the AmethystXr engine.
    // They have not been given a publicly accessible function in the 'include headers' to be used by an application.

    /// Check if this image is loaded
    /// @returns True if this image is loaded
    [[nodiscard]] bool isLoaded() const;
    /// Load this image
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadFile() const;
    /// Unload this image
    void unloadFile() const;

    /// Get the image pixel data
    /// @returns the image pixel data
    [[nodiscard]] const std::vector<stbi_uc>& getPixelData() const;
    /// Get the image width
    /// @returns the image width
    [[nodiscard]] uint32_t getWidth() const;
    /// Get the image height
    /// @returns the image height
    [[nodiscard]] uint32_t getHeight() const;
    /// Get the image color channels
    /// @returns the image color channels
    [[nodiscard]] AxrImageColorChannelsEnum getColorChannels() const;

private:
    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Image Data
    struct Data {
        // ---- Public Variables ----

        uint32_t Width = 0;
        uint32_t Height = 0;
        AxrImageColorChannelsEnum ColorChannels = AXR_IMAGE_COLOR_CHANNELS_UNDEFINED;
        std::vector<stbi_uc> Pixels;

        // ---- Constructors ----

        /// Constructor
        Data() = default;
        /// Copy Constructor
        /// @param src Source Data to copy from
        Data(const Data& src) = default;
        /// Move Constructor
        /// @param src Source Data to move from
        Data(Data&& src) noexcept {
            Pixels = std::move(src.Pixels);

            Width = src.Width;
            Height = src.Height;
            ColorChannels = src.ColorChannels;

            src.Width = 0;
            src.Height = 0;
            src.ColorChannels = AXR_IMAGE_COLOR_CHANNELS_UNDEFINED;
        }

        // ---- Destructor ----

        /// Destructor
        ~Data() {
            clear();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source Data to copy from
        Data& operator=(const Data& src) = default;
        /// Move Assignment Operator
        /// @param src Source Data to move from
        Data& operator=(Data&& src) noexcept {
            if (this != &src) {
                clear();

                Pixels = std::move(src.Pixels);

                Width = src.Width;
                Height = src.Height;
                ColorChannels = src.ColorChannels;

                src.Width = 0;
                src.Height = 0;
                src.ColorChannels = AXR_IMAGE_COLOR_CHANNELS_UNDEFINED;
            }

            return *this;
        }

        // ---- Public Functions ----

        /// Clear data
        void clear() {
            Width = 0;
            Height = 0;
            ColorChannels = AXR_IMAGE_COLOR_CHANNELS_UNDEFINED;
            Pixels.clear();
        }
    };

    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    std::string m_Name;
    std::string m_FilePath;
    AxrImageFormatEnum m_Format = AXR_IMAGE_FORMAT_UNDEFINED;

    mutable Data m_Data;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Clean up this class
    void cleanup();

    /// Convert a given image from srcColorChannels to dstColorChannels.
    /// @param width Image width
    /// @param height Image height
    /// @param srcColorChannels Source color channels
    /// @param dstColorChannels Color channels to convert to
    /// @param data Source image data
    /// @returns The converted image data
    std::vector<stbi_uc> convertColorChannels(
        uint32_t width,
        uint32_t height,
        AxrImageColorChannelsEnum srcColorChannels,
        AxrImageColorChannelsEnum dstColorChannels,
        const stbi_uc* data
    ) const;

    /// Convert the given RGB values to a single gray scale value
    /// @param red Red
    /// @param green Green
    /// @param blue Blue
    stbi_uc toGrayScale(int32_t red, int32_t green, int32_t blue) const;

    /// Load the image at the given path
    /// @param path Image path
    /// @param imageData Output image data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] static AxrResult loadImage(const std::string& path, Data& imageData);
};
