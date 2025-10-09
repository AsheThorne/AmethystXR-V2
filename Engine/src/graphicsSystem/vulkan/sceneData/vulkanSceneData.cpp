#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "vulkanSceneData.hpp"
#include "axr/logger.h"
#include "../../../assets/assetCollection.hpp"
#include "../../../assets/material.hpp"
#include "../vulkanUtils.hpp"
#include "../../../utils.hpp"
#include "../../../assets/engineAssets.hpp"
#include "../../../assets/pushConstantBuffer.hpp"

// ---- Special Functions ----

AxrVulkanSceneData::AxrVulkanSceneData(const Config& config):
    m_SceneName(config.SceneName),
    m_AssetCollection(config.AssetCollection),
    m_EcsRegistryHandle(config.EcsRegistryHandle),
    m_GlobalSceneData(config.GlobalSceneData),
    m_PhysicalDevice(config.PhysicalDevice),
    m_Device(config.Device),
    m_GraphicsCommandPool(config.GraphicsCommandPool),
    m_GraphicsQueue(config.GraphicsQueue),
    m_TransferCommandPool(config.TransferCommandPool),
    m_TransferQueue(config.TransferQueue),
    m_MaxFramesInFlight(config.MaxFramesInFlight),
    m_MaxSamplerAnisotropy(config.MaxSamplerAnisotropy),
    m_DispatchHandle(config.DispatchHandle),
    m_IsWindowDataLoaded(false),
    m_IsXrSessionDataLoaded(false),
    m_PlatformUniformBuffers(
        {
            AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA,
            AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA,
            AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS,
            AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS,
        }
    ),
    m_UIImageResourceCount(config.UIImageResourcesPreloadCount) {
}

AxrVulkanSceneData::~AxrVulkanSceneData() {
    unloadScene();
}

// ---- Public Functions ----

const std::string& AxrVulkanSceneData::getSceneName() const {
    return m_SceneName;
}

entt::registry* AxrVulkanSceneData::getEcsRegistryHandle() const {
    return m_EcsRegistryHandle;
}

AxrResult AxrVulkanSceneData::loadScene() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = m_AssetCollection->loadAssets();
    if (AXR_FAILED(axrResult)) {
        unloadScene();
        return axrResult;
    }

    axrResult = validateAllPushConstantBuffers();
    if (AXR_FAILED(axrResult)) {
        unloadScene();
        return axrResult;
    }

    axrResult = createAllUniformBufferData();
    if (AXR_FAILED(axrResult)) {
        unloadScene();
        return axrResult;
    }

    axrResult = createAllModelData();
    if (AXR_FAILED(axrResult)) {
        unloadScene();
        return axrResult;
    }

    axrResult = createAllImageSamplerData();
    if (AXR_FAILED(axrResult)) {
        unloadScene();
        return axrResult;
    }

    axrResult = createAllImageData();
    if (AXR_FAILED(axrResult)) {
        unloadScene();
        return axrResult;
    }

    axrResult = createAllMaterialLayoutData();
    if (AXR_FAILED(axrResult)) {
        unloadScene();
        return axrResult;
    }

    axrResult = createAllMaterialData();
    if (AXR_FAILED(axrResult)) {
        unloadScene();
        return axrResult;
    }

    axrResult = createAllMaterialsForRendering();
    if (AXR_FAILED(axrResult)) {
        unloadScene();
        return axrResult;
    }

    m_AssetCollection->OnPushConstantBufferCreatedCallbackGraphics
                     .connect<&AxrVulkanSceneData::onPushConstantBufferCreatedCallback>(this);
    m_AssetCollection->OnUniformBufferCreatedCallbackGraphics
                     .connect<&AxrVulkanSceneData::onUniformBufferCreatedCallback>(this);
    m_AssetCollection->OnModelCreatedCallbackGraphics
                     .connect<&AxrVulkanSceneData::onModelCreatedCallback>(this);
    m_AssetCollection->OnImageSamplerCreatedCallbackGraphics
                     .connect<&AxrVulkanSceneData::onImageSamplerCreatedCallback>(this);
    m_AssetCollection->OnImageCreatedCallbackGraphics
                     .connect<&AxrVulkanSceneData::onImageCreatedCallback>(this);
    m_AssetCollection->OnMaterialCreatedCallbackGraphics
                     .connect<&AxrVulkanSceneData::onMaterialCreatedCallback>(this);


    if (m_EcsRegistryHandle != nullptr) {
        m_EcsRegistryHandle->on_construct<AxrTransformComponent>()
                           .connect<&AxrVulkanSceneData::onNewRenderableEntityCallback>(this);
        m_EcsRegistryHandle->on_construct<AxrModelComponent>()
                           .connect<&AxrVulkanSceneData::onNewRenderableEntityCallback>(this);
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::unloadScene() {
    const vk::Result vkResult = m_Device.waitIdle(*m_DispatchHandle);
    axrLogVkResult(vkResult, "m_Device.waitIdle");

    if (m_EcsRegistryHandle != nullptr) {
        m_EcsRegistryHandle->on_construct<AxrTransformComponent>()
                           .disconnect<&AxrVulkanSceneData::onNewRenderableEntityCallback>(this);
        m_EcsRegistryHandle->on_construct<AxrModelComponent>()
                           .disconnect<&AxrVulkanSceneData::onNewRenderableEntityCallback>(this);
    }

    m_AssetCollection->OnMaterialCreatedCallbackGraphics.reset();
    m_AssetCollection->OnImageCreatedCallbackGraphics.reset();
    m_AssetCollection->OnImageSamplerCreatedCallbackGraphics.reset();
    m_AssetCollection->OnModelCreatedCallbackGraphics.reset();
    m_AssetCollection->OnUniformBufferCreatedCallbackGraphics.reset();
    m_AssetCollection->OnPushConstantBufferCreatedCallbackGraphics.reset();

    unloadWindowData();
    unloadXrSessionData();

    destroyAllMaterialsForRendering();
    destroyAllMaterialData();
    destroyAllMaterialLayoutData();
    destroyAllImageData();
    destroyAllImageSamplerData();
    destroyAllModelData();
    destroyAllUniformBufferData();
    if (m_AssetCollection != nullptr) {
        m_AssetCollection->unloadAssets();
    }
}

AxrResult AxrVulkanSceneData::loadWindowData(const LoadWindowDataConfig& config) {
    AxrResult axrResult = AXR_SUCCESS;

    m_LoadWindowDataConfig = config;

    axrResult = createAllWindowUniformBufferData();
    if (AXR_FAILED(axrResult)) {
        unloadWindowData();
        return axrResult;
    }

    axrResult = createAllWindowMaterialData();
    if (AXR_FAILED(axrResult)) {
        unloadWindowData();
        return axrResult;
    }

    m_IsWindowDataLoaded = true;

    axrResult = writeAllDescriptorSets(AXR_PLATFORM_TYPE_WINDOW, 1);
    if (AXR_FAILED(axrResult)) {
        unloadWindowData();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::unloadWindowData() {
    m_IsWindowDataLoaded = false;

    const vk::Result vkResult = m_Device.waitIdle(*m_DispatchHandle);
    axrLogVkResult(vkResult, "m_Device.waitIdle");

    resetAllDescriptorSets(AXR_PLATFORM_TYPE_WINDOW);
    destroyAllWindowMaterialData();
    destroyAllWindowUniformBufferData();

    m_LoadWindowDataConfig = {};
}

AxrResult AxrVulkanSceneData::loadXrSessionData(const LoadXrSessionDataConfig& config) {
    AxrResult axrResult = AXR_SUCCESS;

    m_LoadXrSessionDataConfig = config;

    axrResult = createAllXrSessionUniformBufferData();
    if (AXR_FAILED(axrResult)) {
        unloadXrSessionData();
        return axrResult;
    }

    axrResult = createAllXrSessionMaterialData();
    if (AXR_FAILED(axrResult)) {
        unloadXrSessionData();
        return axrResult;
    }

    m_IsXrSessionDataLoaded = true;

    axrResult = writeAllDescriptorSets(AXR_PLATFORM_TYPE_XR_DEVICE, m_LoadXrSessionDataConfig.ViewCount);
    if (AXR_FAILED(axrResult)) {
        unloadXrSessionData();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::unloadXrSessionData() {
    m_IsXrSessionDataLoaded = false;

    const vk::Result vkResult = m_Device.waitIdle(*m_DispatchHandle);
    axrLogVkResult(vkResult, "m_Device.waitIdle");

    resetAllDescriptorSets(AXR_PLATFORM_TYPE_XR_DEVICE);
    destroyAllXrSessionMaterialData();
    destroyAllXrSessionUniformBufferData();

    m_LoadXrSessionDataConfig = {};
}

const std::vector<AxrVulkanMaterialForRendering>& AxrVulkanSceneData::getMaterialsForRendering(
    const AxrMaterialAlphaRenderModeEnum alphaRenderMode
) const {
    switch (alphaRenderMode) {
        case AXR_MATERIAL_ALPHA_RENDER_MODE_OPAQUE: {
            return m_OpaqueMaterialsForRendering;
        }
        case AXR_MATERIAL_ALPHA_RENDER_MODE_ALPHA_BLEND: {
            return m_AlphaBlendMaterialsForRendering;
        }
        case AXR_MATERIAL_ALPHA_RENDER_MODE_OIT: {
            return m_OITMaterialsForRendering;
        }
        default: {
            axrLogWarningLocation("Unknown alpha render mode: {0}.", static_cast<int32_t>(alphaRenderMode));
            return m_OpaqueMaterialsForRendering;
        }
    }
}

const AxrVulkanMaterialForRendering* AxrVulkanSceneData::getUIRectangleMaterialForRendering() const {
    if (m_UIRectangleMaterialForRenderingIndex < 0 ||
        m_UIMaterialsForRendering.size() - 1 < m_UIRectangleMaterialForRenderingIndex) {
        return nullptr;
    }

    return &m_UIMaterialsForRendering[m_UIRectangleMaterialForRenderingIndex];
}

const AxrVulkanMaterialForRendering* AxrVulkanSceneData::getUIBorderMaterialForRendering() const {
    if (m_UIBorderMaterialForRenderingIndex < 0 ||
        m_UIMaterialsForRendering.size() - 1 < m_UIBorderMaterialForRenderingIndex) {
        return nullptr;
    }

    return &m_UIMaterialsForRendering[m_UIBorderMaterialForRenderingIndex];
}

const AxrVulkanMaterialForRendering* AxrVulkanSceneData::getUIImageMaterialForRendering(
    const uint32_t imageIndex
) const {
    if (m_UIImageMaterialForRenderingIndices.size() - 1 < imageIndex ||
        m_UIMaterialsForRendering.size() - 1 < m_UIImageMaterialForRenderingIndices[imageIndex]) {
        return nullptr;
    }

    return &m_UIMaterialsForRendering[m_UIImageMaterialForRenderingIndices[imageIndex]];
}

AxrResult AxrVulkanSceneData::setUniformBufferData(
    const AxrPlatformType platformType,
    const std::string& bufferName,
    const uint32_t frameIndex,
    const uint32_t viewIndex,
    const bool alignData,
    const vk::DeviceSize offset,
    const vk::DeviceSize dataSize,
    const void* data
) const {
    const AxrVulkanUniformBufferData* uniformBufferData = findUniformBufferData_shared(
        bufferName,
        platformType,
        viewIndex
    );
    if (uniformBufferData == nullptr) {
        axrLogErrorLocation("Uniform buffer named: {0} does not exist.", bufferName.c_str());
        return AXR_ERROR;
    }

    const AxrResult axrResult = uniformBufferData->setData(frameIndex, alignData, offset, dataSize, data);
    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation(
            "Failed to set uniform buffer data for buffer named: {0}. At frame index: {1}",
            bufferName.c_str(),
            frameIndex
        );
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

AxrResult AxrVulkanSceneData::setUIImageData(
    const AxrPlatformType platformType,
    const uint32_t frameIndex,
    const std::vector<AxrUIImageData*>& uiImageData
) {
    if (uiImageData.size() > m_UIImageResourceCount) {
        const AxrResult axrResult = createAdditionalUIImageMaterials(uiImageData.size());
        if (AXR_FAILED(axrResult)) {
            axrLogErrorLocation(
                "Failed to create additional ui image materials to fit: {0} images.",
                uiImageData.size()
            );
            return axrResult;
        }
    }

    uint32_t viewCount = 0;
    switch (platformType) {
        case AXR_PLATFORM_TYPE_WINDOW: {
            viewCount = 1;
            break;
        }
        case AXR_PLATFORM_TYPE_XR_DEVICE: {
            viewCount = m_LoadXrSessionDataConfig.ViewCount;
            break;
        }
        case AXR_PLATFORM_TYPE_UNDEFINED:
        default: {
            axrLogErrorLocation("Unknown platform type: {0}.", static_cast<uint32_t>(platformType));
            break;
        }
    }

    return writeUIImageDescriptorSets(platformType, frameIndex, viewCount, uiImageData);
}

const AxrPushConstantBuffer* AxrVulkanSceneData::findPushConstantBuffer_shared(const std::string& name) const {
    if (name.empty()) return nullptr;

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return nullptr;
    }

    const AxrPushConstantBuffer* foundPushConstantBuffer = m_AssetCollection->findPushConstantBuffer(name);

    if (foundPushConstantBuffer != nullptr) {
        return foundPushConstantBuffer;
    }

    if (m_GlobalSceneData != nullptr) {
        foundPushConstantBuffer = m_GlobalSceneData->findPushConstantBuffer_shared(name);

        if (foundPushConstantBuffer != nullptr) {
            return foundPushConstantBuffer;
        }
    }

    return nullptr;
}

// ---- Private Functions ----

bool AxrVulkanSceneData::isThisGlobalSceneData() const {
    return m_GlobalSceneData == nullptr;
}

bool AxrVulkanSceneData::isPlatformLoaded(const AxrPlatformType platformType) const {
    switch (platformType) {
        case AXR_PLATFORM_TYPE_WINDOW: {
            return m_IsWindowDataLoaded;
        }
        case AXR_PLATFORM_TYPE_XR_DEVICE: {
            return m_IsXrSessionDataLoaded;
        }
        case AXR_PLATFORM_TYPE_UNDEFINED:
        default: {
            axrLogErrorLocation("Unknown platform type");
            return false;
        }
    }
}

AxrResult AxrVulkanSceneData::validateAllPushConstantBuffers() const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return AXR_ERROR;
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        axrLogErrorLocation("Physical device is null.");
        return AXR_ERROR;
    }

    if (m_DispatchHandle == VK_NULL_HANDLE) {
        axrLogErrorLocation("Dispatch handle is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    const vk::PhysicalDeviceProperties properties = m_PhysicalDevice.getProperties(*m_DispatchHandle);

    for (const auto& [bufferName, buffer] : m_AssetCollection->getPushConstantBuffers()) {
        axrResult = validatePushConstantBuffer(properties, buffer);
        if (AXR_FAILED(axrResult)) {
            break;
        }
    }

    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    return AXR_SUCCESS;
}

AxrResult AxrVulkanSceneData::validatePushConstantBuffer(
    const vk::PhysicalDeviceProperties& properties,
    const AxrPushConstantBuffer& pushConstantBuffer
) const {
    if (pushConstantBuffer.getSize() <= properties.limits.maxPushConstantsSize) {
        return AXR_SUCCESS;
    }

    axrLogErrorLocation(
        "Failed push constant buffer validation for buffer named: {0}.",
        pushConstantBuffer.getName().c_str()
    );
    return AXR_ERROR;
}

void AxrVulkanSceneData::onPushConstantBufferCreatedCallback(
    const AxrPushConstantBufferConst_T pushConstantBuffer
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (pushConstantBuffer == nullptr) {
        axrLogErrorLocation("Push constant buffer is null.");
        return;
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        axrLogErrorLocation("Physical device is null.");
        return;
    }

    if (m_DispatchHandle == VK_NULL_HANDLE) {
        axrLogErrorLocation("Dispatch handle is null.");
        return;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const vk::PhysicalDeviceProperties properties = m_PhysicalDevice.getProperties(*m_DispatchHandle);
    AXR_FAILED(validatePushConstantBuffer(properties, *pushConstantBuffer));
}

void AxrVulkanSceneData::destroyUniformBufferData(
    std::unordered_map<std::string, AxrVulkanUniformBufferData>& uniformBufferData
) {
    for (auto& [name, data] : uniformBufferData) {
        data.destroyData();
    }
    uniformBufferData.clear();
}

void AxrVulkanSceneData::destroyUniformBufferData(
    std::unordered_map<std::string, std::array<AxrVulkanUniformBufferData, 2>>& uniformBufferData
) {
    for (auto& [name, data] : uniformBufferData) {
        for (auto& uniformBuffer : data) {
            uniformBuffer.destroyData();
        }
    }
    uniformBufferData.clear();
}

AxrResult AxrVulkanSceneData::createAllUniformBufferData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_UniformBufferData.empty()) {
        axrLogErrorLocation("Uniform buffer data already exists.");
        return AXR_ERROR;
    }

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    auto createUniformBuffer = [this](const AxrUniformBuffer& uniformBuffer) -> void {
        AxrResult axrResult = AXR_SUCCESS;

        AxrVulkanUniformBufferData uniformBufferData;
        axrResult = initializeUniformBufferData(uniformBuffer, uniformBufferData);
        if (AXR_FAILED(axrResult)) {
            return;
        }

        if (m_UniformBufferData.contains(uniformBufferData.getName())) {
            return;
        }

        axrResult = uniformBufferData.createData();
        if (AXR_FAILED(axrResult)) {
            return;
        }

        auto [insertData, insertSucceeded] = m_UniformBufferData.insert(
            std::pair(uniformBufferData.getName(), std::move(uniformBufferData))
        );

        if (!insertSucceeded) {
            insertData->second.destroyData();
            return;
        }
    };

    if (isThisGlobalSceneData()) {
        for (const AxrEngineAssetEnum engineAsset : m_PlatformUniformBuffers) {
            AxrUniformBuffer uniformBuffer;
            axrEngineAssetCreateUniformBuffer(engineAsset, uniformBuffer);
            m_LocalUniformBuffers.push_back(std::move(uniformBuffer));
            // Platform uniform buffers get created during the platform setup
        }
    }

    for (const auto& uniformBuffer : m_AssetCollection->getUniformBuffers() | std::views::values) {
        createUniformBuffer(uniformBuffer);
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllUniformBufferData() {
    destroyUniformBufferData(m_UniformBufferData);
    m_LocalUniformBuffers.clear();
}

AxrResult AxrVulkanSceneData::initializeUniformBufferData(
    const AxrUniformBuffer& uniformBufferHandle,
    AxrVulkanUniformBufferData& uniformBufferData
) const {
    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const AxrVulkanUniformBufferData::Config uniformBufferDataConfig{
        .UniformBufferHandle = &uniformBufferHandle,
        .MaxFramesInFlight = m_MaxFramesInFlight,
        .PhysicalDevice = m_PhysicalDevice,
        .Device = m_Device,
        .TransferCommandPool = m_TransferCommandPool,
        .TransferQueue = m_TransferQueue,
        .DispatchHandle = m_DispatchHandle,
    };

    uniformBufferData = AxrVulkanUniformBufferData(uniformBufferDataConfig);

    return AXR_SUCCESS;
}

const AxrVulkanUniformBufferData* AxrVulkanSceneData::findUniformBufferData_shared(
    const std::string& name,
    const AxrPlatformType platformType,
    const uint32_t viewIndex
) const {
    const auto foundUniformBufferDataIt = m_UniformBufferData.find(name);
    if (foundUniformBufferDataIt != m_UniformBufferData.end()) {
        return &foundUniformBufferDataIt->second;
    }

    if (m_GlobalSceneData != nullptr) {
        // platform type is undefined here because we don't want to search through the platform uniform buffers globally yet.
        const auto foundUniformBufferData = m_GlobalSceneData->findUniformBufferData_shared(
            name,
            AXR_PLATFORM_TYPE_UNDEFINED,
            viewIndex
        );

        if (foundUniformBufferData != nullptr) {
            return foundUniformBufferData;
        }
    }

    switch (platformType) {
        case AXR_PLATFORM_TYPE_WINDOW: {
            const auto foundUniformBufferData = findWindowUniformBufferData_shared(name);

            if (foundUniformBufferData != nullptr) {
                return foundUniformBufferData;
            }
            break;
        }
        case AXR_PLATFORM_TYPE_XR_DEVICE: {
            const auto foundUniformBufferData = findXrSessionUniformBufferData_shared(name, viewIndex);

            if (foundUniformBufferData != nullptr) {
                return foundUniformBufferData;
            }
            break;
        }
        case AXR_PLATFORM_TYPE_UNDEFINED: {
            // Nothing needs to be done here. It's valid to use an undefined type here.
            break;
        }
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            axrLogErrorLocation("Unknown platform type.");
            break;
        }
    }

    return nullptr;
}

const AxrUniformBuffer* AxrVulkanSceneData::findLocalUniformBuffer(const std::string& name) const {
    for (const AxrUniformBuffer& uniformBuffer : m_LocalUniformBuffers) {
        if (uniformBuffer.getName() == name) {
            return &uniformBuffer;
        }
    }

    return nullptr;
}

AxrResult AxrVulkanSceneData::createAllWindowUniformBufferData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_WindowUniformBufferData.empty()) {
        axrLogErrorLocation("Window uniform buffer data already exists.");
        return AXR_ERROR;
    }

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    if (isThisGlobalSceneData()) {
        for (const AxrEngineAssetEnum engineAsset : m_PlatformUniformBuffers) {
            const char* uniformBufferName = axrEngineAssetGetUniformBufferName(engineAsset);

            const AxrUniformBuffer* foundUniformBuffer = findLocalUniformBuffer(uniformBufferName);

            if (foundUniformBuffer == nullptr) {
                axrLogErrorLocation("Failed to find uniform buffer named: {0}.", uniformBufferName);
                continue;
            }

            AxrVulkanUniformBufferData uniformBufferData;
            axrResult = initializeUniformBufferData(*foundUniformBuffer, uniformBufferData);
            if (AXR_FAILED(axrResult)) {
                continue;
            }

            if (m_WindowUniformBufferData.contains(uniformBufferData.getName())) {
                continue;
            }

            axrResult = uniformBufferData.createData();
            if (AXR_FAILED(axrResult)) {
                continue;
            }

            auto [insertData, insertSucceeded] = m_WindowUniformBufferData.insert(
                std::pair(uniformBufferData.getName(), std::move(uniformBufferData))
            );

            if (!insertSucceeded) {
                insertData->second.destroyData();
                continue;
            }
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllWindowUniformBufferData() {
    destroyUniformBufferData(m_WindowUniformBufferData);
}

const AxrVulkanUniformBufferData* AxrVulkanSceneData::findWindowUniformBufferData_shared(
    const std::string& name
) const {
    auto foundUniformBufferDataIt = m_WindowUniformBufferData.find(name);
    if (foundUniformBufferDataIt != m_WindowUniformBufferData.end()) {
        return &foundUniformBufferDataIt->second;
    }

    if (m_GlobalSceneData != nullptr) {
        const auto foundUniformBufferData = m_GlobalSceneData->findWindowUniformBufferData_shared(name);

        if (foundUniformBufferData != nullptr) {
            return foundUniformBufferData;
        }
    }

    return nullptr;
}

AxrResult AxrVulkanSceneData::createAllXrSessionUniformBufferData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_XrSessionUniformBufferData.empty()) {
        axrLogErrorLocation("Xr session uniform buffer data already exists.");
        return AXR_ERROR;
    }

    if (m_LoadXrSessionDataConfig.ViewCount > AXR_MAX_XR_VIEWS) {
        axrLogErrorLocation("View count exceeds the view limit of: {0}.", AXR_MAX_XR_VIEWS);
        return AXR_ERROR;
    }

    if (m_LoadXrSessionDataConfig.ViewCount < 1) {
        axrLogErrorLocation("View count must be greater than 0.");
        return AXR_ERROR;
    }

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    if (isThisGlobalSceneData()) {
        for (const AxrEngineAssetEnum engineAsset : m_PlatformUniformBuffers) {
            const char* uniformBufferName = axrEngineAssetGetUniformBufferName(engineAsset);

            const AxrUniformBuffer* foundUniformBuffer = findLocalUniformBuffer(uniformBufferName);

            if (foundUniformBuffer == nullptr) {
                axrLogErrorLocation("Failed to find uniform buffer named: {0}.", uniformBufferName);
                continue;
            }

            std::array<AxrVulkanUniformBufferData, AXR_MAX_XR_VIEWS> uniformBuffers;
            for (uint32_t viewIndex = 0; viewIndex < m_LoadXrSessionDataConfig.ViewCount; ++viewIndex) {
                AxrVulkanUniformBufferData uniformBufferData;
                axrResult = initializeUniformBufferData(*foundUniformBuffer, uniformBufferData);
                if (AXR_FAILED(axrResult)) {
                    // Break instead of continue because it's only valid if every view was successful
                    break;
                }

                if (m_XrSessionUniformBufferData.contains(uniformBufferData.getName())) {
                    // Break instead of continue because it's only valid if every view was successful
                    break;
                }

                axrResult = uniformBufferData.createData();
                if (AXR_FAILED(axrResult)) {
                    // Break instead of continue because it's only valid if every view was successful
                    break;
                }

                uniformBuffers[viewIndex] = std::move(uniformBufferData);
            }

            if (AXR_FAILED(axrResult)) {
                continue;
            }

            auto [insertData, insertSucceeded] = m_XrSessionUniformBufferData.insert(
                std::pair(
                    // The first uniform buffer should always exist
                    uniformBuffers[0].getName(),
                    std::move(uniformBuffers)
                )
            );

            if (!insertSucceeded) {
                for (AxrVulkanUniformBufferData& uniformBufferData : insertData->second) {
                    uniformBufferData.destroyData();
                }
                continue;
            }
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllXrSessionUniformBufferData() {
    destroyUniformBufferData(m_XrSessionUniformBufferData);
}

const AxrVulkanUniformBufferData* AxrVulkanSceneData::findXrSessionUniformBufferData_shared(
    const std::string& name,
    const uint32_t viewIndex
) const {
    if (viewIndex > AXR_MAX_XR_VIEWS) {
        axrLogErrorLocation("View count exceeds the view limit of: {0}.", AXR_MAX_XR_VIEWS);
        return nullptr;
    }

    auto foundUniformBufferDataIt = m_XrSessionUniformBufferData.find(name);
    if (foundUniformBufferDataIt != m_XrSessionUniformBufferData.end()) {
        return &foundUniformBufferDataIt->second[viewIndex];
    }

    if (m_GlobalSceneData != nullptr) {
        const auto foundUniformBufferData = m_GlobalSceneData->findXrSessionUniformBufferData_shared(name, viewIndex);

        if (foundUniformBufferData != nullptr) {
            return foundUniformBufferData;
        }
    }

    return nullptr;
}

void AxrVulkanSceneData::onUniformBufferCreatedCallback(const AxrUniformBufferConst_T uniformBuffer) {
    if (uniformBuffer == nullptr) {
        axrLogErrorLocation("Uniform buffer is null.");
        return;
    }

    AxrResult axrResult = AXR_SUCCESS;

    AxrVulkanUniformBufferData uniformBufferData;
    axrResult = initializeUniformBufferData(*uniformBuffer, uniformBufferData);
    if (AXR_FAILED(axrResult)) {
        return;
    }

    if (m_UniformBufferData.contains(uniformBufferData.getName())) {
        return;
    }

    axrResult = uniformBufferData.createData();
    if (AXR_FAILED(axrResult)) {
        return;
    }

    auto [insertData, insertSucceeded] = m_UniformBufferData.insert(
        std::pair(uniformBufferData.getName(), std::move(uniformBufferData))
    );

    if (!insertSucceeded) {
        insertData->second.destroyData();
        return;
    }
}

AxrResult AxrVulkanSceneData::createAllModelData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_ModelData.empty()) {
        axrLogErrorLocation("Model data already exists.");
        return AXR_ERROR;
    }

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    for (const auto& model : m_AssetCollection->getModels() | std::views::values) {
        AxrVulkanModelData modelData;
        axrResult = initializeModelData(model, modelData);
        if (AXR_FAILED(axrResult)) {
            continue;
        }

        if (m_ModelData.contains(modelData.getName())) {
            continue;
        }

        axrResult = modelData.createData();
        if (AXR_FAILED(axrResult)) {
            continue;
        }

        auto [insertData, insertSucceeded] = m_ModelData.insert(
            std::pair(modelData.getName(), std::move(modelData))
        );

        if (!insertSucceeded) {
            insertData->second.destroyData();
            continue;
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllModelData() {
    for (auto& data : m_ModelData | std::views::values) {
        data.destroyData();
    }
    m_ModelData.clear();
}

AxrResult AxrVulkanSceneData::initializeModelData(const AxrModel& model, AxrVulkanModelData& modelData) const {
    const AxrVulkanModelData::Config modelDataConfig{
        .ModelHandle = &model,
        .PhysicalDevice = m_PhysicalDevice,
        .Device = m_Device,
        .TransferCommandPool = m_TransferCommandPool,
        .TransferQueue = m_TransferQueue,
        .DispatchHandle = m_DispatchHandle,
    };

    modelData = AxrVulkanModelData(modelDataConfig);

    return AXR_SUCCESS;
}

const AxrVulkanModelData* AxrVulkanSceneData::findModelData_shared(const std::string& name) const {
    const auto foundModelDataIt = m_ModelData.find(name);
    if (foundModelDataIt != m_ModelData.end()) {
        return &foundModelDataIt->second;
    }

    if (m_GlobalSceneData != nullptr) {
        const auto foundModelData = m_GlobalSceneData->findModelData_shared(name);

        if (foundModelData != nullptr) {
            return foundModelData;
        }
    }

    return nullptr;
}

void AxrVulkanSceneData::onModelCreatedCallback(const AxrModelConst_T model) {
    if (model == nullptr) {
        axrLogErrorLocation("Model is null.");
        return;
    }

    AxrResult axrResult = AXR_SUCCESS;

    AxrVulkanModelData modelData;
    axrResult = initializeModelData(*model, modelData);
    if (AXR_FAILED(axrResult)) {
        return;
    }

    if (m_ModelData.contains(modelData.getName())) {
        return;
    }

    axrResult = modelData.createData();
    if (AXR_FAILED(axrResult)) {
        return;
    }

    auto [insertData, insertSucceeded] = m_ModelData.insert(
        std::pair(modelData.getName(), std::move(modelData))
    );

    if (!insertSucceeded) {
        insertData->second.destroyData();
        return;
    }
}

AxrResult AxrVulkanSceneData::createAllImageData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_ImageData.empty()) {
        axrLogErrorLocation("Image data already exists.");
        return AXR_ERROR;
    }

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    for (const auto& image : m_AssetCollection->getImages() | std::views::values) {
        AxrVulkanImageData imageData;
        axrResult = initializeImageData(image, imageData);
        if (AXR_FAILED(axrResult)) {
            continue;
        }

        if (m_ImageData.contains(imageData.getName())) {
            continue;
        }

        axrResult = imageData.createData();
        if (AXR_FAILED(axrResult)) {
            continue;
        }

        auto [insertData, insertSucceeded] = m_ImageData.insert(
            std::pair(imageData.getName(), std::move(imageData))
        );

        if (!insertSucceeded) {
            insertData->second.destroyData();
            continue;
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllImageData() {
    for (auto& data : m_ImageData | std::views::values) {
        data.destroyData();
    }
    m_ImageData.clear();
}

AxrResult AxrVulkanSceneData::initializeImageData(const AxrImage& image, AxrVulkanImageData& imageData) const {
    const AxrVulkanImageData::Config imageDataConfig{
        .ImageHandle = &image,
        .PhysicalDevice = m_PhysicalDevice,
        .Device = m_Device,
        .GraphicsCommandPool = m_GraphicsCommandPool,
        .GraphicsQueue = m_GraphicsQueue,
        .DispatchHandle = m_DispatchHandle,
    };

    imageData = AxrVulkanImageData(imageDataConfig);

    return AXR_SUCCESS;
}

const AxrVulkanImageData* AxrVulkanSceneData::findImageData_shared(const std::string& name) const {
    const auto foundImageDataIt = m_ImageData.find(name);
    if (foundImageDataIt != m_ImageData.end()) {
        return &foundImageDataIt->second;
    }

    if (m_GlobalSceneData != nullptr) {
        const auto foundImageData = m_GlobalSceneData->findImageData_shared(name);

        if (foundImageData != nullptr) {
            return foundImageData;
        }
    }

    return nullptr;
}

void AxrVulkanSceneData::onImageCreatedCallback(const AxrImageConst_T image) {
    if (image == nullptr) {
        axrLogErrorLocation("Image is null.");
        return;
    }

    AxrResult axrResult = AXR_SUCCESS;

    AxrVulkanImageData imageData;
    axrResult = initializeImageData(*image, imageData);
    if (AXR_FAILED(axrResult)) {
        return;
    }

    if (m_ImageData.contains(imageData.getName())) {
        return;
    }

    axrResult = imageData.createData();
    if (AXR_FAILED(axrResult)) {
        return;
    }

    auto [insertData, insertSucceeded] = m_ImageData.insert(
        std::pair(imageData.getName(), std::move(imageData))
    );

    if (!insertSucceeded) {
        insertData->second.destroyData();
        return;
    }
}

AxrResult AxrVulkanSceneData::createAllImageSamplerData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_ImageSamplerData.empty()) {
        axrLogErrorLocation("Image sampler data already exists.");
        return AXR_ERROR;
    }

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    for (const auto& imageSampler : m_AssetCollection->getImageSamplers() | std::views::values) {
        AxrVulkanImageSamplerData imageSamplerData;
        axrResult = initializeImageSamplerData(imageSampler, imageSamplerData);
        if (AXR_FAILED(axrResult)) {
            continue;
        }

        if (m_ImageSamplerData.contains(imageSamplerData.getName())) {
            continue;
        }

        axrResult = imageSamplerData.createData();
        if (AXR_FAILED(axrResult)) {
            continue;
        }

        auto [insertData, insertSucceeded] = m_ImageSamplerData.insert(
            std::pair(imageSamplerData.getName(), std::move(imageSamplerData))
        );

        if (!insertSucceeded) {
            insertData->second.destroyData();
            continue;
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllImageSamplerData() {
    for (auto& data : m_ImageSamplerData | std::views::values) {
        data.destroyData();
    }
    m_ImageSamplerData.clear();
}

AxrResult AxrVulkanSceneData::initializeImageSamplerData(
    const AxrImageSampler& imageSampler,
    AxrVulkanImageSamplerData& imageSamplerData
) const {
    const AxrVulkanImageSamplerData::Config imageSamplerDataConfig{
        .ImageSamplerHandle = &imageSampler,
        .PhysicalDevice = m_PhysicalDevice,
        .Device = m_Device,
        .MaxSamplerAnisotropy = m_MaxSamplerAnisotropy,
        .DispatchHandle = m_DispatchHandle,
    };

    imageSamplerData = AxrVulkanImageSamplerData(imageSamplerDataConfig);

    return AXR_SUCCESS;
}

const AxrVulkanImageSamplerData* AxrVulkanSceneData::findImageSamplerData_shared(const std::string& name) const {
    const auto foundImageSamplerDataIt = m_ImageSamplerData.find(name);
    if (foundImageSamplerDataIt != m_ImageSamplerData.end()) {
        return &foundImageSamplerDataIt->second;
    }

    if (m_GlobalSceneData != nullptr) {
        const auto foundImageSamplerData = m_GlobalSceneData->findImageSamplerData_shared(name);

        if (foundImageSamplerData != nullptr) {
            return foundImageSamplerData;
        }
    }

    return nullptr;
}

void AxrVulkanSceneData::onImageSamplerCreatedCallback(const AxrImageSamplerConst_T imageSampler) {
    if (imageSampler == nullptr) {
        axrLogErrorLocation("Image sampler is null.");
        return;
    }

    AxrResult axrResult = AXR_SUCCESS;

    AxrVulkanImageSamplerData imageSamplerData;
    axrResult = initializeImageSamplerData(*imageSampler, imageSamplerData);
    if (AXR_FAILED(axrResult)) {
        return;
    }

    if (m_ImageSamplerData.contains(imageSamplerData.getName())) {
        return;
    }

    axrResult = imageSamplerData.createData();
    if (AXR_FAILED(axrResult)) {
        return;
    }

    auto [insertData, insertSucceeded] = m_ImageSamplerData.insert(
        std::pair(imageSamplerData.getName(), std::move(imageSamplerData))
    );

    if (!insertSucceeded) {
        insertData->second.destroyData();
        return;
    }
}

const AxrShader* AxrVulkanSceneData::findShader_shared(const std::string& name) const {
    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return nullptr;
    }

    const AxrShader* foundShader = m_AssetCollection->findShader(name);

    if (foundShader != nullptr) {
        return foundShader;
    }

    if (m_GlobalSceneData != nullptr) {
        foundShader = m_GlobalSceneData->findShader_shared(name);

        if (foundShader != nullptr) {
            return foundShader;
        }
    }

    return nullptr;
}

AxrResult AxrVulkanSceneData::createAllMaterialLayoutData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_MaterialLayoutData.empty()) {
        axrLogErrorLocation("Material layout data already exists.");
        return AXR_ERROR;
    }

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    auto createMaterialLayout = [this](const AxrMaterial& material) -> void {
        AxrResult axrResult = AXR_SUCCESS;

        AxrVulkanMaterialLayoutData materialLayoutData;
        axrResult = initializeMaterialLayoutData(material, materialLayoutData);
        if (AXR_FAILED(axrResult)) {
            return;
        }

        if (m_MaterialLayoutData.contains(materialLayoutData.getName())) {
            return;
        }

        axrResult = materialLayoutData.createData();
        if (AXR_FAILED(axrResult)) {
            return;
        }

        auto [insertData, insertSucceeded] = m_MaterialLayoutData.insert(
            std::pair(materialLayoutData.getName(), std::move(materialLayoutData))
        );

        if (!insertSucceeded) {
            insertData->second.destroyData();
            return;
        }
    };

    if (isThisGlobalSceneData()) {
        std::vector<AxrEngineAssetEnum> materialShaders;

        // ---- Rectangle UI ----
        AxrMaterial uiRectangleMaterial;
        axrEngineAssetCreateMaterial_UIRectangle(uiRectangleMaterial, materialShaders);
        m_LocalMaterials.push_back(std::move(uiRectangleMaterial));

        createMaterialLayout(m_LocalMaterials.back());

        // ---- Border UI ----
        AxrMaterial uiBorderMaterial;
        axrEngineAssetCreateMaterial_UIBorder(uiBorderMaterial, materialShaders);
        m_LocalMaterials.push_back(std::move(uiBorderMaterial));

        createMaterialLayout(m_LocalMaterials.back());

        // ---- Image UI ----
        AxrMaterial uiImageMaterial;
        axrEngineAssetCreateMaterial_UIImage(uiImageMaterial, materialShaders);
        m_LocalMaterials.push_back(std::move(uiImageMaterial));

        createMaterialLayout(m_LocalMaterials.back());
    }

    for (const auto& material : m_AssetCollection->getMaterials() | std::views::values) {
        createMaterialLayout(material);
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllMaterialLayoutData() {
    for (auto& data : m_MaterialLayoutData | std::views::values) {
        data.destroyData();
    }
    m_MaterialLayoutData.clear();
    m_LocalMaterials.clear();
}

AxrResult AxrVulkanSceneData::initializeMaterialLayoutData(
    const AxrMaterial& material,
    AxrVulkanMaterialLayoutData& materialLayoutData
) const {
    const AxrShader* foundVertexShader = findShader_shared(material.getVertexShaderName());
    if (foundVertexShader == nullptr) {
        axrLogErrorLocation(
            "Failed to find vertex shader named: {0}.",
            material.getVertexShaderName()
        );
        return AXR_ERROR;
    }

    const AxrShader* foundFragmentShader = findShader_shared(material.getFragmentShaderName());
    if (foundFragmentShader == nullptr) {
        axrLogErrorLocation(
            "Failed to find fragment shader named: {0}.",
            material.getFragmentShaderName()
        );
        return AXR_ERROR;
    }

    const std::string materialLayoutName = material.getMaterialLayoutName();
    const AxrVulkanMaterialLayoutData::Config materialLayoutDataConfig{
        .Name = materialLayoutName,
        .VertexShaderHandle = foundVertexShader,
        .FragmentShaderHandle = foundFragmentShader,
        .Device = m_Device,
        .DispatchHandle = m_DispatchHandle,
    };

    materialLayoutData = AxrVulkanMaterialLayoutData(materialLayoutDataConfig);

    return AXR_SUCCESS;
}

const AxrVulkanMaterialLayoutData* AxrVulkanSceneData::findMaterialLayoutData_shared(const std::string& name) const {
    const auto foundMaterialLayoutDataIt = m_MaterialLayoutData.find(name);
    if (foundMaterialLayoutDataIt != m_MaterialLayoutData.end()) {
        return &foundMaterialLayoutDataIt->second;
    }

    if (m_GlobalSceneData != nullptr) {
        const auto foundMaterialLayoutData = m_GlobalSceneData->findMaterialLayoutData_shared(name);

        if (foundMaterialLayoutData != nullptr) {
            return foundMaterialLayoutData;
        }
    }

    return nullptr;
}

AxrResult AxrVulkanSceneData::createAllMaterialData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_MaterialData.empty()) {
        axrLogErrorLocation("Material data already exists.");
        return AXR_ERROR;
    }

    if (m_AssetCollection == nullptr) {
        axrLogErrorLocation("Asset collection is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    // Don't create ui material data in the global scene data. They're scene specific.
    if (m_GlobalSceneData != nullptr) {
        // The UI materials are always stored in the global scene data.
        // We just need the material data to be in the scene specific data
        for (const AxrMaterial& material : m_GlobalSceneData->m_LocalMaterials) {
            if (material.getName() == axrEngineAssetGetMaterialName(AXR_ENGINE_ASSET_MATERIAL_UI_IMAGE)) {
                for (int i = 0; i < m_MaxUIImageCount; ++i) {
                    createMaterialData(material, std::to_string(i));
                }
            } else {
                createMaterialData(material);
            }
        }
    }

    for (const auto& material : m_AssetCollection->getMaterials() | std::views::values) {
        createMaterialData(material);
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllMaterialData() {
    for (auto& data : m_MaterialData | std::views::values) {
        data.destroyData();
    }
    m_MaterialData.clear();
}

AxrVulkanMaterialData* AxrVulkanSceneData::createMaterialData(
    const AxrMaterial& material,
    const std::string& nameSuffix
) {
    AxrResult axrResult = AXR_SUCCESS;

    AxrVulkanMaterialData materialData;
    axrResult = initializeMaterialData(material, materialData, nameSuffix);
    if (AXR_FAILED(axrResult)) {
        return nullptr;
    }

    if (m_MaterialData.contains(materialData.getName())) {
        axrLogErrorLocation(
            "Material data named: {0} already exists.",
            (material.getName() + nameSuffix).c_str()
        );
        return nullptr;
    }

    axrResult = materialData.createData();
    if (AXR_FAILED(axrResult)) {
        return nullptr;
    }

    if (isPlatformLoaded(AXR_PLATFORM_TYPE_WINDOW)) {
        axrResult = materialData.createWindowData(
            m_LoadWindowDataConfig.RenderPass,
            m_LoadWindowDataConfig.MsaaSampleCount
        );

        if (AXR_FAILED(axrResult)) {
            materialData.destroyWindowData();
            // Don't return. One platform may error but the other might still be ok.
        }
    }

    if (isPlatformLoaded(AXR_PLATFORM_TYPE_XR_DEVICE)) {
        axrResult = materialData.createXrSessionData(
            m_LoadXrSessionDataConfig.RenderPass,
            m_LoadXrSessionDataConfig.MsaaSampleCount,
            m_LoadXrSessionDataConfig.ViewCount
        );

        if (AXR_FAILED(axrResult)) {
            materialData.destroyXrSessionData();
            // Don't return. One platform may error but the other might still be ok.
        }
    }

    auto [insertData, insertSucceeded] = m_MaterialData.insert(
        std::pair(materialData.getName(), std::move(materialData))
    );

    if (!insertSucceeded) {
        insertData->second.destroyXrSessionData();
        insertData->second.destroyWindowData();
        insertData->second.destroyData();
        axrLogErrorLocation(
            "Failed to insert material data for material named: {0}",
            (material.getName() + nameSuffix).c_str()
        );
        return nullptr;
    }

    if (isPlatformLoaded(AXR_PLATFORM_TYPE_WINDOW)) {
        axrResult = writeDescriptorSets(AXR_PLATFORM_TYPE_WINDOW, 1, insertData->second);

        if (AXR_FAILED(axrResult)) {
            resetDescriptorSets(AXR_PLATFORM_TYPE_WINDOW, insertData->second);
            // Don't return. One platform may error but the other might still be ok.
        }
    }

    if (isPlatformLoaded(AXR_PLATFORM_TYPE_XR_DEVICE)) {
        axrResult = writeDescriptorSets(
            AXR_PLATFORM_TYPE_XR_DEVICE,
            m_LoadXrSessionDataConfig.ViewCount,
            insertData->second
        );

        if (AXR_FAILED(axrResult)) {
            resetDescriptorSets(AXR_PLATFORM_TYPE_XR_DEVICE, insertData->second);
            // Don't return. One platform may error but the other might still be ok.
        }
    }

    return &insertData->second;
}

AxrResult AxrVulkanSceneData::initializeMaterialData(
    const AxrMaterial& material,
    AxrVulkanMaterialData& materialData,
    const std::string& nameSuffix
) const {
    const AxrVulkanMaterialLayoutData* foundMaterialLayoutData = findMaterialLayoutData_shared(
        material.getMaterialLayoutName()
    );
    if (foundMaterialLayoutData == nullptr) {
        axrLogErrorLocation(
            "Failed to find material layout data for material: {0}.",
            material.getName()
        );
        return AXR_ERROR;
    }

    const AxrVulkanMaterialData::Config materialDataConfig{
        .MaterialHandle = &material,
        .MaterialNameSuffix = nameSuffix,
        .MaterialLayoutData = foundMaterialLayoutData,
        .MaxFramesInFlight = m_MaxFramesInFlight,
        .Device = m_Device,
        .DispatchHandle = m_DispatchHandle,
    };

    materialData = AxrVulkanMaterialData(materialDataConfig);

    return AXR_SUCCESS;
}

AxrResult AxrVulkanSceneData::createAdditionalUIImageMaterials(const uint32_t minImageCount) {
    const AxrVulkanModelData* foundModelData = findModelData_shared(
        axrEngineAssetGetModelName(AXR_ENGINE_ASSET_MODEL_UI_RECTANGLE)
    );
    if (foundModelData == nullptr) {
        axrLogErrorLocation("Failed to find UI Rectangle model asset.");
        return AXR_ERROR;
    }

    // ---- Get number of materials to create ----

    /// How many image materials to create when we exceed the current m_UIImageResourceCount  
    constexpr uint32_t uiImageIncrementCount = 16;

    /// The total number of image materials we need to accommidate the given `minImageCount` while still being a
    /// multiple of `uiImageIncrementCount`
    const uint32_t newUIImageMaterialCount = (uiImageIncrementCount + minImageCount - 1) & ~(minImageCount - 1);
    /// How many new materials we're gonna create
    const uint32_t uiImageMaterialCreateCount = newUIImageMaterialCount - m_UIImageResourceCount;

    // ---- Create material data ----

    const AxrMaterial* uiImageMaterial = findLocalMaterial_shared(
        axrEngineAssetGetMaterialName(AXR_ENGINE_ASSET_MATERIAL_UI_IMAGE)
    );

    if (uiImageMaterial == nullptr) {
        axrLogErrorLocation("Failed to find ui image material.");
        return AXR_ERROR;
    }

    for (int i = 0; i < uiImageMaterialCreateCount; ++i) {
        const AxrVulkanMaterialData* materialData = createMaterialData(
            *uiImageMaterial,
            std::to_string(m_UIImageResourceCount + i)
        );
        if (materialData == nullptr) {
            axrLogErrorLocation("Failed to create UI Image material data.");
            continue;
        }

        AxrVulkanMaterialForRendering materialForRendering;
        const AxrResult axrResult = buildUIMaterialForRendering(materialData, foundModelData, materialForRendering);
        if (AXR_SUCCEEDED(axrResult)) {
            m_UIImageMaterialForRenderingIndices.push_back(static_cast<int32_t>(m_UIMaterialsForRendering.size()));
            m_UIMaterialsForRendering.push_back(std::move(materialForRendering));
        }
    }

    m_UIImageResourceCount = newUIImageMaterialCount;

    return AXR_SUCCESS;
}

AxrResult AxrVulkanSceneData::createAllWindowMaterialData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_LoadWindowDataConfig.RenderPass == VK_NULL_HANDLE) {
        axrLogErrorLocation("Window render pass is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    for (auto& data : m_MaterialData | std::views::values) {
        axrResult = data.createWindowData(
            m_LoadWindowDataConfig.RenderPass,
            m_LoadWindowDataConfig.MsaaSampleCount
        );
        if (AXR_FAILED(axrResult)) {
            continue;
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllWindowMaterialData() {
    for (auto& data : m_MaterialData | std::views::values) {
        data.destroyWindowData();
    }
}

AxrResult AxrVulkanSceneData::createAllXrSessionMaterialData() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_LoadXrSessionDataConfig.RenderPass == VK_NULL_HANDLE) {
        axrLogErrorLocation("Xr session render pass is null.");
        return AXR_ERROR;
    }

    if (m_LoadXrSessionDataConfig.ViewCount == 0) {
        axrLogErrorLocation("Xr session view count is 0.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    for (auto& data : m_MaterialData | std::views::values) {
        axrResult = data.createXrSessionData(
            m_LoadXrSessionDataConfig.RenderPass,
            m_LoadXrSessionDataConfig.MsaaSampleCount,
            m_LoadXrSessionDataConfig.ViewCount
        );
        if (AXR_FAILED(axrResult)) {
            continue;
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllXrSessionMaterialData() {
    for (auto& data : m_MaterialData | std::views::values) {
        data.destroyXrSessionData();
    }
}

const AxrVulkanMaterialData* AxrVulkanSceneData::findMaterialData_shared(const std::string& name) const {
    const auto foundMaterialDataIt = m_MaterialData.find(name);
    if (foundMaterialDataIt != m_MaterialData.end()) {
        return &foundMaterialDataIt->second;
    }

    if (m_GlobalSceneData != nullptr) {
        const auto foundMaterialData = m_GlobalSceneData->findMaterialData_shared(name);

        if (foundMaterialData != nullptr) {
            return foundMaterialData;
        }
    }

    return nullptr;
}

const AxrMaterial* AxrVulkanSceneData::findLocalMaterial_shared(const std::string& name) const {
    for (const auto& material : m_LocalMaterials) {
        if (material.getName() == name) {
            return &material;
        }
    }

    if (m_GlobalSceneData != nullptr) {
        const auto foundMaterial = m_GlobalSceneData->findLocalMaterial_shared(name);

        if (foundMaterial != nullptr) {
            return foundMaterial;
        }
    }

    return nullptr;
}

void AxrVulkanSceneData::onMaterialCreatedCallback(const AxrMaterialConst_T material) {
    if (material == nullptr) {
        axrLogErrorLocation("Material is null.");
        return;
    }

    AxrResult axrResult = AXR_SUCCESS;

    if (!m_MaterialLayoutData.contains(material->getMaterialLayoutName())) {
        AxrVulkanMaterialLayoutData materialLayoutData;
        axrResult = initializeMaterialLayoutData(*material, materialLayoutData);
        if (AXR_FAILED(axrResult)) {
            return;
        }

        axrResult = materialLayoutData.createData();
        if (AXR_FAILED(axrResult)) {
            return;
        }

        auto [insertData, insertSucceeded] = m_MaterialLayoutData.insert(
            std::pair(materialLayoutData.getName(), std::move(materialLayoutData))
        );

        if (!insertSucceeded) {
            insertData->second.destroyData();
            return;
        }
    }

    AxrVulkanMaterialData* materialData = createMaterialData(*material);
    if (materialData == nullptr) {
        return;
    }
}

AxrResult AxrVulkanSceneData::writeAllDescriptorSets(const AxrPlatformType platformType, const uint32_t viewCount) {
    AxrResult axrResult = AXR_SUCCESS;

    for (auto& data : m_MaterialData | std::views::values) {
        axrResult = writeDescriptorSets(platformType, viewCount, data);

        if (AXR_FAILED(axrResult)) {
            continue;
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::resetAllDescriptorSets(const AxrPlatformType platformType) {
    for (auto& data : m_MaterialData | std::views::values) {
        resetDescriptorSets(platformType, data);
    }
}

AxrResult AxrVulkanSceneData::writeDescriptorSets(
    const AxrPlatformType platformType,
    const uint32_t viewCount,
    AxrVulkanMaterialData& materialData
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!isPlatformLoaded(platformType)) {
        // The platform hasn't loaded so nothing to do
        return AXR_SUCCESS;
    }

    if (m_Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Device is null.");
        return AXR_ERROR;
    }

    if (m_DispatchHandle == nullptr) {
        axrLogErrorLocation("Dispatch Handle is null.");
        return AXR_ERROR;
    }

    const AxrVulkanMaterialLayoutData* materialLayoutData = materialData.getMaterialLayoutData();
    if (materialLayoutData == nullptr) {
        axrLogErrorLocation("Material layout data is null.");
        return AXR_ERROR;
    }

    const AxrMaterial* material = materialData.getMaterial();
    if (material == nullptr) {
        axrLogErrorLocation("Material is null.");
        return AXR_ERROR;
    }

    const std::vector<vk::DescriptorSet>& descriptorSets = materialData.getDescriptorSets(platformType);
    if (descriptorSets.empty()) {
        axrLogErrorLocation("Descriptor sets are empty.");
        return AXR_ERROR;
    }

    if (m_MaxFramesInFlight * viewCount != descriptorSets.size()) {
        axrLogErrorLocation("View count doesn't match what was used for descriptor set creation.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    const std::vector<AxrShaderUniformBufferLinkConst_T> uniformBufferLinks = material->getUniformBufferLinks();
    const std::vector<AxrShaderImageSamplerBufferLinkConst_T> imageSamplerBufferLinks =
        material->getImageSamplerBufferLinks();
    std::vector<vk::WriteDescriptorSet> descriptorWrites;
    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
    std::vector<vk::DescriptorImageInfo> descriptorImageInfos;
    const size_t maxWrites =
        (uniformBufferLinks.size() +
            imageSamplerBufferLinks.size()) *
        m_MaxFramesInFlight *
        viewCount;

    descriptorWrites.reserve(maxWrites);
    descriptorBufferInfos.reserve(maxWrites);
    descriptorImageInfos.reserve(maxWrites);

    for (const AxrShaderUniformBufferLinkConst_T uniformBuffer : uniformBufferLinks) {
        for (int viewIndex = 0; viewIndex < viewCount; ++viewIndex) {
            const AxrVulkanUniformBufferData* foundUniformBufferData = findUniformBufferData_shared(
                uniformBuffer->BufferName,
                platformType,
                viewIndex
            );

            if (foundUniformBufferData == nullptr) {
                axrLogErrorLocation("Failed to find uniform buffer named: {0}.", uniformBuffer->BufferName);
                axrResult = AXR_ERROR;
                break;
            }

            for (uint32_t frameIndex = 0; frameIndex < m_MaxFramesInFlight; ++frameIndex) {
                descriptorBufferInfos.emplace_back(
                    foundUniformBufferData->getBuffer(frameIndex).getBuffer(),
                    0,
                    foundUniformBufferData->getInstanceSize()
                );

                const uint32_t viewIndexOffset = m_MaxFramesInFlight * viewIndex;

                descriptorWrites.emplace_back(
                    descriptorSets[viewIndexOffset + frameIndex],
                    uniformBuffer->Binding,
                    0,
                    1,
                    axrToVkDescriptorType(foundUniformBufferData->getBufferType()),
                    nullptr,
                    &descriptorBufferInfos.back(),
                    nullptr
                );
            }

            if (AXR_FAILED(axrResult)) {
                break;
            }
        }

        if (AXR_FAILED(axrResult)) {
            break;
        }
    }

    for (const AxrShaderImageSamplerBufferLinkConst_T imageSamplerBuffer : imageSamplerBufferLinks) {
        const AxrVulkanImageSamplerData* foundImageSamplerData = findImageSamplerData_shared(
            imageSamplerBuffer->ImageSamplerName
        );

        if (foundImageSamplerData == nullptr) {
            axrLogErrorLocation("Failed to find image sampler named: {0}.", imageSamplerBuffer->ImageSamplerName);
            axrResult = AXR_ERROR;
            break;
        }

        const AxrVulkanImageData* foundImageData = findImageData_shared(imageSamplerBuffer->ImageName);
        if (foundImageData == nullptr) {
            // If image data wasn't found, use the "Missing Texture" image
            foundImageData = findImageData_shared(
                axrEngineAssetGetImageName(AXR_ENGINE_ASSET_IMAGE_MISSING_TEXTURE)
            );

            if (foundImageData == nullptr) {
                axrLogErrorLocation("Failed to find image named: {0}.", imageSamplerBuffer->ImageName);
                axrResult = AXR_ERROR;
                break;
            }

            // When we use the 'missing texture', try to use the image sampler options NEAREST and REPEAT. otherwise it looks weird
            const AxrVulkanImageSamplerData* missingTextureImageSamplerData = findImageSamplerData_shared(
                axrEngineAssetGetImageSamplerName(AXR_ENGINE_ASSET_IMAGE_SAMPLER_NEAREST_REPEAT)
            );

            if (missingTextureImageSamplerData != nullptr) {
                foundImageSamplerData = missingTextureImageSamplerData;
            }
        }

        descriptorImageInfos.emplace_back(
            foundImageSamplerData->getSampler(foundImageData->getImageFormat()),
            foundImageData->getImageView(),
            vk::ImageLayout::eShaderReadOnlyOptimal
        );

        for (int viewIndex = 0; viewIndex < viewCount; ++viewIndex) {
            for (uint32_t frameIndex = 0; frameIndex < m_MaxFramesInFlight; ++frameIndex) {
                const uint32_t viewIndexOffset = m_MaxFramesInFlight * viewIndex;

                descriptorWrites.emplace_back(
                    descriptorSets[viewIndexOffset + frameIndex],
                    imageSamplerBuffer->Binding,
                    0,
                    1,
                    vk::DescriptorType::eCombinedImageSampler,
                    &descriptorImageInfos.back(),
                    nullptr,
                    nullptr
                );
            }
        }
    }

    if (AXR_FAILED(axrResult)) {
        resetDescriptorSets(platformType, materialData);
        return axrResult;
    }

    m_Device.updateDescriptorSets(
        static_cast<uint32_t>(descriptorWrites.size()),
        descriptorWrites.data(),
        0,
        nullptr,
        *m_DispatchHandle
    );

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::resetDescriptorSets(
    const AxrPlatformType platformType,
    AxrVulkanMaterialData& materialData
) const {
    materialData.resetDescriptorSets(platformType);
}

AxrResult AxrVulkanSceneData::writeUIImageDescriptorSets(
    const AxrPlatformType platformType,
    const uint32_t frameIndex,
    const uint32_t viewCount,
    const std::vector<AxrUIImageData*>& uiImageData
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!isPlatformLoaded(platformType)) {
        // The platform hasn't loaded so nothing to do
        return AXR_SUCCESS;
    }

    if (m_Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Device is null.");
        return AXR_ERROR;
    }

    if (m_DispatchHandle == nullptr) {
        axrLogErrorLocation("Dispatch Handle is null.");
        return AXR_ERROR;
    }

    if (frameIndex > m_MaxFramesInFlight - 1) {
        axrLogErrorLocation("Frame index out of bounds.");
        return AXR_ERROR;
    }

    if (uiImageData.empty()) {
        return AXR_SUCCESS;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    std::vector<vk::WriteDescriptorSet> descriptorWrites;
    std::vector<vk::DescriptorImageInfo> descriptorImageInfos;
    const size_t maxWrites =
        uiImageData.size() *
        m_MaxFramesInFlight *
        viewCount;

    descriptorWrites.reserve(maxWrites);
    descriptorImageInfos.reserve(maxWrites);
    int32_t binding = -1;

    for (size_t imageIndex = 0; imageIndex < uiImageData.size(); ++imageIndex) {
        if (uiImageData[imageIndex] == nullptr) continue;

        std::string materialName = axrEngineAssetGetMaterialName(AXR_ENGINE_ASSET_MATERIAL_UI_IMAGE) + std::to_string(
            imageIndex
        );
        const AxrVulkanMaterialData* materialData = findMaterialData_shared(materialName);
        if (materialData == nullptr) {
            axrLogErrorLocation("Failed to find ui image material data named: {0}.", materialName.c_str());
            continue;
        }

        // We only need to get the binding once. it'll be the same for all images
        if (binding < 0) {
            const AxrMaterial* material = materialData->getMaterial();
            if (material == nullptr) {
                axrLogErrorLocation(
                    "Failed to get material from material data for material named: {0}.",
                    materialName.c_str()
                );
                continue;
            }

            const std::vector<AxrShaderImageSamplerBufferLinkConst_T> imageSamplerBufferLinks =
                material->getImageSamplerBufferLinks();
            if (imageSamplerBufferLinks.empty() || imageSamplerBufferLinks[0] == nullptr) {
                axrLogErrorLocation(
                    "Failed to get image shader binding for material named: {0}.",
                    materialName.c_str()
                );
                continue;
            }

            binding = static_cast<int32_t>(imageSamplerBufferLinks[0]->Binding);
        }

        const std::vector<vk::DescriptorSet>& descriptorSets = materialData->getDescriptorSets(platformType);
        if (descriptorSets.empty()) {
            axrLogErrorLocation("Descriptor sets are empty for material data named: {0}.", materialName.c_str());
            continue;
        }

        if (m_MaxFramesInFlight * viewCount != descriptorSets.size()) {
            axrLogErrorLocation("View count doesn't match what was used for descriptor set creation.");
            continue;
        }

        const AxrVulkanImageSamplerData* foundImageSamplerData = findImageSamplerData_shared(
            uiImageData[imageIndex]->ImageSamplerName
        );

        if (foundImageSamplerData == nullptr) {
            axrLogErrorLocation("Failed to find image sampler named: {0}.", uiImageData[imageIndex]->ImageSamplerName);
            continue;
        }

        const AxrVulkanImageData* foundImageData = findImageData_shared(uiImageData[imageIndex]->ImageName);
        if (foundImageData == nullptr) {
            // If image data wasn't found, use the "Missing Texture" image
            foundImageData = findImageData_shared(
                axrEngineAssetGetImageName(AXR_ENGINE_ASSET_IMAGE_MISSING_TEXTURE)
            );

            if (foundImageData == nullptr) {
                axrLogErrorLocation("Failed to find image named: {0}.", uiImageData[imageIndex]->ImageName);
                continue;
            }

            // When we use the 'missing texture', try to use the image sampler options NEAREST and REPEAT. otherwise it looks weird
            const AxrVulkanImageSamplerData* missingTextureImageSamplerData = findImageSamplerData_shared(
                axrEngineAssetGetImageSamplerName(AXR_ENGINE_ASSET_IMAGE_SAMPLER_NEAREST_REPEAT)
            );

            if (missingTextureImageSamplerData != nullptr) {
                foundImageSamplerData = missingTextureImageSamplerData;
            }
        }

        descriptorImageInfos.emplace_back(
            foundImageSamplerData->getSampler(foundImageData->getImageFormat()),
            foundImageData->getImageView(),
            vk::ImageLayout::eShaderReadOnlyOptimal
        );

        for (int viewIndex = 0; viewIndex < viewCount; ++viewIndex) {
            const uint32_t viewIndexOffset = m_MaxFramesInFlight * viewIndex;

            descriptorWrites.emplace_back(
                descriptorSets[viewIndexOffset + frameIndex],
                binding,
                0,
                1,
                vk::DescriptorType::eCombinedImageSampler,
                &descriptorImageInfos.back(),
                nullptr,
                nullptr
            );
        }
    }

    if (descriptorWrites.empty()) {
        return AXR_SUCCESS;
    }

    m_Device.updateDescriptorSets(
        static_cast<uint32_t>(descriptorWrites.size()),
        descriptorWrites.data(),
        0,
        nullptr,
        *m_DispatchHandle
    );

    return AXR_SUCCESS;
}

AxrResult AxrVulkanSceneData::createAllMaterialsForRendering() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_OpaqueMaterialsForRendering.empty()) {
        axrLogErrorLocation("Opaque materials for rendering already exist.");
        return AXR_ERROR;
    }

    if (!m_AlphaBlendMaterialsForRendering.empty()) {
        axrLogErrorLocation("Simple transparency materials for rendering already exist.");
        return AXR_ERROR;
    }

    if (!m_OITMaterialsForRendering.empty()) {
        axrLogErrorLocation("Advanced transparency materials for rendering already exist.");
        return AXR_ERROR;
    }

    // We don't necessarily need a registry handle. The global assets definitely won't have one.
    if (m_EcsRegistryHandle == nullptr) {
        return AXR_SUCCESS;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    if (!isThisGlobalSceneData()) {
        axrResult = createUIMaterialsForRendering();
        if (AXR_FAILED(axrResult)) {
            axrLogErrorLocation("Failed to create UI materials for rendering");
        }
    }

    for (const auto [entity, transformComponent, modelComponent] :
         m_EcsRegistryHandle->view<AxrTransformComponent, AxrModelComponent>().each()) {
        axrResult = addMaterialForRendering(transformComponent, modelComponent);

        if (AXR_FAILED(axrResult)) {
            continue;
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyAllMaterialsForRendering() {
    // Materials for rendering only contain references to objects so we don't need to explicitly clean up any objects
    m_OpaqueMaterialsForRendering.clear();
    m_AlphaBlendMaterialsForRendering.clear();
    m_OITMaterialsForRendering.clear();
    destroyUIMaterialsForRendering();
}

AxrResult AxrVulkanSceneData::createUIMaterialsForRendering() {
    AxrResult axrResult = AXR_SUCCESS;

    const AxrVulkanModelData* foundModelData = findModelData_shared(
        axrEngineAssetGetModelName(AXR_ENGINE_ASSET_MODEL_UI_RECTANGLE)
    );
    if (foundModelData == nullptr) {
        axrLogErrorLocation("Failed to find UI Rectangle model asset.");
        return AXR_ERROR;
    }

    // ---- Rectangle UI ----

    const AxrVulkanMaterialData* foundMaterialData = findMaterialData_shared(
        axrEngineAssetGetMaterialName(AXR_ENGINE_ASSET_MATERIAL_UI_RECTANGLE)
    );
    if (foundMaterialData == nullptr) {
        axrLogErrorLocation("Failed to find UI Rectangle material asset.");
    } else {
        AxrVulkanMaterialForRendering materialForRendering;
        axrResult = buildUIMaterialForRendering(foundMaterialData, foundModelData, materialForRendering);
        if (AXR_SUCCEEDED(axrResult)) {
            m_UIRectangleMaterialForRenderingIndex = static_cast<int32_t>(m_UIMaterialsForRendering.size());
            m_UIMaterialsForRendering.push_back(std::move(materialForRendering));
        }
    }

    // ---- Border UI ----

    foundMaterialData = findMaterialData_shared(
        axrEngineAssetGetMaterialName(AXR_ENGINE_ASSET_MATERIAL_UI_BORDER)
    );
    if (foundMaterialData == nullptr) {
        axrLogErrorLocation("Failed to find UI Border material asset.");
    } else {
        AxrVulkanMaterialForRendering materialForRendering;
        axrResult = buildUIMaterialForRendering(foundMaterialData, foundModelData, materialForRendering);
        if (AXR_SUCCEEDED(axrResult)) {
            m_UIBorderMaterialForRenderingIndex = static_cast<int32_t>(m_UIMaterialsForRendering.size());
            m_UIMaterialsForRendering.push_back(std::move(materialForRendering));
        }
    }

    // ---- Image UI ----

        foundMaterialData = findMaterialData_shared(
            std::string(axrEngineAssetGetMaterialName(AXR_ENGINE_ASSET_MATERIAL_UI_IMAGE)) + std::to_string(i)
        );
        if (foundMaterialData == nullptr) {
            axrLogErrorLocation("Failed to find UI Image material asset.");
        } else {
            AxrVulkanMaterialForRendering materialForRendering;
            axrResult = buildUIMaterialForRendering(foundMaterialData, foundModelData, materialForRendering);
            if (AXR_SUCCEEDED(axrResult)) {
                m_UIImageMaterialForRenderingIndices.push_back(static_cast<int32_t>(m_UIMaterialsForRendering.size()));
        for (int i = 0; i < m_UIImageResourceCount; ++i) {
                m_UIMaterialsForRendering.push_back(std::move(materialForRendering));
            }
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::destroyUIMaterialsForRendering() {
    m_UIMaterialsForRendering.clear();

    m_UIRectangleMaterialForRenderingIndex = -1;
    m_UIBorderMaterialForRenderingIndex = -1;
    m_UIImageMaterialForRenderingIndices.clear();
}

AxrResult AxrVulkanSceneData::addMaterialForRendering(
    const AxrTransformComponent& transformComponent,
    const AxrModelComponent& modelComponent
) {
    const AxrVulkanModelData* foundModelData = findModelData_shared(modelComponent.ModelName);
    if (foundModelData == nullptr) {
        axrLogErrorLocation("Failed to find model data for model: {0}.", modelComponent.ModelName);
        return AXR_ERROR;
    }

    for (size_t meshIndex = 0; meshIndex < modelComponent.Meshes.size(); ++meshIndex) {
        for (size_t submeshIndex = 0;
             submeshIndex < modelComponent.Meshes[meshIndex].Submeshes.size();
             ++submeshIndex) {
            const AxrModelComponent::Submesh& currentSubmesh = modelComponent.Meshes[meshIndex].Submeshes[submeshIndex];

            std::string currentMaterialName = currentSubmesh.MaterialName;
            const AxrVulkanMaterialData* foundMaterialData = findMaterialData_shared(currentMaterialName);
            if (foundMaterialData == nullptr) {
                axrLogErrorLocation("Failed to find material data for material: {0}.", currentMaterialName.c_str());
                continue;
            }

            std::vector<AxrVulkanMaterialForRendering>* materialsForRendering = nullptr;
            const AxrMaterial* materialHandle = foundMaterialData->getMaterial();
            if (materialHandle == nullptr) {
                axrLogErrorLocation("Material handle is null.");
                continue;
            }

            switch (materialHandle->getAlphaRenderMode()) {
                case AXR_MATERIAL_ALPHA_RENDER_MODE_OPAQUE: {
                    materialsForRendering = &m_OpaqueMaterialsForRendering;
                    break;
                }
                case AXR_MATERIAL_ALPHA_RENDER_MODE_ALPHA_BLEND: {
                    materialsForRendering = &m_AlphaBlendMaterialsForRendering;
                    break;
                }
                case AXR_MATERIAL_ALPHA_RENDER_MODE_OIT: {
                    materialsForRendering = &m_OITMaterialsForRendering;
                    break;
                }
                default: {
                    axrLogWarningLocation(
                        "Unknown alpha render mode: {0}.",
                        static_cast<int32_t>(materialHandle->getAlphaRenderMode())
                    );
                    materialsForRendering = &m_OpaqueMaterialsForRendering;
                }
            }

            const vk::ShaderStageFlags& pushConstantStageFlags = foundMaterialData->getMaterialLayoutData()
                ->getPushConstantShaderStages();

            auto meshForRendering = AxrVulkanMeshForRendering{
                .Buffer = &foundModelData->getModelBuffer(),
                .BufferIndicesOffset = &foundModelData->getSubmeshBufferIndicesOffset(meshIndex, submeshIndex),
                .BufferVerticesOffset = &foundModelData->getSubmeshBufferVerticesOffset(meshIndex, submeshIndex),
                .IndexCount = &foundModelData->getSubmeshIndexCount(meshIndex, submeshIndex),
                .TransformComponent = &transformComponent,
                .PushConstantShaderStages = pushConstantStageFlags,
                .PushConstantBufferName = modelComponent.PushConstantBufferName,
            };

            AxrVulkanMaterialForRendering* foundMaterialForRendering = findMaterialForRendering(
                *materialsForRendering,
                currentMaterialName
            );
            if (foundMaterialForRendering != nullptr) {
                foundMaterialForRendering->Meshes.push_back(meshForRendering);
            } else {
                /// Key is the binding, Value is the instance size
                std::map<uint32_t, uint64_t> dynamicUniformBufferInstanceSizes;

                const std::vector<AxrShaderUniformBufferLinkConst_T>& uniformBufferLinks =
                    materialHandle->getUniformBufferLinks();

                for (auto& uniformBufferLink : uniformBufferLinks) {
                    if (uniformBufferLink == nullptr) continue;

                    // We don't care about the view index since we only want to know the buffer type and instance size.
                    // Which wouldn't change between views.
                    // NOTE: We might care about the platform though. Since without it, we can't find platform specific uniform buffers.
                    //  We can't just use window or XrSession blindingly though. since it wouldn't be defined if that platform isn't set up.
                    const AxrVulkanUniformBufferData* foundUniformBufferData = findUniformBufferData_shared(
                        uniformBufferLink->BufferName,
                        AXR_PLATFORM_TYPE_UNDEFINED,
                        0
                    );
                    if (foundUniformBufferData == nullptr) {
                        continue;
                    }

                    if (foundUniformBufferData->getBufferType() == AXR_UNIFORM_BUFFER_TYPE_DYNAMIC) {
                        dynamicUniformBufferInstanceSizes.insert(
                            std::pair(uniformBufferLink->Binding, foundUniformBufferData->getInstanceSize())
                        );
                    }
                }

                std::vector<AxrDynamicUniformBufferOffsetConfig> dynamicOffsetConfigs =
                    foundMaterialData->getDynamicUniformBufferOffsets();

                std::ranges::sort(
                    dynamicOffsetConfigs,
                    [](const AxrDynamicUniformBufferOffsetConfig& a, const AxrDynamicUniformBufferOffsetConfig& b) {
                        return a.Binding < b.Binding;
                    }
                );

                // Use the dynamicUniformBufferInstanceSizes size so that even if we're missing a binding offset value,
                // it still initializes to 0 and vkCmdBindDescriptorSets doesn't throw an error.
                std::vector<uint32_t> dynamicOffsets(dynamicUniformBufferInstanceSizes.size(), 0);
                uint32_t currentDynamicOffsetIndex = 0;

                for (const AxrDynamicUniformBufferOffsetConfig& dynamicOffsetConfig : dynamicOffsetConfigs) {
                    auto foundDynamicOffsetInstanceSize = dynamicUniformBufferInstanceSizes.find(
                        dynamicOffsetConfig.Binding
                    );
                    if (foundDynamicOffsetInstanceSize == dynamicUniformBufferInstanceSizes.end()) {
                        axrLogErrorLocation(
                            "Failed to find dynamic uniform buffer at binding: {0}.",
                            dynamicOffsetConfig.Binding
                        );
                        continue;
                    }

                    dynamicOffsets[currentDynamicOffsetIndex] = dynamicOffsetConfig.OffsetIndex *
                        sizeof(foundDynamicOffsetInstanceSize->second);
                    dynamicUniformBufferInstanceSizes.erase(foundDynamicOffsetInstanceSize);

                    currentDynamicOffsetIndex++;
                }

                if (!dynamicUniformBufferInstanceSizes.empty()) {
                    axrLogErrorLocation("Missing dynamic uniform buffer offsets.");
                }

                materialsForRendering->emplace_back(
                    AxrVulkanMaterialForRendering{
                        .MaterialName = currentMaterialName,
                        .PipelineLayout = &foundMaterialData->getMaterialLayoutData()->getPipelineLayout(),
                        .WindowPipeline = &foundMaterialData->getWindowPipeline(),
                        .XrSessionPipeline = &foundMaterialData->getXrSessionPipeline(),
                        .WindowDescriptorSets = &foundMaterialData->getDescriptorSets(AXR_PLATFORM_TYPE_WINDOW),
                        .XrSessionDescriptorSets = &foundMaterialData->getDescriptorSets(
                            AXR_PLATFORM_TYPE_XR_DEVICE
                        ),
                        .Meshes = {
                            meshForRendering,
                        },
                        .DynamicOffsets = dynamicOffsets
                    }
                );
            }
        }
    }

    return AXR_SUCCESS;
}

AxrResult AxrVulkanSceneData::buildUIMaterialForRendering(
    const AxrVulkanMaterialData* materialData,
    const AxrVulkanModelData* modelData,
    AxrVulkanMaterialForRendering& materialForRendering
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    // Transform component can be null

    if (materialData == nullptr) {
        axrLogErrorLocation("Material data is null.");
        return AXR_ERROR;
    }

    if (modelData == nullptr) {
        axrLogErrorLocation("Model data is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    std::vector<AxrVulkanMeshForRendering> meshesForRendering;

    for (uint32_t meshIndex = 0; meshIndex < modelData->getMeshCount(); ++meshIndex) {
        for (int submeshIndex = 0; submeshIndex < modelData->getSubmeshCount(meshIndex); ++submeshIndex) {
            meshesForRendering.emplace_back(
                AxrVulkanMeshForRendering{
                    .Buffer = &modelData->getModelBuffer(),
                    .BufferIndicesOffset = &modelData->getSubmeshBufferIndicesOffset(meshIndex, submeshIndex),
                    .BufferVerticesOffset = &modelData->getSubmeshBufferVerticesOffset(meshIndex, submeshIndex),
                    .IndexCount = &modelData->getSubmeshIndexCount(meshIndex, submeshIndex),
                    .TransformComponent = nullptr,
                    .PushConstantShaderStages = vk::ShaderStageFlagBits::eVertex,
                    .PushConstantBufferName = axrEngineAssetGetPushConstantBufferName(
                        AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MODEL_MATRIX
                    ),
                }
            );
        }
    }

    materialForRendering = AxrVulkanMaterialForRendering{
        .MaterialName = materialData->getName(),
        .PipelineLayout = &materialData->getMaterialLayoutData()->getPipelineLayout(),
        .WindowPipeline = &materialData->getWindowPipeline(),
        .XrSessionPipeline = &materialData->getXrSessionPipeline(),
        .WindowDescriptorSets = &materialData->getDescriptorSets(AXR_PLATFORM_TYPE_WINDOW),
        .XrSessionDescriptorSets = &materialData->getDescriptorSets(AXR_PLATFORM_TYPE_XR_DEVICE),
        .Meshes = meshesForRendering,
        .DynamicOffsets = {},
    };

    return AXR_SUCCESS;
}

void AxrVulkanSceneData::onNewRenderableEntityCallback(entt::registry& registry, const entt::entity entity) {
    auto [transformComponent, modelComponent] = registry.try_get<AxrTransformComponent, AxrModelComponent>(entity);
    if (transformComponent == nullptr || modelComponent == nullptr) {
        return;
    }

    if (AXR_FAILED(addMaterialForRendering(*transformComponent, *modelComponent))) {
        return;
    };
}

AxrVulkanMaterialForRendering* AxrVulkanSceneData::findMaterialForRendering(
    std::vector<AxrVulkanMaterialForRendering>& materials,
    const std::string& materialName
) const {
    for (AxrVulkanMaterialForRendering& material : materials) {
        if (material.MaterialName == materialName) {
            return &material;
        }
    }

    return nullptr;
}

#endif
