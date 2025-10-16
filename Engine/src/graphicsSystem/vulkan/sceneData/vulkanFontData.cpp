// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "vulkanFontData.hpp"
#include "axr/logger.h"
#include "../../../assets/engineAssets.hpp"

// ----------------------------------------- //
// Internal Functions
// ----------------------------------------- //

// ---- Special Functions ----

AxrVulkanFontData::AxrVulkanFontData() = default;

AxrVulkanFontData::AxrVulkanFontData(const Config& config):
    m_FontHandle(config.FontHandle),
    m_Device(config.Device),
    m_DispatchHandle(config.DispatchHandle) {
    if (m_FontHandle == nullptr) {
        axrLogErrorLocation("Font handle is null.");
    } else {
        m_MaterialData = AxrVulkanMaterialData(
            AxrVulkanMaterialData::Config{
                .MaterialHandle = &m_FontHandle->getMaterial(),
                .MaterialNameSuffix = "",
                .MaterialLayoutData = config.MaterialLayoutData,
                .MaxFramesInFlight = config.MaxFramesInFlight,
                .Device = m_Device,
                .DispatchHandle = m_DispatchHandle,
            }
        );
        setMaterialDataCallbacks();

        m_GlyphUniformBufferData = AxrVulkanUniformBufferData(
            AxrVulkanUniformBufferData::Config{
                .UniformBufferHandle = &m_FontHandle->getGlyphUniformBuffer(),
                .MaxFramesInFlight = config.MaxFramesInFlight,
                .PhysicalDevice = config.PhysicalDevice,
                .Device = m_Device,
                .TransferCommandPool = config.TransferCommandPool,
                .TransferQueue = config.TransferQueue,
                .DispatchHandle = m_DispatchHandle,
            }
        );
    }
}

AxrVulkanFontData::AxrVulkanFontData(AxrVulkanFontData&& src) noexcept {
    FindUniformBufferCallback = std::move(src.FindUniformBufferCallback);
    FindImageSamplerCallback = std::move(src.FindImageSamplerCallback);
    FindImageCallback = std::move(src.FindImageCallback);
    m_MaterialData = std::move(src.m_MaterialData);
    m_GlyphUniformBufferData = std::move(src.m_GlyphUniformBufferData);

    // We need to set these again because 'this' changed
    setMaterialDataCallbacks();

    m_FontHandle = src.m_FontHandle;
    m_Device = src.m_Device;
    m_DispatchHandle = src.m_DispatchHandle;

    src.m_FontHandle = nullptr;
    src.m_Device = VK_NULL_HANDLE;
    src.m_DispatchHandle = nullptr;
}

AxrVulkanFontData::~AxrVulkanFontData() {
    cleanup();
}

AxrVulkanFontData& AxrVulkanFontData::operator=(AxrVulkanFontData&& src) noexcept {
    if (this != &src) {
        cleanup();

        FindUniformBufferCallback = std::move(src.FindUniformBufferCallback);
        FindImageSamplerCallback = std::move(src.FindImageSamplerCallback);
        FindImageCallback = std::move(src.FindImageCallback);
        m_MaterialData = std::move(src.m_MaterialData);
        m_GlyphUniformBufferData = std::move(src.m_GlyphUniformBufferData);

        // We need to set these again because 'this' changed
        setMaterialDataCallbacks();

        m_FontHandle = src.m_FontHandle;
        m_Device = src.m_Device;
        m_DispatchHandle = src.m_DispatchHandle;

        src.m_FontHandle = nullptr;
        src.m_Device = VK_NULL_HANDLE;
        src.m_DispatchHandle = nullptr;
    }
    return *this;
}

// ---- Public Functions ----

const std::string& AxrVulkanFontData::getName() const {
    if (m_FontHandle == nullptr) {
        return m_DummyName;
    }

    return m_FontHandle->getName();
}

uint16_t AxrVulkanFontData::getID() const {
    if (m_FontHandle == nullptr) {
        return 0;
    }

    return m_FontHandle->getID();
}

const AxrVulkanMaterialData& AxrVulkanFontData::getMaterialData() const {
    return m_MaterialData;
}

const AxrVulkanUniformBufferData& AxrVulkanFontData::getGlyphUniformBufferData() const {
    return m_GlyphUniformBufferData;
}

bool AxrVulkanFontData::doesDataExist() const {
    return m_MaterialData.doesDataExist() &&
        m_GlyphUniformBufferData.doesDataExist();
}

bool AxrVulkanFontData::doesWindowDataExist() const {
    return m_MaterialData.doesWindowDataExist();
}

bool AxrVulkanFontData::doesXrSessionDataExist() const {
    return m_MaterialData.doesXrSessionDataExist();
}

AxrResult AxrVulkanFontData::createData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_FontHandle == nullptr) {
        axrLogErrorLocation("Font handle is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    if (!m_FontHandle->isLoaded()) {
        axrResult = m_FontHandle->loadFile();

        if (AXR_FAILED(axrResult)) {
            destroyData();
            return axrResult;
        }
    }

    axrResult = m_GlyphUniformBufferData.createData();
    if (AXR_FAILED(axrResult)) {
        destroyData();
        return axrResult;
    }

    axrResult = m_MaterialData.createData();
    if (AXR_FAILED(axrResult)) {
        destroyData();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanFontData::destroyData() {
    m_MaterialData.destroyData();
    m_GlyphUniformBufferData.destroyData();
}

AxrResult AxrVulkanFontData::createWindowData(
    const vk::RenderPass renderPass,
    const vk::SampleCountFlagBits msaaSampleCount
) {
    return m_MaterialData.createWindowData(renderPass, msaaSampleCount);
}

void AxrVulkanFontData::destroyWindowData() {
    m_MaterialData.destroyWindowData();
}

AxrResult AxrVulkanFontData::createXrSessionData(
    const vk::RenderPass renderPass,
    const vk::SampleCountFlagBits msaaSampleCount,
    const uint32_t viewCount
) {
    return m_MaterialData.createXrSessionData(renderPass, msaaSampleCount, viewCount);
}

void AxrVulkanFontData::destroyXrSessionData() {
    m_MaterialData.destroyXrSessionData();
}

// ---- Private Functions ----

void AxrVulkanFontData::cleanup() {
    destroyData();

    m_FontHandle = nullptr;
    m_Device = VK_NULL_HANDLE;
    m_DispatchHandle = nullptr;
    FindUniformBufferCallback.reset();
    FindImageSamplerCallback.reset();
    FindImageCallback.reset();
}

void AxrVulkanFontData::setMaterialDataCallbacks() {
    m_MaterialData.FindUniformBufferCallback.connect<&AxrVulkanFontData::findUniformBufferData>(this);
    m_MaterialData.FindImageSamplerCallback.connect<&AxrVulkanFontData::findImageSamplerData>(this);
    m_MaterialData.FindImageCallback.connect<&AxrVulkanFontData::findImageData>(this);
}

const AxrVulkanUniformBufferData* AxrVulkanFontData::findUniformBufferData(
    const std::string& name,
    const AxrPlatformType platformType,
    const uint32_t viewIndex
) const {
    if (name == axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_GLYPHS)) {
        return &m_GlyphUniformBufferData;
    }

    return FindUniformBufferCallback(name, platformType, viewIndex);
}

const AxrVulkanImageSamplerData* AxrVulkanFontData::findImageSamplerData(const std::string& name) const {
    return FindImageSamplerCallback(name);
}

const AxrVulkanImageData* AxrVulkanFontData::findImageData(const std::string& name) const {
    return FindImageCallback(name);
}
