#pragma once
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <vector>

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "vulkanSceneData.hpp"

/// Vulkan loaded scenes collection
class AxrVulkanLoadedScenesCollection {
public:
    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// AxrVulkanLoadedScenesCollection setup config
    struct SetupConfig {
        vk::PhysicalDevice PhysicalDevice;
        vk::Device Device;
        vk::CommandPool GraphicsCommandPool;
        vk::Queue GraphicsQueue;
        vk::CommandPool TransferCommandPool;
        vk::Queue TransferQueue;
        uint32_t MaxFramesInFlight;
        float MaxSamplerAnisotropy;
        vk::DispatchLoaderDynamic* Dispatch;
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Constructor
    AxrVulkanLoadedScenesCollection();
    /// Copy Constructor
    /// @param src Source AxrVulkanLoadedScenesCollection to copy from
    AxrVulkanLoadedScenesCollection(const AxrVulkanLoadedScenesCollection& src) = delete;
    /// Move Constructor
    /// @param src Source AxrVulkanLoadedScenesCollection to move from
    AxrVulkanLoadedScenesCollection(AxrVulkanLoadedScenesCollection&& src) noexcept = delete;

    // ---- Destructor ----

    /// Destructor
    ~AxrVulkanLoadedScenesCollection();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrVulkanLoadedScenesCollection to copy from
    AxrVulkanLoadedScenesCollection& operator=(const AxrVulkanLoadedScenesCollection& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrVulkanLoadedScenesCollection to move from
    AxrVulkanLoadedScenesCollection& operator=(AxrVulkanLoadedScenesCollection&& src) noexcept = delete;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Check if the loaded scenes have been set up
    /// @returns True if the loaded scenes have been set up
    [[nodiscard]] bool isSetup() const;
    /// Set up the loaded scenes collection
    /// @param config Setup config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setup(const SetupConfig& config);
    /// Reset the setup() function
    void resetSetup();

    /// Get the global scene data
    /// @returns The global scene data. Or nullptr if it doesn't exist
    [[nodiscard]] AxrVulkanSceneData* getGlobalSceneData() const;

    /// Load the global scene
    /// @param assetCollection Asset collection to use
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadGlobalSceneData(
        AxrAssetCollection_T assetCollection
    );
    /// Load a new scene
    /// @param scene Scene
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadScene(AxrScene_T scene);
    /// Unload the named scene
    /// @param sceneName Name of the scene 
    void unloadScene(const std::string& sceneName);

    /// Unload and remove all loaded scenes
    void clear();

    /// Find the named loaded scene data
    /// @param sceneName Scene name
    /// @returns The named loaded scene data. Or nullptr if it wasn't found
    [[nodiscard]] AxrVulkanSceneData* findLoadedScene(const std::string& sceneName);

    /// Set the active scene to the named scene
    /// @param sceneName Scene name
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setActiveScene(const std::string& sceneName);
    /// Get the active scene data
    /// @returns A handle to the active scene data or nullptr if an active scene isn't set
    [[nodiscard]] AxrVulkanSceneData* getActiveSceneData() const;
    /// Get the active scene
    /// @returns A handle to the active scene or nullptr if an active scene isn't set
    [[nodiscard]] AxrScene_T getActiveScene() const;

    /// Set up the window data for all scenes and load all window specific scene data
    /// @param renderPass Render pass to use
    /// @param msaaSampleCount Msaa sample count
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupWindowData(vk::RenderPass renderPass, vk::SampleCountFlagBits msaaSampleCount);
    /// Reset the setupWindowData() function
    void resetSetupWindowData();

    /// Set up the xr session data for all scenes and load all xr session specific scene data
    /// @param renderPass Render pass to use
    /// @param msaaSampleCount Msaa sample count
    /// @param viewCount The number of views for the xr device
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupXrSessionData(
        vk::RenderPass renderPass,
        vk::SampleCountFlagBits msaaSampleCount,
        uint32_t viewCount
    );
    /// Reset the setupXrSessionData() function
    void resetSetupXrSessionData();

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Setup Config ----
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_Device;
    vk::CommandPool m_GraphicsCommandPool;
    vk::Queue m_GraphicsQueue;
    vk::CommandPool m_TransferCommandPool;
    vk::Queue m_TransferQueue;
    uint32_t m_MaxFramesInFlight;
    float m_MaxSamplerAnisotropy;
    vk::DispatchLoaderDynamic* m_Dispatch;

    bool m_IsSetup;
    std::string m_GlobalSceneName = "AXR:SceneGlobal";
    AxrVulkanSceneData::LoadWindowDataConfig m_LoadWindowDataConfig;
    AxrVulkanSceneData::LoadXrSessionDataConfig m_LoadXrSessionDataConfig;

    std::pair<AxrScene_T, AxrVulkanSceneData*> m_ActiveScene;
    std::vector<std::pair<AxrScene_T, AxrVulkanSceneData*>> m_LoadedScenes;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Find the named loaded scene data iterator
    /// @param sceneName Scene name
    /// @returns The named loaded scene data iterator. Or m_LoadedScenes.end() if it wasn't found
    [[nodiscard]] std::vector<std::pair<AxrScene_T, AxrVulkanSceneData*>>::iterator findLoadedSceneIterator(
        const std::string& sceneName
    );

    /// Create vulkan scene data
    /// @param sceneName Name of the scene
    /// @param uiImageResourcesPreloadCount Number of ui image resources to preload
    /// @param assetCollection Asset collection to use
    /// @param ecsRegistryHandle ECS registry to use
    /// @param globalSceneData Global scene data
    /// @returns A handle to the created vulkan scene data
    [[nodiscard]] AxrVulkanSceneData* createSceneData(
        const std::string& sceneName,
        uint32_t uiImageResourcesPreloadCount,
        AxrAssetCollection_T assetCollection,
        entt::registry* ecsRegistryHandle,
        AxrVulkanSceneData* globalSceneData
    ) const;
    /// Destroy the given vulkan scene data
    void destroySceneData(AxrVulkanSceneData*& sceneData) const;

    // ---- Window Scene Data ----

    /// Check if the window scene data is ready
    /// @returns True if the window scene data is ready
    [[nodiscard]] bool isWindowReady() const;

    /// Load all window specific scene data for the loaded scenes
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadAllWindowSceneData() const;
    /// Unload all window specific scene data for the loaded scenes
    void unloadAllWindowSceneData() const;

    // ---- Xr session Scene Data ----

    /// Check if the xr session scene data is ready
    /// @returns True if the xr session scene data is ready
    [[nodiscard]] bool isXrSessionReady() const;

    /// Load all xr session specific scene data for the loaded scenes
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadAllXrSessionSceneData() const;
    /// Unload all xr session specific scene data for the loaded scenes
    void unloadAllXrSessionSceneData() const;
};

#endif
