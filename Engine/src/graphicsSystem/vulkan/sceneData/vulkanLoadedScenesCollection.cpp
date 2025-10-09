#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "vulkanLoadedScenesCollection.hpp"

#include "../../../scene/scene.hpp"
#include "axr/logger.h"

// ---- Special Functions ----

AxrVulkanLoadedScenesCollection::AxrVulkanLoadedScenesCollection():
    m_PhysicalDevice(VK_NULL_HANDLE),
    m_Device(VK_NULL_HANDLE),
    m_GraphicsCommandPool(VK_NULL_HANDLE),
    m_GraphicsQueue(VK_NULL_HANDLE),
    m_TransferCommandPool(VK_NULL_HANDLE),
    m_TransferQueue(VK_NULL_HANDLE),
    m_MaxFramesInFlight(0),
    m_MaxSamplerAnisotropy(0.0f),
    m_Dispatch(nullptr),
    m_IsSetup(false),
    m_ActiveScene(std::pair(nullptr, nullptr)) {
}

AxrVulkanLoadedScenesCollection::~AxrVulkanLoadedScenesCollection() {
    resetSetup();
}

// ---- Public Functions ----

bool AxrVulkanLoadedScenesCollection::isSetup() const {
    return m_IsSetup;
}

AxrResult AxrVulkanLoadedScenesCollection::setup(const SetupConfig& config) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_PhysicalDevice != VK_NULL_HANDLE) {
        axrLogErrorLocation("Physical device isn't null.");
        return AXR_ERROR;
    }

    if (config.PhysicalDevice == VK_NULL_HANDLE) {
        axrLogErrorLocation("config.PhysicalDevice is null.");
        return AXR_ERROR;
    }

    if (m_Device != VK_NULL_HANDLE) {
        axrLogErrorLocation("Device isn't null.");
        return AXR_ERROR;
    }

    if (config.Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("config.Device is null.");
        return AXR_ERROR;
    }

    if (m_GraphicsCommandPool != VK_NULL_HANDLE) {
        axrLogErrorLocation("Graphics command pool isn't null.");
        return AXR_ERROR;
    }

    if (config.GraphicsCommandPool == VK_NULL_HANDLE) {
        axrLogErrorLocation("config.GraphicsCommandPool is null.");
        return AXR_ERROR;
    }

    if (m_GraphicsQueue != VK_NULL_HANDLE) {
        axrLogErrorLocation("Graphics queue isn't null.");
        return AXR_ERROR;
    }

    if (config.GraphicsQueue == VK_NULL_HANDLE) {
        axrLogErrorLocation("config.GraphicsQueue is null.");
        return AXR_ERROR;
    }

    if (m_TransferCommandPool != VK_NULL_HANDLE) {
        axrLogErrorLocation("Transfer command pool isn't null.");
        return AXR_ERROR;
    }

    if (config.TransferCommandPool == VK_NULL_HANDLE) {
        axrLogErrorLocation("config.TransferCommandPool is null.");
        return AXR_ERROR;
    }

    if (m_TransferQueue != VK_NULL_HANDLE) {
        axrLogErrorLocation("Transfer queue isn't null.");
        return AXR_ERROR;
    }

    if (config.TransferQueue == VK_NULL_HANDLE) {
        axrLogErrorLocation("config.TransferQueue is null.");
        return AXR_ERROR;
    }

    if (m_Dispatch != VK_NULL_HANDLE) {
        axrLogErrorLocation("Dispatch isn't null.");
        return AXR_ERROR;
    }

    if (config.Dispatch == VK_NULL_HANDLE) {
        axrLogErrorLocation("config.Dispatch is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    m_PhysicalDevice = config.PhysicalDevice;
    m_Device = config.Device;
    m_GraphicsCommandPool = config.GraphicsCommandPool;
    m_GraphicsQueue = config.GraphicsQueue;
    m_TransferCommandPool = config.TransferCommandPool;
    m_TransferQueue = config.TransferQueue;
    m_MaxFramesInFlight = config.MaxFramesInFlight;
    m_MaxSamplerAnisotropy = config.MaxSamplerAnisotropy;
    m_Dispatch = config.Dispatch;

    m_IsSetup = true;

    return AXR_SUCCESS;
}

void AxrVulkanLoadedScenesCollection::resetSetup() {
    clear();
    resetSetupWindowData();
    resetSetupXrSessionData();

    m_IsSetup = false;

    m_PhysicalDevice = VK_NULL_HANDLE;
    m_Device = VK_NULL_HANDLE;
    m_GraphicsCommandPool = VK_NULL_HANDLE;
    m_GraphicsQueue = VK_NULL_HANDLE;
    m_TransferCommandPool = VK_NULL_HANDLE;
    m_TransferQueue = VK_NULL_HANDLE;
    m_MaxFramesInFlight = 0;
    m_MaxSamplerAnisotropy = 0.0f;
    m_Dispatch = nullptr;
}

AxrVulkanSceneData* AxrVulkanLoadedScenesCollection::getGlobalSceneData() const {
    if (m_LoadedScenes.empty()) {
        return nullptr;
    }

    // The global scene data should always be the first loaded scene
    return m_LoadedScenes[0].second;
}

AxrResult AxrVulkanLoadedScenesCollection::loadGlobalSceneData(const AxrAssetCollection_T assetCollection) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    AxrVulkanSceneData* globalSceneData = getGlobalSceneData();
    if (globalSceneData != nullptr) {
        axrLogErrorLocation("Global scene data already exists.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    AxrVulkanSceneData* sceneData = createSceneData(
        m_GlobalSceneName,
        // No ui images get stored in the global scene data
        0,
        assetCollection,
        nullptr,
        globalSceneData
    );
    m_LoadedScenes.push_back(std::pair(nullptr, sceneData));

    axrResult = sceneData->loadScene();
    if (AXR_FAILED(axrResult)) {
        unloadScene(sceneData->getSceneName());
        return axrResult;
    }

    if (isWindowReady()) {
        axrResult = sceneData->loadWindowData(m_LoadWindowDataConfig);
        if (AXR_FAILED(axrResult)) {
            unloadScene(sceneData->getSceneName());
            return axrResult;
        }
    }

    if (isXrSessionReady()) {
        axrResult = sceneData->loadXrSessionData(m_LoadXrSessionDataConfig);
        if (AXR_FAILED(axrResult)) {
            unloadScene(sceneData->getSceneName());
            return axrResult;
        }
    }

    return AXR_SUCCESS;
}

AxrResult AxrVulkanLoadedScenesCollection::loadScene(const AxrScene_T scene) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (scene == nullptr) {
        axrLogErrorLocation("Scene is null.");
        return AXR_ERROR;
    }

    const std::string& sceneName = scene->getName();
    const AxrAssetCollection_T assetCollection = scene->getAssetCollection();
    entt::registry* ecsRegistryHandle = scene->getEcsRegistry();

    AxrVulkanSceneData* globalSceneData = getGlobalSceneData();
    if (globalSceneData == nullptr) {
        axrLogErrorLocation("Global scene data is null. Load global scene data first.");
        return AXR_ERROR;
    }

    for (const AxrVulkanSceneData* sceneData : m_LoadedScenes | std::views::values) {
        if (sceneData->getSceneName() == sceneName) {
            axrLogErrorLocation("Scene named \"{0}\" has already been loaded", sceneName.c_str());
            return AXR_ERROR;
        }
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    AxrVulkanSceneData* sceneData = createSceneData(
        sceneName,
        scene->getUIImageResourcesPreloadCount(),
        assetCollection,
        ecsRegistryHandle,
        globalSceneData
    );
    m_LoadedScenes.emplace_back(scene, sceneData);

    axrResult = sceneData->loadScene();
    if (AXR_FAILED(axrResult)) {
        unloadScene(sceneData->getSceneName());
        return axrResult;
    }

    if (isWindowReady()) {
        axrResult = sceneData->loadWindowData(m_LoadWindowDataConfig);
        if (AXR_FAILED(axrResult)) {
            unloadScene(sceneData->getSceneName());
            return axrResult;
        }
    }

    if (isXrSessionReady()) {
        axrResult = sceneData->loadXrSessionData(m_LoadXrSessionDataConfig);
        if (AXR_FAILED(axrResult)) {
            unloadScene(sceneData->getSceneName());
            return axrResult;
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanLoadedScenesCollection::unloadScene(const std::string& sceneName) {
    if (m_ActiveScene.second != nullptr && m_ActiveScene.second->getSceneName() == sceneName) {
        m_ActiveScene.first = nullptr;
        m_ActiveScene.second = nullptr;
    }

    const auto foundScene = findLoadedSceneIterator(sceneName);
    if (foundScene == m_LoadedScenes.end()) {
        axrLogErrorLocation("Scene named: {0} not found.", sceneName.c_str());
        return;
    }

    foundScene->second->unloadWindowData();
    foundScene->second->unloadXrSessionData();
    foundScene->second->unloadScene();
    destroySceneData(foundScene->second);

    m_LoadedScenes.erase(foundScene);
}

void AxrVulkanLoadedScenesCollection::clear() {
    for (AxrVulkanSceneData* sceneData : m_LoadedScenes | std::views::values) {
        sceneData->unloadWindowData();
        sceneData->unloadXrSessionData();
        sceneData->unloadScene();
        destroySceneData(sceneData);
    }

    m_LoadedScenes.clear();
    m_ActiveScene.first = nullptr;
    m_ActiveScene.second = nullptr;
}

AxrVulkanSceneData* AxrVulkanLoadedScenesCollection::findLoadedScene(const std::string& sceneName) {
    const auto foundScene = findLoadedSceneIterator(sceneName);
    if (foundScene == m_LoadedScenes.end()) {
        return nullptr;
    }

    return foundScene->second;
}

AxrResult AxrVulkanLoadedScenesCollection::setActiveScene(const std::string& sceneName) {
    const auto foundScene = findLoadedSceneIterator(sceneName);
    if (foundScene == m_LoadedScenes.end()) {
        return AXR_ERROR;
    }

    m_ActiveScene = *foundScene;
    return AXR_SUCCESS;
}

AxrVulkanSceneData* AxrVulkanLoadedScenesCollection::getActiveSceneData() const {
    return m_ActiveScene.second;
}

AxrScene_T AxrVulkanLoadedScenesCollection::getActiveScene() const {
    return m_ActiveScene.first;
}

AxrResult AxrVulkanLoadedScenesCollection::setupWindowData(
    const vk::RenderPass renderPass,
    const vk::SampleCountFlagBits msaaSampleCount
) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (isWindowReady()) {
        axrLogErrorLocation("Window data is already set up.");
        return AXR_ERROR;
    }

    if (renderPass == VK_NULL_HANDLE) {
        axrLogErrorLocation("renderPass is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    // If anything new gets added here, make sure it also gets added to the isWindowReady() function
    m_LoadWindowDataConfig = AxrVulkanSceneData::LoadWindowDataConfig{
        .RenderPass = renderPass,
        .MsaaSampleCount = msaaSampleCount,
    };

    return loadAllWindowSceneData();
}

void AxrVulkanLoadedScenesCollection::resetSetupWindowData() {
    m_LoadWindowDataConfig = {};

    unloadAllWindowSceneData();
}

AxrResult AxrVulkanLoadedScenesCollection::setupXrSessionData(
    const vk::RenderPass renderPass,
    const vk::SampleCountFlagBits msaaSampleCount,
    const uint32_t viewCount
) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (isXrSessionReady()) {
        axrLogErrorLocation("Xr session data is already set up.");
        return AXR_ERROR;
    }

    if (renderPass == VK_NULL_HANDLE) {
        axrLogErrorLocation("renderPass is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    // If anything new gets added here, make sure it also gets added to the isXrSessionReady() function
    m_LoadXrSessionDataConfig = AxrVulkanSceneData::LoadXrSessionDataConfig{
        .RenderPass = renderPass,
        .MsaaSampleCount = msaaSampleCount,
        .ViewCount = viewCount
    };

    return loadAllXrSessionSceneData();
}

void AxrVulkanLoadedScenesCollection::resetSetupXrSessionData() {
    m_LoadXrSessionDataConfig = {};

    unloadAllXrSessionSceneData();
}

// ---- Private Functions ----

std::vector<std::pair<AxrScene_T, AxrVulkanSceneData*>>::iterator
AxrVulkanLoadedScenesCollection::findLoadedSceneIterator(
    const std::string& sceneName
) {
    for (auto i = m_LoadedScenes.begin(); i != m_LoadedScenes.end(); ++i) {
        if (i->second->getSceneName() == sceneName) {
            return i;
        }
    }

    return m_LoadedScenes.end();
}

AxrVulkanSceneData* AxrVulkanLoadedScenesCollection::createSceneData(
    const std::string& sceneName,
    const uint32_t uiImageResourcesPreloadCount,
    const AxrAssetCollection_T assetCollection,
    entt::registry* ecsRegistryHandle,
    AxrVulkanSceneData* globalSceneData
) const {
    return new AxrVulkanSceneData(
        AxrVulkanSceneData::Config{
            .SceneName = sceneName,
            .UIImageResourcesPreloadCount = uiImageResourcesPreloadCount,
            .AssetCollection = assetCollection,
            .EcsRegistryHandle = ecsRegistryHandle,
            .GlobalSceneData = globalSceneData,
            .PhysicalDevice = m_PhysicalDevice,
            .Device = m_Device,
            .GraphicsCommandPool = m_GraphicsCommandPool,
            .GraphicsQueue = m_GraphicsQueue,
            .TransferCommandPool = m_TransferCommandPool,
            .TransferQueue = m_TransferQueue,
            .MaxFramesInFlight = m_MaxFramesInFlight,
            .MaxSamplerAnisotropy = m_MaxSamplerAnisotropy,
            .DispatchHandle = m_Dispatch
        }
    );
}

void AxrVulkanLoadedScenesCollection::destroySceneData(AxrVulkanSceneData*& sceneData) const {
    if (sceneData == nullptr) return;

    delete sceneData;
    sceneData = nullptr;
}

bool AxrVulkanLoadedScenesCollection::isWindowReady() const {
    return m_LoadWindowDataConfig.RenderPass != VK_NULL_HANDLE;
}

AxrResult AxrVulkanLoadedScenesCollection::loadAllWindowSceneData() const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!isWindowReady()) {
        axrLogErrorLocation("Window data is not ready.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    for (AxrVulkanSceneData* scene : m_LoadedScenes | std::views::values) {
        axrResult = scene->loadWindowData(m_LoadWindowDataConfig);
        if (AXR_FAILED(axrResult)) {
            break;
        }
    }

    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to load all window scene data.");
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanLoadedScenesCollection::unloadAllWindowSceneData() const {
    for (AxrVulkanSceneData* scene : m_LoadedScenes | std::views::values) {
        scene->unloadWindowData();
    }
}

bool AxrVulkanLoadedScenesCollection::isXrSessionReady() const {
    return m_LoadXrSessionDataConfig.RenderPass != VK_NULL_HANDLE &&
        m_LoadXrSessionDataConfig.ViewCount > 0;
}

AxrResult AxrVulkanLoadedScenesCollection::loadAllXrSessionSceneData() const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!isXrSessionReady()) {
        axrLogErrorLocation("Xr session data is not ready.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    for (AxrVulkanSceneData* scene : m_LoadedScenes | std::views::values) {
        axrResult = scene->loadXrSessionData(m_LoadXrSessionDataConfig);
        if (AXR_FAILED(axrResult)) {
            break;
        }
    }

    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to load all xr session scene data.");
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanLoadedScenesCollection::unloadAllXrSessionSceneData() const {
    for (AxrVulkanSceneData* scene : m_LoadedScenes | std::views::values) {
        scene->unloadXrSessionData();
    }
}

#endif
