#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "vulkanUniformBufferData.hpp"
#include "axr/logger.h"
#include "../../../assets/engineAssets.hpp"

// ----------------------------------------- //
// Internal Functions
// ----------------------------------------- //

// ---- Special Functions ----

AxrVulkanUniformBufferData::AxrVulkanUniformBufferData() = default;

AxrVulkanUniformBufferData::AxrVulkanUniformBufferData(const Config& config):
    m_UniformBufferHandle(config.UniformBufferHandle),
    m_MaxFramesInFlight(config.MaxFramesInFlight),
    m_PhysicalDevice(config.PhysicalDevice),
    m_Device(config.Device),
    m_TransferCommandPool(config.TransferCommandPool),
    m_TransferQueue(config.TransferQueue),
    m_DispatchHandle(config.DispatchHandle) {
    m_UniformBufferHandle->addOnDataChangedCallback<&AxrVulkanUniformBufferData::onDataChangedCallback>(this);
}

AxrVulkanUniformBufferData::AxrVulkanUniformBufferData(AxrVulkanUniformBufferData&& src) noexcept {
    m_UniformBuffers = std::move(src.m_UniformBuffers);

    m_UniformBufferHandle = src.m_UniformBufferHandle;
    m_MaxFramesInFlight = src.m_MaxFramesInFlight;
    m_PhysicalDevice = src.m_PhysicalDevice;
    m_Device = src.m_Device;
    m_TransferCommandPool = src.m_TransferCommandPool;
    m_TransferQueue = src.m_TransferQueue;
    m_DispatchHandle = src.m_DispatchHandle;
    m_UniformBufferAlignment = src.m_UniformBufferAlignment;
    m_IsDataDirty = src.m_IsDataDirty;

    if (m_UniformBufferHandle != nullptr) {
        m_UniformBufferHandle->removeOnDataChangedCallback<&AxrVulkanUniformBufferData::onDataChangedCallback>(
            &src
        );
        m_UniformBufferHandle->addOnDataChangedCallback<&AxrVulkanUniformBufferData::onDataChangedCallback>(
            this
        );
    }

    src.m_UniformBufferHandle = nullptr;
    src.m_MaxFramesInFlight = 0;
    src.m_PhysicalDevice = VK_NULL_HANDLE;
    src.m_Device = VK_NULL_HANDLE;
    src.m_TransferCommandPool = VK_NULL_HANDLE;
    src.m_TransferQueue = VK_NULL_HANDLE;
    src.m_DispatchHandle = nullptr;
    src.m_UniformBufferAlignment = 0;
    src.m_IsDataDirty = false;
}

AxrVulkanUniformBufferData::~AxrVulkanUniformBufferData() {
    cleanup();
}

AxrVulkanUniformBufferData& AxrVulkanUniformBufferData::operator=(AxrVulkanUniformBufferData&& src) noexcept {
    if (this != &src) {
        cleanup();

        m_UniformBuffers = std::move(src.m_UniformBuffers);

        m_UniformBufferHandle = src.m_UniformBufferHandle;
        m_MaxFramesInFlight = src.m_MaxFramesInFlight;
        m_PhysicalDevice = src.m_PhysicalDevice;
        m_Device = src.m_Device;
        m_TransferCommandPool = src.m_TransferCommandPool;
        m_TransferQueue = src.m_TransferQueue;
        m_DispatchHandle = src.m_DispatchHandle;
        m_UniformBufferAlignment = src.m_UniformBufferAlignment;
        m_IsDataDirty = src.m_IsDataDirty;

        if (m_UniformBufferHandle != nullptr) {
            m_UniformBufferHandle->removeOnDataChangedCallback<&AxrVulkanUniformBufferData::onDataChangedCallback>(
                &src
            );
            m_UniformBufferHandle->addOnDataChangedCallback<&AxrVulkanUniformBufferData::onDataChangedCallback>(
                this
            );
        }

        src.m_UniformBufferHandle = nullptr;
        src.m_MaxFramesInFlight = 0;
        src.m_PhysicalDevice = VK_NULL_HANDLE;
        src.m_Device = VK_NULL_HANDLE;
        src.m_TransferCommandPool = VK_NULL_HANDLE;
        src.m_TransferQueue = VK_NULL_HANDLE;
        src.m_DispatchHandle = nullptr;
        src.m_UniformBufferAlignment = 0;
        src.m_IsDataDirty = false;
    }

    return *this;
}

// ---- Public Functions ----

const std::string& AxrVulkanUniformBufferData::getName() const {
    if (m_UniformBufferHandle == nullptr) {
        return m_DummyName;
    }

    return m_UniformBufferHandle->getName();
}

const AxrVulkanBuffer& AxrVulkanUniformBufferData::getBuffer(const uint32_t frameIndex) const {
    return m_UniformBuffers[frameIndex];
}

vk::DeviceSize AxrVulkanUniformBufferData::getBufferSize() const {
    if (m_UniformBufferHandle == nullptr) {
        return 0;
    }

    return m_UniformBufferHandle->getDataSize();
}

vk::DeviceSize AxrVulkanUniformBufferData::getInstanceSize() const {
    if (m_UniformBufferHandle == nullptr) {
        return 0;
    }

    return m_UniformBufferHandle->getInstanceSize();
}

AxrUniformBufferTypeEnum AxrVulkanUniformBufferData::getBufferType() const {
    if (m_UniformBufferHandle == nullptr) {
        return AXR_UNIFORM_BUFFER_TYPE_UNDEFINED;
    }

    return m_UniformBufferHandle->getBufferType();
}

bool AxrVulkanUniformBufferData::doesDataExist() const {
    return !m_UniformBuffers.empty();
}

AxrResult AxrVulkanUniformBufferData::createData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_UniformBuffers.empty()) {
        axrLogErrorLocation("Uniform buffers already exist.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    axrResult = setAlignment();
    if (AXR_FAILED(axrResult)) {
        destroyData();
        return axrResult;
    }

    m_UniformBuffers.resize(m_MaxFramesInFlight);

    for (uint32_t i = 0; i < m_MaxFramesInFlight; ++i) {
        auto buffer = AxrVulkanBuffer(
            AxrVulkanBuffer::Config{
                .PhysicalDevice = m_PhysicalDevice,
                .Device = m_Device,
                .TransferCommandPool = m_TransferCommandPool,
                .TransferQueue = m_TransferQueue,
                .DispatchHandle = m_DispatchHandle
            }
        );
        m_UniformBuffers[i] = std::move(buffer);

        axrResult = createUniformBuffer(m_UniformBuffers[i]);
        if (AXR_FAILED(axrResult)) {
            break;
        }
    }

    if (AXR_FAILED(axrResult)) {
        destroyData();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanUniformBufferData::destroyData() {
    for (AxrVulkanBuffer& uniformBuffer : m_UniformBuffers) {
        destroyUniformBuffer(uniformBuffer);
    }
    m_UniformBuffers.clear();
    resetAlignment();
}

AxrResult AxrVulkanUniformBufferData::setData(
    const uint32_t frameIndex,
    const bool alignData,
    const vk::DeviceSize offset,
    const vk::DeviceSize size,
    const void* data
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (data == nullptr) {
        axrLogErrorLocation("Data is null.");
        return AXR_ERROR;
    }

    if (frameIndex > m_UniformBuffers.size() - 1) {
        axrLogErrorLocation("Index out of bounds.");
        return AXR_ERROR;
    }

    if (m_UniformBuffers[frameIndex].isEmpty()) {
        axrLogErrorLocation("Buffer is empty.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    return setData(m_UniformBuffers[frameIndex], alignData, offset, size, data);
}

void AxrVulkanUniformBufferData::updateDirtyData(const uint32_t frameIndex) const {
    if (!m_IsDataDirty) return;

    if (m_UniformBufferHandle == nullptr) {
        axrLogErrorLocation("Uniform buffer handle is null.");
        return;
    }

    const bool alignData = m_UniformBufferHandle->getBufferType() == AXR_UNIFORM_BUFFER_TYPE_DYNAMIC;
    const AxrResult axrResult = setData(
        frameIndex,
        alignData,
        0,
        m_UniformBufferHandle->getDataSize(),
        m_UniformBufferHandle->getData()
    );

    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to update uniform buffer data.");
    }
}

// ---- Public Static Functions ----

vk::DeviceSize AxrVulkanUniformBufferData::calculateUniformBufferAlignment(
    const vk::PhysicalDevice physicalDevice,
    const vk::DeviceSize instanceSize,
    const vk::DispatchLoaderDynamic& dispatch
) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (physicalDevice == VK_NULL_HANDLE) {
        axrLogErrorLocation("Physical device is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const vk::PhysicalDeviceProperties properties = physicalDevice.getProperties(dispatch);
    const uint64_t minUniformBufferOffsetAlignment = properties.limits.minUniformBufferOffsetAlignment;

    vk::DeviceSize uniformBufferAlignment = instanceSize;

    if (minUniformBufferOffsetAlignment > 0) {
        uniformBufferAlignment =
            (uniformBufferAlignment + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1);
    }

    return uniformBufferAlignment;
}

// ---- Private Functions ----

void AxrVulkanUniformBufferData::cleanup() {
    destroyData();

    if (m_UniformBufferHandle != nullptr) {
        m_UniformBufferHandle->removeOnDataChangedCallback<&AxrVulkanUniformBufferData::onDataChangedCallback>(
            this
        );
    }

    m_UniformBufferHandle = nullptr;
    m_MaxFramesInFlight = 0;
    m_PhysicalDevice = VK_NULL_HANDLE;
    m_Device = VK_NULL_HANDLE;
    m_TransferCommandPool = VK_NULL_HANDLE;
    m_TransferQueue = VK_NULL_HANDLE;
    m_DispatchHandle = nullptr;
    m_UniformBufferAlignment = 0;
    m_IsDataDirty = false;
}

AxrResult AxrVulkanUniformBufferData::setAlignment() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_DispatchHandle == nullptr) {
        axrLogErrorLocation("Dispatch handle is null.");
        return AXR_ERROR;
    }

    if (m_UniformBufferHandle == nullptr) {
        axrLogErrorLocation("Uniform buffer handle is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    m_UniformBufferAlignment = calculateUniformBufferAlignment(
        m_PhysicalDevice,
        m_UniformBufferHandle->getInstanceSize(),
        *m_DispatchHandle
    );

    return AXR_SUCCESS;
}

void AxrVulkanUniformBufferData::resetAlignment() {
    m_UniformBufferAlignment = 0;
}

AxrResult AxrVulkanUniformBufferData::createUniformBuffer(AxrVulkanBuffer& buffer) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!buffer.isEmpty()) {
        axrLogErrorLocation("Uniform buffer already exists.");
        return AXR_ERROR;
    }

    if (m_UniformBufferHandle == nullptr) {
        axrLogErrorLocation("Uniform buffer handle is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    const bool alignData = m_UniformBufferHandle->getBufferType() == AXR_UNIFORM_BUFFER_TYPE_DYNAMIC;
    const uint64_t srcBufferSize = m_UniformBufferHandle->getDataSize();
    uint64_t dstBufferSize;
    if (alignData) {
        dstBufferSize = m_UniformBufferHandle->getInstanceCount() * m_UniformBufferAlignment;
    } else {
        dstBufferSize = srcBufferSize;
    }

    axrResult = buffer.createBuffer(
        false,
        dstBufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer
    );
    if (AXR_FAILED(axrResult)) {
        destroyUniformBuffer(buffer);
        return axrResult;
    }

    const void* srcData = m_UniformBufferHandle->getData();

    if (srcData != nullptr) {
        axrResult = setData(buffer, alignData, 0, srcBufferSize, srcData);

        if (AXR_FAILED(axrResult)) {
            destroyUniformBuffer(buffer);
            return axrResult;
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanUniformBufferData::destroyUniformBuffer(AxrVulkanBuffer& buffer) const {
    buffer.destroyBuffer();
}

AxrResult AxrVulkanUniformBufferData::setData(
    const AxrVulkanBuffer& buffer,
    const bool alignData,
    const vk::DeviceSize offset,
    const vk::DeviceSize size,
    const void* data
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (data == nullptr) {
        axrLogErrorLocation("Data is null.");
        return AXR_ERROR;
    }

    if (offset + size > buffer.getSize()) {
        axrLogErrorLocation("Invalid offset and size. Out of bounds");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    const uint64_t instanceSize = m_UniformBufferHandle->getInstanceSize();

    // If the instances are already aligned, then we don't need to do anything special
    if (!alignData || instanceSize == m_UniformBufferAlignment) {
        return buffer.setBufferData(offset, size, data);
    }

    // Check that when we get instanceCount below, it's a whole number
    if (size % instanceSize != 0) {
        axrLogErrorLocation("Data size is not a multiple of the instance size.");
        return AXR_ERROR;
    }

    const uint32_t instanceCount = size / instanceSize;
    const uint64_t alignedSize = instanceCount * m_UniformBufferAlignment;

    // Check if it's out of bounds again. But this time, accounting for the alignment too
    if (offset + alignedSize > buffer.getSize()) {
        axrLogErrorLocation("Invalid offset and size. Out of bounds");
        return AXR_ERROR;
    }

    void* alignedData = calloc(1, alignedSize);
    for (int i = 0; i < instanceCount; ++i) {
        memcpy_s(
            static_cast<int8_t*>(alignedData) + m_UniformBufferAlignment * i,
            alignedSize - m_UniformBufferAlignment * i,
            static_cast<const int8_t*>(data) + instanceSize * i,
            instanceSize
        );
    }
    const AxrResult axrResult = buffer.setBufferData(offset, alignedSize, alignedData);
    free(alignedData);

    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to set uniform buffer data.");
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrVulkanUniformBufferData::onDataChangedCallback() {
    m_IsDataDirty = true;
}

#endif
