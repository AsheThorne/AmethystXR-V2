// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "vulkanFontData.hpp"

#include "axr/logger.h"

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
    }
}

AxrVulkanFontData::AxrVulkanFontData(AxrVulkanFontData&& src) noexcept {
    m_MaterialData = std::move(src.m_MaterialData);

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

        m_MaterialData = std::move(src.m_MaterialData);

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

AxrVulkanMaterialData& AxrVulkanFontData::getMaterialData() {
    return m_MaterialData;
}

bool AxrVulkanFontData::doesDataExist() const {
    return m_MaterialData.doesDataExist();
}

bool AxrVulkanFontData::doesWindowDataExist() const {
    return m_MaterialData.doesWindowDataExist();
}

bool AxrVulkanFontData::doesXrSessionDataExist() const {
    return m_MaterialData.doesXrSessionDataExist();
}

AxrResult AxrVulkanFontData::createData() {
    return m_MaterialData.createData();
}

void AxrVulkanFontData::destroyData() {
    m_MaterialData.destroyData();
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
    m_MaterialData.destroyData();

    m_FontHandle = nullptr;
    m_Device = VK_NULL_HANDLE;
    m_DispatchHandle = nullptr;
}
