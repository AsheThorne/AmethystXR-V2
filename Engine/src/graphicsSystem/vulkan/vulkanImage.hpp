#pragma once
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/common/enums.h"
#include "../../assets/image.hpp"

// ----------------------------------------- //
// Vulkan Headers
// ----------------------------------------- //
#include <vulkan/vulkan.hpp>

/// Vulkan Image
class AxrVulkanImage {
public:
    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Vulkan buffer config
    struct Config {
        vk::PhysicalDevice PhysicalDevice;
        vk::Device Device;
        vk::CommandPool GraphicsCommandPool;
        vk::Queue GraphicsQueue;
        vk::DispatchLoaderDynamic* DispatchHandle;
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Default Constructor
    AxrVulkanImage();
    /// Constructor
    /// @param config Vulkan image config
    AxrVulkanImage(const Config& config);
    /// Copy Constructor
    /// @param src Source AxrVulkanImage to copy from
    AxrVulkanImage(const AxrVulkanImage& src) = delete;
    /// Move Constructor
    /// @param src Source AxrVulkanImage to move from
    AxrVulkanImage(AxrVulkanImage&& src) noexcept;

    // ---- Destructor ----

    /// Destructor
    ~AxrVulkanImage();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrVulkanImage to copy from
    AxrVulkanImage& operator=(const AxrVulkanImage& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrVulkanImage to move from
    AxrVulkanImage& operator=(AxrVulkanImage&& src) noexcept;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Check if the image exists
    /// @returns True if the image doesn't exist
    [[nodiscard]] bool isEmpty() const;
    /// Get the vulkan image
    /// @returns The vulkan image
    [[nodiscard]] const vk::Image& getImage() const;
    /// Get the vulkan image view
    /// @returns The vulkan image view
    [[nodiscard]] const vk::ImageView& getImageView() const;
    /// Get the image mip level count
    /// @returns The image mip level count
    [[nodiscard]] uint32_t getMipLevelCount() const;
    /// Get the image format
    /// @returns The image format
    [[nodiscard]] vk::Format getImageFormat() const;

    /// Create vulkan image resources for the given AxrImage
    /// @param image Source image
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createImage(AxrImageConst_T image);
    /// Create vulkan image resources
    /// @param extent Image extent
    /// @param sampleCount Image sample count
    /// @param format Image format
    /// @param imageTiling Image tiling option
    /// @param imageUsage Image usage flags
    /// @param memoryProperties Image memory property flags
    /// @param imageAspect Image aspect flags
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createImage(
        vk::Extent2D extent,
        vk::SampleCountFlagBits sampleCount,
        vk::Format format,
        vk::ImageTiling imageTiling,
        vk::ImageUsageFlags imageUsage,
        vk::MemoryPropertyFlags memoryProperties,
        vk::ImageAspectFlags imageAspect
    );
    /// Destroy the image
    void destroyImage();

    /// Transition the images layout
    /// @param srcAccessMask Image memory barrier src access mask
    /// @param dstAccessMask Image memory barrier dst access mask
    /// @param oldLayout Old image layout
    /// @param newLayout New image layout
    /// @param srcStageMask Pipeline barrier src stage mask
    /// @param dstStageMask Pipeline barrier dst stage mask
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult transitionImageLayout(
        vk::AccessFlags srcAccessMask,
        vk::AccessFlags dstAccessMask,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        vk::PipelineStageFlagBits srcStageMask,
        vk::PipelineStageFlagBits dstStageMask
    ) const;

    // ----------------------------------------- //
    // Public Static Functions
    // ----------------------------------------- //

    /// Convert the given image format and color channels to a vk::Format
    /// @param imageFormat Image format to use
    /// @param colorChannels Color channels to use
    /// @returns The vk::Format
    [[nodiscard]] static vk::Format toVkFormat(
        AxrImageFormatEnum imageFormat,
        AxrImageColorChannelsEnum colorChannels
    );

    /// Create a vulkan image
    /// @param device Device to use
    /// @param physicalDevice Physical device to use
    /// @param extent Image extent
    /// @param mipLevelCount Image mip level count
    /// @param sampleCount Image sample count
    /// @param format Image format
    /// @param imageTiling Image tiling
    /// @param imageUsage Image usage
    /// @param memoryProperties Image memory properties
    /// @param image Output created image
    /// @param imageMemory Output image memory
    /// @param dispatch Dispatch to use
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] static AxrResult createImage(
        vk::Device device,
        vk::PhysicalDevice physicalDevice,
        vk::Extent2D extent,
        uint32_t mipLevelCount,
        vk::SampleCountFlagBits sampleCount,
        vk::Format format,
        vk::ImageTiling imageTiling,
        vk::ImageUsageFlags imageUsage,
        vk::MemoryPropertyFlags memoryProperties,
        vk::Image& image,
        vk::DeviceMemory& imageMemory,
        const vk::DispatchLoaderDynamic& dispatch
    );
    /// Destroy a vulkan image
    /// @param device Device to use
    /// @param image Image to destroy
    /// @param imageMemory Image memory to destroy
    /// @param dispatch Dispatch to use
    static void destroyImage(
        vk::Device device,
        vk::Image& image,
        vk::DeviceMemory& imageMemory,
        const vk::DispatchLoaderDynamic& dispatch
    );

    /// Create an image view for the given image
    /// @param device Device to use
    /// @param image Image to create image view from
    /// @param format Format to use
    /// @param aspectMask Aspect mask to use
    /// @param mipLevelCount Mip level count to use
    /// @param imageView Output image view
    /// @param dispatch Dispatch to use
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] static AxrResult createImageView(
        vk::Device device,
        const vk::Image& image,
        vk::Format format,
        vk::ImageAspectFlags aspectMask,
        uint32_t mipLevelCount,
        vk::ImageView& imageView,
        const vk::DispatchLoaderDynamic& dispatch
    );
    /// Destroy the given image view
    /// @param device Device to use
    /// @param imageView Image view to destroy
    /// @param dispatch Dispatch to use
    static void destroyImageView(
        vk::Device device,
        vk::ImageView& imageView,
        const vk::DispatchLoaderDynamic& dispatch
    );

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_Device;
    vk::CommandPool m_GraphicsCommandPool;
    vk::Queue m_GraphicsQueue;
    vk::DispatchLoaderDynamic* m_DispatchHandle;

    vk::Image m_Image;
    vk::ImageView m_ImageView;
    vk::DeviceMemory m_ImageMemory;
    vk::ImageAspectFlags m_ImageAspectFlags;
    uint32_t m_MipLevelCount;
    vk::Format m_ImageFormat;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Clean up this class
    void cleanup();

    /// Count the number of mip levels for the given dimensions
    /// @param width Image width
    /// @param height Image height
    /// @returns The number of mip levels
    [[nodiscard]] uint32_t countImageMipLevels(uint32_t width, uint32_t height) const;

    /// Copy the given buffer to the given image
    /// @param buffer Buffer
    /// @param image Image
    /// @param imageWidth Image width
    /// @param imageHeight Image height
    /// @param imageFormat Image format
    /// @param mipLevelCount Mip level count
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult copyBufferToImage(
        vk::Buffer buffer,
        vk::Image image,
        uint32_t imageWidth,
        uint32_t imageHeight,
        vk::Format imageFormat,
        uint32_t mipLevelCount
    ) const;

    /// Generate mipmaps and set the image layout to the finalImageLayout
    /// @param commandBuffer Command buffer to use
    /// @param image Image
    /// @param imageWidth Image width
    /// @param imageHeight Image height
    /// @param imageFormat Image format
    /// @param mipLevelCount Mip level count
    /// @param initialImageAccessMask Initial image memory barrier access mask
    /// @param finalImageAccessMask Final image memory barrier access mask
    /// @param initialImageLayout Initial image layout
    /// @param finalImageLayout Final image layout
    /// @param initialImageStageMask Initial image pipeline barrier stage mask
    /// @param finalImageStageMask Final image pipeline barrier stage mask
    void generateMipmaps(
        vk::CommandBuffer commandBuffer,
        vk::Image image,
        uint32_t imageWidth,
        uint32_t imageHeight,
        vk::Format imageFormat,
        uint32_t mipLevelCount,
        vk::AccessFlags initialImageAccessMask,
        vk::AccessFlags finalImageAccessMask,
        vk::ImageLayout initialImageLayout,
        vk::ImageLayout finalImageLayout,
        vk::PipelineStageFlagBits initialImageStageMask,
        vk::PipelineStageFlagBits finalImageStageMask
    ) const;
};

#endif
