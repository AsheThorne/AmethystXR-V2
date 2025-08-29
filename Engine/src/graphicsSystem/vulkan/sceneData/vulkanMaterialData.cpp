#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "vulkanMaterialData.hpp"
#include "axr/logger.h"
#include "../vulkanUtils.hpp"
#include "../../../assets/material.hpp"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <unordered_set>

// ----------------------------------------- //
// Internal Functions
// ----------------------------------------- //

// ---- Special Functions ----

AxrVulkanMaterialData::AxrVulkanMaterialData():
    m_MaterialHandle(nullptr),
    m_MaterialLayoutData(nullptr),
    m_MaxFramesInFlight(0),
    m_Device(VK_NULL_HANDLE),
    m_DispatchHandle(nullptr),
    m_WindowDescriptorPool(VK_NULL_HANDLE),
    m_WindowPipeline(VK_NULL_HANDLE),
    m_XrSessionDescriptorPool(VK_NULL_HANDLE),
    m_XrSessionPipeline(VK_NULL_HANDLE) {
}

AxrVulkanMaterialData::AxrVulkanMaterialData(const Config& config):
    m_MaterialHandle(config.MaterialHandle),
    m_MaterialLayoutData(config.MaterialLayoutData),
    m_MaxFramesInFlight(config.MaxFramesInFlight),
    m_Device(config.Device),
    m_DispatchHandle(config.DispatchHandle),
    m_WindowDescriptorPool(VK_NULL_HANDLE),
    m_WindowPipeline(VK_NULL_HANDLE),
    m_XrSessionDescriptorPool(VK_NULL_HANDLE),
    m_XrSessionPipeline(VK_NULL_HANDLE) {
}

AxrVulkanMaterialData::AxrVulkanMaterialData(AxrVulkanMaterialData&& src) noexcept {
    m_WindowDescriptorSets = std::move(src.m_WindowDescriptorSets);
    m_XrSessionDescriptorSets = std::move(src.m_XrSessionDescriptorSets);

    m_MaterialHandle = src.m_MaterialHandle;
    m_MaterialLayoutData = src.m_MaterialLayoutData;
    m_MaxFramesInFlight = src.m_MaxFramesInFlight;
    m_Device = src.m_Device;
    m_DispatchHandle = src.m_DispatchHandle;
    m_WindowPipeline = src.m_WindowPipeline;
    m_WindowDescriptorPool = src.m_WindowDescriptorPool;
    m_XrSessionPipeline = src.m_XrSessionPipeline;
    m_XrSessionDescriptorPool = src.m_XrSessionDescriptorPool;

    src.m_MaterialHandle = nullptr;
    src.m_MaterialLayoutData = nullptr;
    src.m_MaxFramesInFlight = 0;
    src.m_Device = VK_NULL_HANDLE;
    src.m_DispatchHandle = nullptr;
    src.m_WindowPipeline = VK_NULL_HANDLE;
    src.m_WindowDescriptorPool = VK_NULL_HANDLE;
    src.m_XrSessionPipeline = VK_NULL_HANDLE;
    src.m_XrSessionDescriptorPool = VK_NULL_HANDLE;
}

AxrVulkanMaterialData::~AxrVulkanMaterialData() {
    cleanup();
}

AxrVulkanMaterialData& AxrVulkanMaterialData::operator=(AxrVulkanMaterialData&& src) noexcept {
    if (this != &src) {
        cleanup();
        m_WindowDescriptorSets = std::move(src.m_WindowDescriptorSets);
        m_XrSessionDescriptorSets = std::move(src.m_XrSessionDescriptorSets);

        m_MaterialHandle = src.m_MaterialHandle;
        m_MaterialLayoutData = src.m_MaterialLayoutData;
        m_MaxFramesInFlight = src.m_MaxFramesInFlight;
        m_Device = src.m_Device;
        m_DispatchHandle = src.m_DispatchHandle;
        m_WindowPipeline = src.m_WindowPipeline;
        m_WindowDescriptorPool = src.m_WindowDescriptorPool;
        m_XrSessionPipeline = src.m_XrSessionPipeline;
        m_XrSessionDescriptorPool = src.m_XrSessionDescriptorPool;

        src.m_MaterialHandle = nullptr;
        src.m_MaterialLayoutData = nullptr;
        src.m_MaxFramesInFlight = 0;
        src.m_Device = VK_NULL_HANDLE;
        src.m_DispatchHandle = nullptr;
        src.m_WindowPipeline = VK_NULL_HANDLE;
        src.m_WindowDescriptorPool = VK_NULL_HANDLE;
        src.m_XrSessionPipeline = VK_NULL_HANDLE;
        src.m_XrSessionDescriptorPool = VK_NULL_HANDLE;
    }

    return *this;
}

// ---- Public Functions ----

const std::string& AxrVulkanMaterialData::getName() const {
    if (m_MaterialHandle == nullptr) {
        return m_DummyName;
    }

    return m_MaterialHandle->getName();
}

const AxrVulkanMaterialLayoutData* AxrVulkanMaterialData::getMaterialLayoutData() const {
    return m_MaterialLayoutData;
}

const vk::Pipeline& AxrVulkanMaterialData::getWindowPipeline() const {
    return m_WindowPipeline;
}

const vk::Pipeline& AxrVulkanMaterialData::getXrSessionPipeline() const {
    return m_XrSessionPipeline;
}

const std::vector<vk::DescriptorSet>& AxrVulkanMaterialData::getDescriptorSets(
    const AxrPlatformType platformType
) const {
    switch (platformType) {
        case AXR_PLATFORM_TYPE_WINDOW: {
            return m_WindowDescriptorSets;
        }
        case AXR_PLATFORM_TYPE_XR_DEVICE: {
            return m_XrSessionDescriptorSets;
        }
        case AXR_PLATFORM_TYPE_UNDEFINED:
        default: {
            axrLogErrorLocation("Unknown platform type.");
            return m_DummyDescriptorSets;
        }
    }
}

void AxrVulkanMaterialData::resetDescriptorSets(const AxrPlatformType platformType) {
    switch (platformType) {
        case AXR_PLATFORM_TYPE_WINDOW: {
            resetDescriptorSets(m_WindowDescriptorPool, m_WindowDescriptorSets);
            break;
        }
        case AXR_PLATFORM_TYPE_XR_DEVICE: {
            resetDescriptorSets(m_XrSessionDescriptorPool, m_XrSessionDescriptorSets);
            break;
        }
        case AXR_PLATFORM_TYPE_UNDEFINED:
        default: {
            axrLogErrorLocation("Unknown platform type.");
            break;
        }
    }
}

const AxrMaterial* AxrVulkanMaterialData::getMaterial() const {
    return m_MaterialHandle;
}

const std::vector<AxrDynamicUniformBufferOffsetConfig>& AxrVulkanMaterialData::getDynamicUniformBufferOffsets() const {
    return m_MaterialHandle->getDynamicUniformBufferOffsets();
}

bool AxrVulkanMaterialData::doesDataExist() const {
    // When we have general data, check for it here
    return true;
}

bool AxrVulkanMaterialData::doesWindowDataExist() const {
    return m_WindowDescriptorPool != VK_NULL_HANDLE &&
        !m_WindowDescriptorSets.empty() &&
        m_WindowPipeline != VK_NULL_HANDLE;
}

bool AxrVulkanMaterialData::doesXrSessionDataExist() const {
    return m_XrSessionDescriptorPool != VK_NULL_HANDLE &&
        !m_XrSessionDescriptorSets.empty() &&
        m_XrSessionPipeline != VK_NULL_HANDLE;
}

AxrResult AxrVulkanMaterialData::createData() {
    // We don't have general data yet
    return AXR_SUCCESS;
}

void AxrVulkanMaterialData::destroyData() {
    destroyWindowData();
    destroyXrSessionData();

    // Destroy general data here when we have some
}

AxrResult AxrVulkanMaterialData::createWindowData(
    const vk::RenderPass renderPass,
    const vk::SampleCountFlagBits msaaSampleCount
) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (doesWindowDataExist()) {
        axrLogErrorLocation("Material window data already exists.");
        return AXR_ERROR;
    }

    if (!doesDataExist()) {
        axrLogErrorLocation("Material data is missing.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    axrResult = createDescriptorPool(1, m_WindowDescriptorPool);
    if (AXR_FAILED(axrResult)) {
        destroyWindowData();
        return axrResult;
    }

    axrResult = createDescriptorSets(1, m_WindowDescriptorPool, m_WindowDescriptorSets);
    if (AXR_FAILED(axrResult)) {
        destroyWindowData();
        return axrResult;
    }

    axrResult = createPipeline(renderPass, msaaSampleCount, m_WindowPipeline);
    if (AXR_FAILED(axrResult)) {
        destroyWindowData();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanMaterialData::destroyWindowData() {
    destroyPipeline(m_WindowPipeline);
    resetDescriptorSets(AXR_PLATFORM_TYPE_WINDOW);
    destroyDescriptorPool(m_WindowDescriptorPool);
}

AxrResult AxrVulkanMaterialData::createXrSessionData(
    const vk::RenderPass renderPass,
    const vk::SampleCountFlagBits msaaSampleCount,
    const uint32_t viewCount
) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (doesXrSessionDataExist()) {
        axrLogErrorLocation("Material xr session data already exists.");
        return AXR_ERROR;
    }

    if (!doesDataExist()) {
        axrLogErrorLocation("Material data is missing.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    axrResult = createDescriptorPool(viewCount, m_XrSessionDescriptorPool);
    if (AXR_FAILED(axrResult)) {
        destroyXrSessionData();
        return axrResult;
    }

    axrResult = createDescriptorSets(viewCount, m_XrSessionDescriptorPool, m_XrSessionDescriptorSets);
    if (AXR_FAILED(axrResult)) {
        destroyXrSessionData();
        return axrResult;
    }

    axrResult = createPipeline(renderPass, msaaSampleCount, m_XrSessionPipeline);
    if (AXR_FAILED(axrResult)) {
        destroyXrSessionData();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanMaterialData::destroyXrSessionData() {
    destroyPipeline(m_XrSessionPipeline);
    resetDescriptorSets(AXR_PLATFORM_TYPE_XR_DEVICE);
    destroyDescriptorPool(m_XrSessionDescriptorPool);
}

// ---- Private Functions ----

void AxrVulkanMaterialData::cleanup() {
    destroyWindowData();
    destroyXrSessionData();
    destroyData();

    m_MaterialHandle = nullptr;
    m_MaterialLayoutData = nullptr;
    m_MaxFramesInFlight = 0;
    m_Device = VK_NULL_HANDLE;
    m_DispatchHandle = nullptr;
}

AxrResult AxrVulkanMaterialData::createDescriptorPool(
    const uint32_t viewCount,
    vk::DescriptorPool& descriptorPool
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (descriptorPool != VK_NULL_HANDLE) {
        axrLogErrorLocation("Descriptor pool already exists.");
        return AXR_ERROR;
    }

    if (m_Device == VK_NULL_HANDLE) {
        axrLogError("Device is null.");
        return AXR_ERROR;
    }

    if (m_DispatchHandle == nullptr) {
        axrLogError("Dispatch handle is null.");
        return AXR_ERROR;
    }

    if (m_MaterialLayoutData == nullptr) {
        axrLogError("Material layout data is null.");
        return AXR_ERROR;
    }

    const AxrShader* vertexShaderHandle = m_MaterialLayoutData->getVertexShaderHandle();
    if (vertexShaderHandle == nullptr) {
        axrLogErrorLocation("Vertex shader handle is null.");
        return AXR_ERROR;
    }

    const AxrShader* fragmentShaderHandle = m_MaterialLayoutData->getFragmentShaderHandle();
    if (fragmentShaderHandle == nullptr) {
        axrLogErrorLocation("Fragment shader handle is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const uint32_t uniformBufferLayoutCount = static_cast<uint32_t>(
        vertexShaderHandle->getProperties().getUniformBufferLayouts().size() +
        fragmentShaderHandle->getProperties().getUniformBufferLayouts().size()
    );
    const uint32_t dynamicUniformBufferLayoutCount = static_cast<uint32_t>(
        vertexShaderHandle->getProperties().getDynamicUniformBufferLayouts().size() +
        fragmentShaderHandle->getProperties().getDynamicUniformBufferLayouts().size()
    );
    const uint32_t imageSamplerBufferLayoutCount = static_cast<uint32_t>(
        vertexShaderHandle->getProperties().getImageSamplerBufferLayouts().size() +
        fragmentShaderHandle->getProperties().getImageSamplerBufferLayouts().size()
    );

    std::vector<vk::DescriptorPoolSize> poolSizes;

    if (uniformBufferLayoutCount != 0) {
        const vk::DescriptorPoolSize poolSize(
            vk::DescriptorType::eUniformBuffer,
            uniformBufferLayoutCount * m_MaxFramesInFlight * viewCount
        );
        poolSizes.push_back(poolSize);
    }

    if (dynamicUniformBufferLayoutCount != 0) {
        const vk::DescriptorPoolSize poolSize(
            vk::DescriptorType::eUniformBufferDynamic,
            dynamicUniformBufferLayoutCount * m_MaxFramesInFlight * viewCount
        );
        poolSizes.push_back(poolSize);
    }

    if (imageSamplerBufferLayoutCount != 0) {
        const vk::DescriptorPoolSize poolSize(
            vk::DescriptorType::eSampledImage,
            imageSamplerBufferLayoutCount * m_MaxFramesInFlight * viewCount
        );
        poolSizes.push_back(poolSize);
    }

    const vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo(
        {},
        m_MaxFramesInFlight * viewCount,
        static_cast<uint32_t>(poolSizes.size()),
        poolSizes.data()
    );

    const vk::Result vkResult = m_Device.createDescriptorPool(
        &descriptorPoolCreateInfo,
        nullptr,
        &descriptorPool,
        *m_DispatchHandle
    );
    axrLogVkResult(vkResult, "m_Device.createDescriptorPool");
    if (VK_FAILED(vkResult)) {
        destroyDescriptorPool(descriptorPool);
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrVulkanMaterialData::destroyDescriptorPool(vk::DescriptorPool& descriptorPool) const {
    if (descriptorPool == VK_NULL_HANDLE) return;

    m_Device.destroyDescriptorPool(descriptorPool, nullptr, *m_DispatchHandle);
    descriptorPool = VK_NULL_HANDLE;
}

AxrResult AxrVulkanMaterialData::createDescriptorSets(
    const uint32_t viewCount,
    const vk::DescriptorPool descriptorPool,
    std::vector<vk::DescriptorSet>& descriptorSets
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!descriptorSets.empty()) {
        axrLogErrorLocation("Descriptor sets already exist.");
        return AXR_ERROR;
    }

    if (m_MaterialLayoutData == nullptr) {
        axrLogErrorLocation("Material layout data is null.");
        return AXR_ERROR;
    }

    const vk::DescriptorSetLayout& descriptorSetLayout = m_MaterialLayoutData->getDescriptorSetLayout();
    if (descriptorSetLayout == VK_NULL_HANDLE) {
        axrLogErrorLocation("Descriptor set layout is null.");
        return AXR_ERROR;
    }

    if (descriptorPool == VK_NULL_HANDLE) {
        axrLogErrorLocation("Descriptor pool is null.");
        return AXR_ERROR;
    }

    if (viewCount < 1) {
        axrLogErrorLocation("View count must be greater than 0.");
        return AXR_ERROR;
    }

    if (m_Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Device is null.");
        return AXR_ERROR;
    }

    if (m_DispatchHandle == nullptr) {
        axrLogErrorLocation("Dispatch handle is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const std::vector layouts(m_MaxFramesInFlight * viewCount, descriptorSetLayout);

    const vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo(
        descriptorPool,
        m_MaxFramesInFlight * viewCount,
        layouts.data()
    );

    descriptorSets.resize(m_MaxFramesInFlight * viewCount);

    const vk::Result vkResult = m_Device.allocateDescriptorSets(
        &descriptorSetAllocateInfo,
        descriptorSets.data(),
        *m_DispatchHandle
    );
    axrLogVkResult(vkResult, "m_Device.allocateDescriptorSets");
    if (VK_FAILED(vkResult)) {
        resetDescriptorSets(descriptorPool, descriptorSets);
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrVulkanMaterialData::resetDescriptorSets(
    const vk::DescriptorPool descriptorPool,
    std::vector<vk::DescriptorSet>& descriptorSets
) const {
    if (descriptorPool == VK_NULL_HANDLE) return;

    m_Device.resetDescriptorPool(descriptorPool, {}, *m_DispatchHandle);
    descriptorSets.clear();
}

AxrResult AxrVulkanMaterialData::createPipeline(
    const vk::RenderPass renderPass,
    const vk::SampleCountFlagBits msaaSampleCount,
    vk::Pipeline& pipeline
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (pipeline != VK_NULL_HANDLE) {
        axrLogErrorLocation("Pipeline already exists.");
        return AXR_ERROR;
    }

    if (m_MaterialLayoutData == nullptr) {
        axrLogErrorLocation("Material layout data is null.");
        return AXR_ERROR;
    }

    if (m_MaterialLayoutData->getPipelineLayout() == VK_NULL_HANDLE) {
        axrLogErrorLocation("Pipeline layout is null.");
        return AXR_ERROR;
    }

    if (m_Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Device is null.");
        return AXR_ERROR;
    }

    if (m_DispatchHandle == nullptr) {
        axrLogErrorLocation("Dispatch handle is null.");
        return AXR_ERROR;
    }

    const AxrShader* vertexShaderHandle = m_MaterialLayoutData->getVertexShaderHandle();
    if (vertexShaderHandle == nullptr) {
        axrLogErrorLocation("Vertex shader handle is null.");
        return AXR_ERROR;
    }

    const AxrShader* fragmentShaderHandle = m_MaterialLayoutData->getFragmentShaderHandle();
    if (fragmentShaderHandle == nullptr) {
        axrLogErrorLocation("Fragment shader handle is null.");
        return AXR_ERROR;
    }

    if (m_MaterialHandle == nullptr) {
        axrLogErrorLocation("Material handle is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    // ---- Shader Stage ----

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfos(2);
    std::vector<vk::ShaderModule> shaderModules(2);

    if (!vertexShaderHandle->isLoaded()) {
        axrResult = vertexShaderHandle->loadFile();
        if (AXR_FAILED(axrResult)) {
            cleanupPipelineCreationData(shaderModules);
            return axrResult;
        }
    }

    axrResult = createShaderModule(vertexShaderHandle->getFileData(), shaderModules[0]);
    if (AXR_FAILED(axrResult)) {
        cleanupPipelineCreationData(shaderModules);
        return axrResult;
    }

    shaderStageCreateInfos[0] = vk::PipelineShaderStageCreateInfo(
        {},
        vk::ShaderStageFlagBits::eVertex,
        shaderModules[0],
        "main"
    );

    if (!fragmentShaderHandle->isLoaded()) {
        axrResult = fragmentShaderHandle->loadFile();
        if (AXR_FAILED(axrResult)) {
            cleanupPipelineCreationData(shaderModules);
            return axrResult;
        }
    }

    axrResult = createShaderModule(fragmentShaderHandle->getFileData(), shaderModules[1]);
    if (AXR_FAILED(axrResult)) {
        cleanupPipelineCreationData(shaderModules);
        return axrResult;
    }

    shaderStageCreateInfos[1] = vk::PipelineShaderStageCreateInfo(
        {},
        vk::ShaderStageFlagBits::eFragment,
        shaderModules[1],
        "main"
    );

    // ---- Vertex Input State ----

    std::vector<AxrShaderVertexAttribute> vertexAttributes = vertexShaderHandle->getProperties().
        getVertexAttributes();

    vk::VertexInputBindingDescription vertexBindingDescription(
        0,
        sizeof(AxrVertex),
        vk::VertexInputRate::eVertex
    );

    std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescriptions(vertexAttributes.size());
    for (size_t i = 0; i < vertexAttributeDescriptions.size(); ++i) {
        vertexAttributeDescriptions[i] = vk::VertexInputAttributeDescription(
            vertexAttributes[i].Location,
            vertexBindingDescription.binding,
            getVertexAttributeFormat(vertexAttributes[i].Type),
            getVertexAttributeOffset(vertexAttributes[i].Type)
        );
    }

    const vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo(
        {},
        1,
        &vertexBindingDescription,
        static_cast<uint32_t>(vertexAttributeDescriptions.size()),
        vertexAttributeDescriptions.data()
    );

    // ---- Input Assembly State ----

    constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo(
        {},
        vk::PrimitiveTopology::eTriangleList,
        vk::False
    );

    // ---- Viewport State ----

    constexpr vk::PipelineViewportStateCreateInfo viewportStateCreateInfo(
        {},
        1,
        {},
        1,
        {}
    );

    // ---- Rasterization State ----

    const vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo(
        {},
        vk::False,
        vk::False,
        vk::PolygonMode::eFill,
        axrToVkCullMode(m_MaterialHandle->getBackfaceCullMode()),
        vk::FrontFace::eCounterClockwise,
        vk::False,
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    // ---- Multisample State ----

    const vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo(
        {},
        msaaSampleCount,
        vk::False,
        1.0f,
        nullptr,
        vk::False,
        vk::False
    );

    // ---- Depth Stencil State ----

    const vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo(
        {},
        m_MaterialHandle->enableDepthTest() ? vk::True : vk::False,
        m_MaterialHandle->enableDepthWrite() ? vk::True : vk::False,
        vk::CompareOp::eLess,
        vk::False,
        vk::False,
        {},
        {},
        0.0f,
        1.0f
    );

    // ---- Color Blend State ----

    const vk::PipelineColorBlendAttachmentState colorBlendAttachment(
        m_MaterialHandle->getAlphaRenderMode() == AXR_MATERIAL_ALPHA_RENDER_MODE_OPAQUE ? vk::False : vk::True,
        vk::BlendFactor::eSrcAlpha,
        vk::BlendFactor::eOneMinusSrcAlpha,
        vk::BlendOp::eAdd,
        vk::BlendFactor::eOne,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA
    );

    constexpr std::array blendConstants{
        0.0f,
        0.0f,
        0.0f,
        0.0f
    };

    const vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo(
        {},
        vk::False,
        vk::LogicOp::eCopy,
        1,
        &colorBlendAttachment,
        blendConstants
    );

    // ---- Dynamic State ----

    constexpr std::array dynamicStates{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo(
        {},
        static_cast<uint32_t>(dynamicStates.size()),
        dynamicStates.data()
    );

    // ---- Pipeline Creation ----

    const vk::GraphicsPipelineCreateInfo pipelineCreateInfo(
        {},
        static_cast<uint32_t>(shaderStageCreateInfos.size()),
        shaderStageCreateInfos.data(),
        &vertexInputStateCreateInfo,
        &inputAssemblyStateCreateInfo,
        {},
        &viewportStateCreateInfo,
        &rasterizationStateCreateInfo,
        &multisampleStateCreateInfo,
        &depthStencilStateCreateInfo,
        &colorBlendStateCreateInfo,
        &dynamicStateCreateInfo,
        m_MaterialLayoutData->getPipelineLayout(),
        renderPass,
        0,
        VK_NULL_HANDLE,
        -1
    );

    const vk::Result vkResult = m_Device.createGraphicsPipelines(
        VK_NULL_HANDLE,
        1,
        &pipelineCreateInfo,
        nullptr,
        &pipeline,
        *m_DispatchHandle
    );
    axrLogVkResult(vkResult, "m_Device.createGraphicsPipelines");

    cleanupPipelineCreationData(shaderModules);

    if (VK_FAILED(vkResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrVulkanMaterialData::destroyPipeline(vk::Pipeline& pipeline) {
    if (pipeline == VK_NULL_HANDLE) return;

    m_Device.destroyPipeline(pipeline, nullptr, *m_DispatchHandle);
    pipeline = VK_NULL_HANDLE;
}

void AxrVulkanMaterialData::cleanupPipelineCreationData(std::vector<vk::ShaderModule>& shaderModules) const {
    for (vk::ShaderModule& shaderModule : shaderModules) {
        destroyShaderModule(shaderModule);
    }
    shaderModules.clear();
}

AxrResult AxrVulkanMaterialData::createShaderModule(
    const std::vector<char>& shaderFileData,
    vk::ShaderModule& shaderModule
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Device is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const vk::ShaderModuleCreateInfo shaderModuleCreateInfo(
        {},
        shaderFileData.size(),
        reinterpret_cast<const uint32_t*>(shaderFileData.data())
    );

    const vk::Result vkResult = m_Device.createShaderModule(
        &shaderModuleCreateInfo,
        nullptr,
        &shaderModule,
        *m_DispatchHandle
    );
    axrLogVkResult(vkResult, "m_Device.createShaderModule");
    if (VK_FAILED(vkResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrVulkanMaterialData::destroyShaderModule(vk::ShaderModule& shaderModule) const {
    if (shaderModule == VK_NULL_HANDLE) return;

    m_Device.destroyShaderModule(shaderModule, nullptr, *m_DispatchHandle);
    shaderModule = VK_NULL_HANDLE;
}

vk::Format AxrVulkanMaterialData::getVertexAttributeFormat(const AxrShaderVertexAttributeEnum vertexAttribute) const {
    switch (vertexAttribute) {
        case AXR_SHADER_VERTEX_ATTRIBUTE_POSITION:
        case AXR_SHADER_VERTEX_ATTRIBUTE_COLOR: {
            return vk::Format::eR32G32B32Sfloat;
        }
        case AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_0:
        case AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_1:
        case AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_2:
        case AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_3: {
            return vk::Format::eR32G32Sfloat;
        }
        case AXR_SHADER_VERTEX_ATTRIBUTE_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            return vk::Format::eUndefined;
        }
    }
}

uint32_t AxrVulkanMaterialData::getVertexAttributeOffset(const AxrShaderVertexAttributeEnum vertexAttribute) const {
    switch (vertexAttribute) {
        case AXR_SHADER_VERTEX_ATTRIBUTE_POSITION: {
            return offsetof(AxrVertex, Position);
        }
        case AXR_SHADER_VERTEX_ATTRIBUTE_COLOR: {
            return offsetof(AxrVertex, Color);
        }
        case AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_0: {
            return offsetof(AxrVertex, TexCoord_0);
        }
        case AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_1: {
            return offsetof(AxrVertex, TexCoord_1);
        }
        case AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_2: {
            return offsetof(AxrVertex, TexCoord_2);
        }
        case AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_3: {
            return offsetof(AxrVertex, TexCoord_3);
        }
        case AXR_SHADER_VERTEX_ATTRIBUTE_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            return 0;
        }
    }
}

#endif
