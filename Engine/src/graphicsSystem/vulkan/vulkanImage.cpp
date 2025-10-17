#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "vulkanImage.hpp"
#include "axr/logger.h"
#include "vulkanBuffer.hpp"
#include "vulkanUtils.hpp"

// ---- Special Functions ----

AxrVulkanImage::AxrVulkanImage():
    m_PhysicalDevice(VK_NULL_HANDLE),
    m_Device(VK_NULL_HANDLE),
    m_GraphicsCommandPool(VK_NULL_HANDLE),
    m_GraphicsQueue(VK_NULL_HANDLE),
    m_DispatchHandle(VK_NULL_HANDLE),
    m_Image(VK_NULL_HANDLE),
    m_ImageView(VK_NULL_HANDLE),
    m_ImageMemory(VK_NULL_HANDLE),
    m_ImageAspectFlags(vk::ImageAspectFlags()),
    m_MipLevelCount(0),
    m_ImageFormat(vk::Format::eUndefined) {
}

AxrVulkanImage::AxrVulkanImage(const Config& config):
    m_PhysicalDevice(config.PhysicalDevice),
    m_Device(config.Device),
    m_GraphicsCommandPool(config.GraphicsCommandPool),
    m_GraphicsQueue(config.GraphicsQueue),
    m_DispatchHandle(config.DispatchHandle),
    m_Image(VK_NULL_HANDLE),
    m_ImageView(VK_NULL_HANDLE),
    m_ImageMemory(VK_NULL_HANDLE),
    m_ImageAspectFlags(vk::ImageAspectFlags()),
    m_MipLevelCount(0),
    m_ImageFormat(vk::Format::eUndefined) {
}

AxrVulkanImage::AxrVulkanImage(AxrVulkanImage&& src) noexcept {
    m_PhysicalDevice = src.m_PhysicalDevice;
    m_Device = src.m_Device;
    m_GraphicsCommandPool = src.m_GraphicsCommandPool;
    m_GraphicsQueue = src.m_GraphicsQueue;
    m_DispatchHandle = src.m_DispatchHandle;
    m_Image = src.m_Image;
    m_ImageView = src.m_ImageView;
    m_ImageMemory = src.m_ImageMemory;
    m_ImageAspectFlags = src.m_ImageAspectFlags;
    m_MipLevelCount = src.m_MipLevelCount;
    m_ImageFormat = src.m_ImageFormat;

    src.m_PhysicalDevice = VK_NULL_HANDLE;
    src.m_Device = VK_NULL_HANDLE;
    src.m_GraphicsCommandPool = VK_NULL_HANDLE;
    src.m_GraphicsQueue = VK_NULL_HANDLE;
    src.m_DispatchHandle = VK_NULL_HANDLE;
    src.m_Image = VK_NULL_HANDLE;
    src.m_ImageView = VK_NULL_HANDLE;
    src.m_ImageMemory = VK_NULL_HANDLE;
    src.m_ImageAspectFlags = vk::ImageAspectFlags();
    src.m_MipLevelCount = 0;
    src.m_ImageFormat = vk::Format::eUndefined;
}

AxrVulkanImage::~AxrVulkanImage() {
    cleanup();
}

AxrVulkanImage& AxrVulkanImage::operator=(AxrVulkanImage&& src) noexcept {
    if (this != &src) {
        cleanup();

        m_PhysicalDevice = src.m_PhysicalDevice;
        m_Device = src.m_Device;
        m_GraphicsCommandPool = src.m_GraphicsCommandPool;
        m_GraphicsQueue = src.m_GraphicsQueue;
        m_DispatchHandle = src.m_DispatchHandle;
        m_Image = src.m_Image;
        m_ImageView = src.m_ImageView;
        m_ImageMemory = src.m_ImageMemory;
        m_ImageAspectFlags = src.m_ImageAspectFlags;
        m_MipLevelCount = src.m_MipLevelCount;
        m_ImageFormat = src.m_ImageFormat;

        src.m_PhysicalDevice = VK_NULL_HANDLE;
        src.m_Device = VK_NULL_HANDLE;
        src.m_GraphicsCommandPool = VK_NULL_HANDLE;
        src.m_GraphicsQueue = VK_NULL_HANDLE;
        src.m_DispatchHandle = VK_NULL_HANDLE;
        src.m_Image = VK_NULL_HANDLE;
        src.m_ImageView = VK_NULL_HANDLE;
        src.m_ImageMemory = VK_NULL_HANDLE;
        src.m_ImageAspectFlags = vk::ImageAspectFlags();
        src.m_MipLevelCount = 0;
        src.m_ImageFormat = vk::Format::eUndefined;
    }

    return *this;
}

// ---- Public Functions ----

bool AxrVulkanImage::isEmpty() const {
    return m_Image == VK_NULL_HANDLE &&
        m_ImageView == VK_NULL_HANDLE &&
        m_ImageMemory == VK_NULL_HANDLE &&
        m_ImageFormat == vk::Format::eUndefined;
}

const vk::Image& AxrVulkanImage::getImage() const {
    return m_Image;
}

const vk::ImageView& AxrVulkanImage::getImageView() const {
    return m_ImageView;
}

uint32_t AxrVulkanImage::getMipLevelCount() const {
    return m_MipLevelCount;
}

vk::Format AxrVulkanImage::getImageFormat() const {
    return m_ImageFormat;
}

AxrResult AxrVulkanImage::createImage(const AxrImageConst_T image) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (image == nullptr) {
        axrLogErrorLocation("Image is null.");
        return AXR_ERROR;
    }

    if (m_DispatchHandle == nullptr) {
        axrLogErrorLocation("DispatchHandle is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    if (!image->isLoaded()) {
        axrResult = image->loadFile();

        if (AXR_FAILED(axrResult)) {
            destroyImage();
            return axrResult;
        }
    }

    const vk::DeviceSize imageSizeInBytes = image->getPixelData().size() * sizeof(stbi_uc);

    const AxrVulkanBuffer::Config bufferConfig{
        .PhysicalDevice = m_PhysicalDevice,
        .Device = m_Device,
        .TransferCommandPool = m_GraphicsCommandPool,
        .TransferQueue = m_GraphicsQueue,
        .DispatchHandle = m_DispatchHandle,
    };
    AxrVulkanBuffer buffer(bufferConfig);
    axrResult = buffer.createBuffer(false, imageSizeInBytes, vk::BufferUsageFlagBits::eTransferSrc);
    if (AXR_FAILED(axrResult)) {
        buffer.destroyBuffer();
        destroyImage();
        return axrResult;
    }

    axrResult = buffer.setBufferData(0, imageSizeInBytes, image->getPixelData().data());
    if (AXR_FAILED(axrResult)) {
        buffer.destroyBuffer();
        destroyImage();
        return axrResult;
    }

    m_MipLevelCount = countImageMipLevels(image->getWidth(), image->getHeight());
    m_ImageFormat = toVkFormat(image->getFormat(), image->getColorChannels());
    vk::ImageUsageFlags imageUsageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    if (m_MipLevelCount > 1) {
        imageUsageFlags |= vk::ImageUsageFlagBits::eTransferSrc;
    }

    axrResult = createImage(
        m_Device,
        m_PhysicalDevice,
        vk::Extent2D(image->getWidth(), image->getHeight()),
        m_MipLevelCount,
        vk::SampleCountFlagBits::e1,
        m_ImageFormat,
        vk::ImageTiling::eOptimal,
        imageUsageFlags,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        m_Image,
        m_ImageMemory,
        *m_DispatchHandle
    );
    if (AXR_FAILED(axrResult)) {
        buffer.destroyBuffer();
        destroyImage();
        return axrResult;
    }

    axrResult = copyBufferToImage(
        buffer.getBuffer(),
        m_Image,
        image->getWidth(),
        image->getHeight(),
        m_ImageFormat,
        m_MipLevelCount
    );
    // We're done with the buffer now
    buffer.destroyBuffer();

    if (AXR_FAILED(axrResult)) {
        destroyImage();
        return axrResult;
    }

    m_ImageAspectFlags = vk::ImageAspectFlagBits::eColor;

    axrResult = createImageView(
        m_Device,
        m_Image,
        m_ImageFormat,
        m_ImageAspectFlags,
        m_MipLevelCount,
        m_ImageView,
        *m_DispatchHandle
    );
    if (AXR_FAILED(axrResult)) {
        destroyImage();
        return axrResult;
    }

    return AXR_SUCCESS;
}

AxrResult AxrVulkanImage::createImage(
    const vk::Extent2D extent,
    const vk::SampleCountFlagBits sampleCount,
    const vk::Format format,
    const vk::ImageTiling imageTiling,
    const vk::ImageUsageFlags imageUsage,
    const vk::MemoryPropertyFlags memoryProperties,
    const vk::ImageAspectFlags imageAspect
) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_DispatchHandle == nullptr) {
        axrLogErrorLocation("DispatchHandle is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;
    m_MipLevelCount = 1;
    m_ImageFormat = format;

    axrResult = createImage(
        m_Device,
        m_PhysicalDevice,
        extent,
        m_MipLevelCount,
        sampleCount,
        m_ImageFormat,
        imageTiling,
        imageUsage,
        memoryProperties,
        m_Image,
        m_ImageMemory,
        *m_DispatchHandle
    );
    if (AXR_FAILED(axrResult)) {
        destroyImage();
        return axrResult;
    }

    m_ImageAspectFlags = imageAspect;

    axrResult = createImageView(
        m_Device,
        m_Image,
        m_ImageFormat,
        m_ImageAspectFlags,
        m_MipLevelCount,
        m_ImageView,
        *m_DispatchHandle
    );
    if (AXR_FAILED(axrResult)) {
        destroyImage();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanImage::destroyImage() {
    if (m_Image == VK_NULL_HANDLE &&
        m_ImageMemory == VK_NULL_HANDLE &&
        m_ImageView == VK_NULL_HANDLE)
        return;

    if (m_DispatchHandle == nullptr) {
        axrLogErrorLocation("Failed to destroy image. Dispatch is null.");
        return;
    }

    m_ImageFormat = vk::Format::eUndefined;
    m_MipLevelCount = 0;
    m_ImageAspectFlags = vk::ImageAspectFlags();
    destroyImageView(m_Device, m_ImageView, *m_DispatchHandle);
    destroyImage(m_Device, m_Image, m_ImageMemory, *m_DispatchHandle);
}

AxrResult AxrVulkanImage::transitionImageLayout(
    const vk::AccessFlags srcAccessMask,
    const vk::AccessFlags dstAccessMask,
    const vk::ImageLayout oldLayout,
    const vk::ImageLayout newLayout,
    const vk::PipelineStageFlagBits srcStageMask,
    const vk::PipelineStageFlagBits dstStageMask
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    vk::CommandBuffer commandBuffer;
    axrResult = axrBeginSingleTimeCommand(
        m_Device,
        m_GraphicsCommandPool,
        commandBuffer,
        *m_DispatchHandle
    );
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    const vk::ImageMemoryBarrier imageMemoryBarrier(
        srcAccessMask,
        dstAccessMask,
        oldLayout,
        newLayout,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        m_Image,
        vk::ImageSubresourceRange(
            m_ImageAspectFlags,
            0,
            m_MipLevelCount,
            0,
            1
        )
    );

    commandBuffer.pipelineBarrier(
        srcStageMask,
        dstStageMask,
        vk::DependencyFlags(),
        0,
        nullptr,
        0,
        nullptr,
        1,
        &imageMemoryBarrier,
        *m_DispatchHandle
    );

    axrResult = axrEndSingleTimeCommand(
        m_Device,
        m_GraphicsCommandPool,
        m_GraphicsQueue,
        commandBuffer,
        *m_DispatchHandle
    );
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    return AXR_SUCCESS;
}

vk::Format AxrVulkanImage::toVkFormat(
    const AxrImageFormatEnum imageFormat,
    const AxrImageColorChannelsEnum colorChannels
) {
    switch (imageFormat) {
        case AXR_IMAGE_FORMAT_SRGB: {
            switch (colorChannels) {
                case AXR_IMAGE_COLOR_CHANNELS_GRAY: {
                    return vk::Format::eR8Srgb;
                }
                case AXR_IMAGE_COLOR_CHANNELS_GRAY_ALPHA: {
                    return vk::Format::eR8G8Srgb;
                }
                case AXR_IMAGE_COLOR_CHANNELS_RGB: {
                    return vk::Format::eR8G8B8Srgb;
                }
                case AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA: {
                    return vk::Format::eR8G8B8A8Srgb;
                }
                case AXR_IMAGE_COLOR_CHANNELS_UNDEFINED:
                default: {
                    axrLogErrorLocation("Unknown image color channels.");
                    return vk::Format::eUndefined;
                }
            }
        }
        case AXR_IMAGE_FORMAT_UNORM: {
            switch (colorChannels) {
                case AXR_IMAGE_COLOR_CHANNELS_GRAY: {
                    return vk::Format::eR8Unorm;
                }
                case AXR_IMAGE_COLOR_CHANNELS_GRAY_ALPHA: {
                    return vk::Format::eR8G8Unorm;
                }
                case AXR_IMAGE_COLOR_CHANNELS_RGB: {
                    return vk::Format::eR8G8B8Unorm;
                }
                case AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA: {
                    return vk::Format::eR8G8B8A8Unorm;
                }
                case AXR_IMAGE_COLOR_CHANNELS_UNDEFINED:
                default: {
                    axrLogErrorLocation("Unknown image color channels.");
                    return vk::Format::eUndefined;
                }
            }
        }
        case AXR_IMAGE_FORMAT_UNDEFINED:
        default: {
            axrLogErrorLocation("Unknown image format.");
            return vk::Format::eUndefined;
        }
    }
}

AxrResult AxrVulkanImage::createImage(
    const vk::Device device,
    const vk::PhysicalDevice physicalDevice,
    const vk::Extent2D extent,
    const uint32_t mipLevelCount,
    const vk::SampleCountFlagBits sampleCount,
    const vk::Format format,
    const vk::ImageTiling imageTiling,
    const vk::ImageUsageFlags imageUsage,
    const vk::MemoryPropertyFlags memoryProperties,
    vk::Image& image,
    vk::DeviceMemory& imageMemory,
    const vk::DispatchLoaderDynamic& dispatch
) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (image != VK_NULL_HANDLE || imageMemory != VK_NULL_HANDLE) {
        axrLogErrorLocation("Image already exists.");
        return AXR_ERROR;
    }

    if (device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Device is null.");
        return AXR_ERROR;
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        axrLogErrorLocation("PhysicalDevice is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const vk::ImageCreateInfo imageCreateInfo(
        {},
        vk::ImageType::e2D,
        format,
        vk::Extent3D(extent, 1),
        mipLevelCount,
        1,
        sampleCount,
        imageTiling,
        imageUsage,
        vk::SharingMode::eExclusive,
        {},
        {},
        vk::ImageLayout::eUndefined
    );
    vk::Result vkResult = device.createImage(&imageCreateInfo, nullptr, &image, dispatch);
    axrLogVkResult(vkResult, "device.createImage");
    if (VK_FAILED(vkResult)) {
        destroyImage(device, image, imageMemory, dispatch);
        return AXR_ERROR;
    }

    const vk::MemoryRequirements memoryRequirements = device.getImageMemoryRequirements(image, dispatch);
    uint32_t memoryTypeIndex = 0;
    const AxrResult axrResult = axrFindMemoryTypeIndex(
        physicalDevice,
        memoryRequirements.memoryTypeBits,
        memoryProperties,
        memoryTypeIndex,
        dispatch
    );
    if (AXR_FAILED(axrResult)) {
        destroyImage(device, image, imageMemory, dispatch);
        return AXR_ERROR;
    }

    const vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        memoryTypeIndex
    );
    vkResult = device.allocateMemory(
        &memoryAllocateInfo,
        nullptr,
        &imageMemory,
        dispatch
    );
    axrLogVkResult(vkResult, "device.allocateMemory");
    if (VK_FAILED(vkResult)) {
        destroyImage(device, image, imageMemory, dispatch);
        return AXR_ERROR;
    }

    vkResult = device.bindImageMemory(
        image,
        imageMemory,
        0,
        dispatch
    );
    axrLogVkResult(vkResult, "device.bindImageMemory");
    if (VK_FAILED(vkResult)) {
        destroyImage(device, image, imageMemory, dispatch);
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrVulkanImage::destroyImage(
    const vk::Device device,
    vk::Image& image,
    vk::DeviceMemory& imageMemory,
    const vk::DispatchLoaderDynamic& dispatch
) {
    if (image != VK_NULL_HANDLE) {
        device.destroyImage(image, nullptr, dispatch);
        image = VK_NULL_HANDLE;
    }

    if (imageMemory != VK_NULL_HANDLE) {
        device.freeMemory(imageMemory, nullptr, dispatch);
        imageMemory = VK_NULL_HANDLE;
    }
}

AxrResult AxrVulkanImage::createImageView(
    const vk::Device device,
    const vk::Image& image,
    const vk::Format format,
    const vk::ImageAspectFlags aspectMask,
    const uint32_t mipLevelCount,
    vk::ImageView& imageView,
    const vk::DispatchLoaderDynamic& dispatch
) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (imageView != VK_NULL_HANDLE) {
        axrLogErrorLocation("Image view already exists.");
        return AXR_ERROR;
    }

    if (device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Device is null.");
        return AXR_ERROR;
    }

    if (image == VK_NULL_HANDLE) {
        axrLogErrorLocation("Image is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const vk::ImageViewCreateInfo imageViewCreateInfo(
        {},
        image,
        vk::ImageViewType::e2D,
        format,
        vk::ComponentMapping(),
        vk::ImageSubresourceRange{
            aspectMask,
            0,
            mipLevelCount,
            0,
            1
        }
    );

    const auto imageViewResult = device.createImageView(imageViewCreateInfo, nullptr, dispatch);
    axrLogVkResult(imageViewResult.result, "device.createImageView");
    if (VK_FAILED(imageViewResult.result)) {
        return AXR_ERROR;
    }

    imageView = imageViewResult.value;

    return AXR_SUCCESS;
}

void AxrVulkanImage::destroyImageView(
    const vk::Device device,
    vk::ImageView& imageView,
    const vk::DispatchLoaderDynamic& dispatch
) {
    if (imageView == VK_NULL_HANDLE) return;

    device.destroyImageView(imageView, nullptr, dispatch);
    imageView = VK_NULL_HANDLE;
}

// ---- Private Functions ----

void AxrVulkanImage::cleanup() {
    destroyImage();

    m_PhysicalDevice = VK_NULL_HANDLE;
    m_Device = VK_NULL_HANDLE;
    m_GraphicsCommandPool = VK_NULL_HANDLE;
    m_GraphicsQueue = VK_NULL_HANDLE;
    m_DispatchHandle = VK_NULL_HANDLE;
}

uint32_t AxrVulkanImage::countImageMipLevels(const uint32_t width, const uint32_t height) const {
    return static_cast<uint32_t>(
        std::floor(std::log2(std::max(width, height)))
    ) + 1;
}

AxrResult AxrVulkanImage::copyBufferToImage(
    const vk::Buffer buffer,
    const vk::Image image,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const vk::Format imageFormat,
    const uint32_t mipLevelCount
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Device == VK_NULL_HANDLE) {
        axrLogError("Device is null.");
        return AXR_ERROR;
    }

    if (m_GraphicsCommandPool == VK_NULL_HANDLE) {
        axrLogError("Graphics command pool is null.");
        return AXR_ERROR;
    }

    if (m_GraphicsQueue == VK_NULL_HANDLE) {
        axrLogError("Graphics queue is null.");
        return AXR_ERROR;
    }

    if (m_DispatchHandle == nullptr) {
        axrLogError("Dispatch handle is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    vk::CommandBuffer commandBuffer;
    axrResult = axrBeginSingleTimeCommand(
        m_Device,
        m_GraphicsCommandPool,
        commandBuffer,
        *m_DispatchHandle
    );
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    const vk::ImageMemoryBarrier imageMemoryBarrier(
        vk::AccessFlagBits::eNone,
        vk::AccessFlagBits::eTransferWrite,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        image,
        vk::ImageSubresourceRange(
            vk::ImageAspectFlagBits::eColor,
            0,
            mipLevelCount,
            0,
            1
        )
    );

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eTransfer,
        vk::DependencyFlags(),
        0,
        nullptr,
        0,
        nullptr,
        1,
        &imageMemoryBarrier,
        *m_DispatchHandle
    );

    const vk::BufferImageCopy bufferImageCopy(
        0,
        0,
        0,
        vk::ImageSubresourceLayers(
            vk::ImageAspectFlagBits::eColor,
            0,
            0,
            1
        ),
        vk::Offset3D(0, 0, 0),
        vk::Extent3D(imageWidth, imageHeight, 1)
    );

    commandBuffer.copyBufferToImage(
        buffer,
        image,
        vk::ImageLayout::eTransferDstOptimal,
        1,
        &bufferImageCopy,
        *m_DispatchHandle
    );

    generateMipmaps(
        commandBuffer,
        image,
        imageWidth,
        imageHeight,
        imageFormat,
        mipLevelCount,
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader
    );

    axrResult = axrEndSingleTimeCommand(
        m_Device,
        m_GraphicsCommandPool,
        m_GraphicsQueue,
        commandBuffer,
        *m_DispatchHandle
    );
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanImage::generateMipmaps(
    const vk::CommandBuffer commandBuffer,
    const vk::Image image,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const vk::Format imageFormat,
    const uint32_t mipLevelCount,
    const vk::AccessFlags initialImageAccessMask,
    const vk::AccessFlags finalImageAccessMask,
    const vk::ImageLayout initialImageLayout,
    const vk::ImageLayout finalImageLayout,
    const vk::PipelineStageFlagBits initialImageStageMask,
    const vk::PipelineStageFlagBits finalImageStageMask
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (commandBuffer == VK_NULL_HANDLE) {
        axrLogErrorLocation("Command buffer is null.");
        return;
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        axrLogErrorLocation("Physical device is null.");
        return;
    }

    if (m_DispatchHandle == nullptr) {
        axrLogErrorLocation("Dispatch handle is null.");
        return;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    vk::Filter blitFilter = vk::Filter::eLinear;

    // If linear filter isn't supported, use nearest
    if (!axrAreFormatFeaturesSupported(
        imageFormat,
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eBlitSrc | vk::FormatFeatureFlagBits::eSampledImageFilterLinear,
        m_PhysicalDevice,
        *m_DispatchHandle
    )) {
        blitFilter = vk::Filter::eNearest;
    }

    vk::ImageMemoryBarrier imageMemoryBarrier(
        // Gets set in the loop for each mip level
        vk::AccessFlagBits::eNone,
        // Gets set in the loop for each mip level
        vk::AccessFlagBits::eNone,
        // Gets set in the loop for each mip level
        vk::ImageLayout::eUndefined,
        // Gets set in the loop for each mip level
        vk::ImageLayout::eUndefined,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        image,
        vk::ImageSubresourceRange(
            vk::ImageAspectFlagBits::eColor,
            // Gets set in the loop for each mip level
            0,
            1,
            0,
            1
        )
    );

    uint32_t mipWidth = imageWidth;
    uint32_t mipHeight = imageHeight;

    for (uint32_t i = 1; i < mipLevelCount; ++i) {
        if (initialImageLayout != vk::ImageLayout::eTransferSrcOptimal) {
            imageMemoryBarrier.srcAccessMask = initialImageAccessMask;
            imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
            imageMemoryBarrier.oldLayout = initialImageLayout;
            imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
            imageMemoryBarrier.subresourceRange.baseMipLevel = i - 1;

            commandBuffer.pipelineBarrier(
                initialImageStageMask,
                vk::PipelineStageFlagBits::eTransfer,
                vk::DependencyFlags(),
                0,
                nullptr,
                0,
                nullptr,
                1,
                &imageMemoryBarrier,
                *m_DispatchHandle
            );
        }

        if (initialImageLayout != vk::ImageLayout::eTransferDstOptimal) {
            imageMemoryBarrier.srcAccessMask = initialImageAccessMask;
            imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
            imageMemoryBarrier.oldLayout = initialImageLayout;
            imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
            imageMemoryBarrier.subresourceRange.baseMipLevel = i;

            commandBuffer.pipelineBarrier(
                initialImageStageMask,
                vk::PipelineStageFlagBits::eTransfer,
                vk::DependencyFlags(),
                0,
                nullptr,
                0,
                nullptr,
                1,
                &imageMemoryBarrier,
                *m_DispatchHandle
            );
        }

        vk::ImageBlit imageBlit(
            vk::ImageSubresourceLayers(
                vk::ImageAspectFlagBits::eColor,
                i - 1,
                0,
                1
            ),
            std::array{
                vk::Offset3D(0, 0, 0),
                vk::Offset3D(
                    static_cast<int32_t>(mipWidth),
                    static_cast<int32_t>(mipHeight),
                    1
                ),
            },
            vk::ImageSubresourceLayers(
                vk::ImageAspectFlagBits::eColor,
                i,
                0,
                1
            ),
            std::array{
                vk::Offset3D(0, 0, 0),
                vk::Offset3D(
                    mipWidth > 1 ? static_cast<int32_t>(mipWidth) / 2 : 1,
                    mipHeight > 1 ? static_cast<int32_t>(mipHeight) / 2 : 1,
                    1
                ),
            }
        );

        commandBuffer.blitImage(
            image,
            vk::ImageLayout::eTransferSrcOptimal,
            image,
            vk::ImageLayout::eTransferDstOptimal,
            1,
            &imageBlit,
            blitFilter,
            *m_DispatchHandle
        );

        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        imageMemoryBarrier.dstAccessMask = finalImageAccessMask;
        imageMemoryBarrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        imageMemoryBarrier.newLayout = finalImageLayout;
        imageMemoryBarrier.subresourceRange.baseMipLevel = i - 1;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            finalImageStageMask,
            vk::DependencyFlags(),
            0,
            nullptr,
            0,
            nullptr,
            1,
            &imageMemoryBarrier,
            *m_DispatchHandle
        );

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    imageMemoryBarrier.dstAccessMask = finalImageAccessMask;
    imageMemoryBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    imageMemoryBarrier.newLayout = finalImageLayout;
    imageMemoryBarrier.subresourceRange.baseMipLevel = mipLevelCount - 1;

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        finalImageStageMask,
        vk::DependencyFlags(),
        0,
        nullptr,
        0,
        nullptr,
        1,
        &imageMemoryBarrier,
        *m_DispatchHandle
    );
}

#endif
