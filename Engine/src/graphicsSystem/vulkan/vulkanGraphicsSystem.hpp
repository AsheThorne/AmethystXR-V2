#pragma once
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/common/enums.h"
#include "axr/windowSystem.h"
#include "axr/assets.h"
#include "vulkanQueueFamilies.hpp"
#include "vulkanWindowGraphics.hpp"
#include "vulkanXrGraphics.hpp"
#include "../../extensionCollection.hpp"
#include "sceneData/vulkanLoadedScenesCollection.hpp"
#include "vulkanRenderCommands.hpp"

// ----------------------------------------- //
// Vulkan Headers
// ----------------------------------------- //
#include <vulkan/vulkan.hpp>

/// Vulkan Graphics System
class AxrVulkanGraphicsSystem {
public:
    // ----------------------------------------- //
    // Types
    // ----------------------------------------- //

    /// The instance pNext structure chain type
    using InstanceChain_T = vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT>;
    /// The device pNext structure chain type
    using DeviceChain_T = vk::StructureChain<vk::DeviceCreateInfo>;

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Vulkan Graphics System Config
    struct WindowConfig {
        AxrVulkanPresentationModeEnum PresentationMode;
        AxrMsaaSampleCountEnum MaxMsaaSampleCount;
    };

    /// Vulkan Graphics Xr Session Config
    struct XrSessionConfig {
        AxrMsaaSampleCountEnum MaxMsaaSampleCount;
    };

    /// Vulkan Graphics System Config
    struct Config {
        std::string ApplicationName;
        uint32_t ApplicationVersion;
        AxrWindowSystem_T WindowSystem;
        AxrXrSystem_T XrSystem;
        AxrAssetCollection_T GlobalAssetCollection;
        WindowConfig* WindowConfig;
        XrSessionConfig* XrSessionConfig;
        AxrSamplerAnisotropyQualityEnum SamplerAnisotropyQuality;
        uint32_t ApiLayerCount;
        AxrVulkanApiLayer_T* ApiLayers;
        uint32_t ExtensionCount;
        AxrVulkanExtension_T* Extensions;
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Constructor
    /// @param config Vulkan graphics system config
    AxrVulkanGraphicsSystem(const Config& config);
    /// Copy Constructor
    /// @param src Source AxrVulkanGraphicsSystem to copy from
    AxrVulkanGraphicsSystem(const AxrVulkanGraphicsSystem& src) = delete;
    /// Move Constructor
    /// @param src Source AxrVulkanGraphicsSystem to move from
    AxrVulkanGraphicsSystem(AxrVulkanGraphicsSystem&& src) noexcept = delete;

    // ---- Destructor ----

    /// Destructor
    ~AxrVulkanGraphicsSystem();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrVulkanGraphicsSystem to copy from
    AxrVulkanGraphicsSystem& operator=(const AxrVulkanGraphicsSystem& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrVulkanGraphicsSystem to move from
    AxrVulkanGraphicsSystem& operator=(AxrVulkanGraphicsSystem&& src) noexcept = delete;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Set up the vulkan graphics system
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setup();

    /// Draw the current frame
    void drawFrame() const;

    /// Set the clear color
    /// @param color Clear color
    void setClearColor(const AxrColor& color) const;

    /// Set the window render source
    /// @param renderSource Window render source
    void setWindowRenderSource(AxrWindowRenderSourceEnum renderSource) const;
    /// Get the window render source
    /// @returns The renderSource Window render source
    [[nodiscard]] AxrWindowRenderSourceEnum getWindowRenderSource() const;

    /// Load the named scene
    /// @param scene Scene to load
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadScene(AxrScene_T scene);

    /// Set the active scene to the named scene
    /// @param sceneName Name of the scene
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setActiveScene(const std::string& sceneName);

private:
    // ----------------------------------------- //
    // Private Structs
    // ----------------------------------------- //

    /// Mesh reference used for rendering transparent objects that need sorting
    struct SortableMeshReference {
        uint64_t SortKey;
        uint32_t MaterialIndex;
        uint32_t MeshIndex;
    };

    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    std::string m_ApplicationName;
    uint32_t m_ApplicationVersion;
    AxrAssetCollection_T m_GlobalAssetCollection;
    AxrExtensionCollection<AxrVulkanApiLayer_T, AxrVulkanApiLayerTypeEnum> m_ApiLayers;
    AxrExtensionCollection<AxrVulkanExtension_T, AxrVulkanExtensionTypeEnum> m_Extensions;
    AxrSamplerAnisotropyQualityEnum m_SamplerAnisotropyQuality;

    /// Ordered from most desired to the least desired
    std::vector<vk::SurfaceFormatKHR> m_SwapchainColorFormatOptions;
    /// Ordered from most desired to the least desired
    std::vector<vk::Format> m_SwapchainDepthFormatOptions;

    vk::DispatchLoaderDynamic m_Dispatch;
    vk::Instance m_Instance;
    vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger;
    vk::PhysicalDevice m_PhysicalDevice;
    AxrVulkanQueueFamilies m_QueueFamilies;
    vk::Device m_Device;
    vk::CommandPool m_GraphicsCommandPool;
    vk::CommandPool m_TransferCommandPool;
    uint32_t m_MaxFramesInFlight;
    Clay_Context* m_ClayContext;
    Clay_Arena m_ClayArena;

    AxrVulkanLoadedScenesCollection m_LoadedScenes;
    AxrVulkanWindowGraphics* m_WindowGraphics;
    AxrVulkanXrGraphics* m_XrGraphics;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Reset setup()
    void resetSetup();

    // ---- Instance Functions ----

    /// Create the vulkan instance
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createInstance();
    /// Destroy the vulkan instance
    void destroyInstance();

    /// Create the instance pNext chain using the given instance create info
    /// @param instanceCreateInfo Instance create info to use
    /// @returns The instance structure chain
    [[nodiscard]] InstanceChain_T createInstanceChain(const vk::InstanceCreateInfo& instanceCreateInfo) const;

    // ---- Api Layers / Extensions ----

    /// Find the supported instance api layers
    /// @returns The supported instance api layers
    std::vector<std::string> getSupportedInstanceApiLayers() const;
    /// Find the supported device api layers for the given physical device
    /// @returns The supported device api layers
    std::vector<std::string> getSupportedDeviceApiLayers(const vk::PhysicalDevice& physicalDevice) const;
    /// Find the supported instance extensions
    /// @returns The supported instance extensions
    std::vector<std::string> getSupportedInstanceExtensions() const;
    /// Find the supported device extensions for the given physical device
    /// @returns The supported device extensions
    std::vector<std::string> getSupportedDeviceExtensions(const vk::PhysicalDevice& physicalDevice) const;

    /// Remove api layers from m_ApiLayers that aren't supported by the m_Instance
    void removeUnsupportedApiLayers();
    /// Remove instance level extensions from m_Extensions that aren't supported by the m_Instance
    /// @returns AXR_SUCCESS if we can continue.
    /// @returns AXR_ERROR if there was a required extension that wasn't supported.
    [[nodiscard]] AxrResult removeUnsupportedInstanceExtensions();
    /// Remove device level extensions from m_Extensions that aren't supported by the m_PhysicalDevice
    /// @returns AXR_SUCCESS if we can continue.
    /// @returns AXR_ERROR if there was a required extension that wasn't supported.
    [[nodiscard]] AxrResult removeUnsupportedDeviceExtensions();

    /// Get a collection of all api layer names to use
    /// @returns A collection of api layer names
    [[nodiscard]] std::vector<const char*> getAllApiLayerNames() const;
    /// Get a collection of all instance extension names to use
    /// @returns A collection of instance extension names
    [[nodiscard]] std::vector<const char*> getAllInstanceExtensionNames() const;
    /// Get a collection of all device extension names to use
    /// @returns A collection of device extension names
    [[nodiscard]] std::vector<const char*> getAllDeviceExtensionNames() const;

    /// Add the required instance extensions to m_Extensions
    void addRequiredInstanceExtensions();
    /// Add the required device extensions to m_Extensions
    void addRequiredDeviceExtensions();

    // ---- Debug Utils ----

    /// Create the debug utils messenger create info
    /// @retrns The debug utils messenger create info
    [[nodiscard]] vk::DebugUtilsMessengerCreateInfoEXT createDebugUtilsCreateInfo() const;

    /// Create the debug utils messenger
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createDebugUtils();
    /// Destroy the debug utils messenger
    void destroyDebugUtils();

    // ---- Physical Device ----

    /// Set up the physical device
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupPhysicalDevice();
    /// Reset setupPhysicalDevice()
    void resetPhysicalDevice();

    /// Decide on which physical device we'd like to use.
    /// If OpenXR is being used though, we need to use the one it selects for us
    /// @param physicalDevice Output physical device we should use
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult pickPhysicalDevice(vk::PhysicalDevice& physicalDevice) const;

    /// Score the given physical device. The higher the number, the better.
    /// 0 Means it is not suitable to use.
    /// @param physicalDevice Physical device to score
    /// @returns The physical device score
    uint32_t scorePhysicalDeviceSuitability(const vk::PhysicalDevice& physicalDevice) const;
    /// Score the given physical device on its available queue families.
    /// @param physicalDevice Physical device to score
    /// @returns 0 -> Not suitable at all.
    /// @returns 1 -> Meets the minimum requirements.
    /// @returns More than 1, has extra desired features.
    uint32_t scorePhysicalDeviceQueueFamilies(const vk::PhysicalDevice& physicalDevice) const;
    /// Score the given physical device on its available api layers.
    /// @details
    /// All api layers used by the instance should also be used for the device.
    /// This is for compatibility with older version of vulkan.
    /// "https://docs.vulkan.org/spec/latest/chapters/extensions.html#extendingvulkan-layers-devicelayerdeprecation"
    /// If any of the api layers aren't supported, then it's still ok for more recent versions of vulkan.
    /// So, a score of 1 indicates that not all api layers are supported, but it's not desirable.
    /// A score of more than 1 means all api layers are supported.
    /// @param physicalDevice Physical device to score
    /// @returns 0 -> Not suitable at all.
    /// @returns 1 -> Meets the minimum requirements.
    /// @returns More than 1, supports all api layers
    uint32_t scorePhysicalDeviceApiLayers(const vk::PhysicalDevice& physicalDevice) const;
    /// Score the given physical device on its available extensions.
    /// @param physicalDevice Physical device to score
    /// @returns 0 -> Not suitable at all.
    /// @returns 1 -> Meets the minimum requirements.
    /// @returns More than 1, supports more than just the required extensions.
    uint32_t scorePhysicalDeviceExtensions(const vk::PhysicalDevice& physicalDevice) const;
    /// Score the given physical device on its available features.
    /// @param physicalDevice Physical device to score
    /// @returns 0 -> Not suitable at all.
    /// @returns 1 -> Meets the minimum requirements.
    /// @returns More than 1, has extra desired features.
    uint32_t scorePhysicalDeviceFeatures(const vk::PhysicalDevice& physicalDevice) const;
    /// Score the given physical device on its available properties.
    /// @param physicalDevice Physical device to score
    /// @returns 0 -> Not suitable at all.
    /// @returns 1 -> Meets the minimum requirements.
    /// @returns More than 1, has extra desired features.
    uint32_t scorePhysicalDeviceProperties(const vk::PhysicalDevice& physicalDevice) const;

    /// Check if all the api layers are supported for the given physical device
    /// @param physicalDevice Physical device to check
    /// @returns True if all the api layers are supported for the given physical device
    bool areApiLayersSupportedForPhysicalDevice(const vk::PhysicalDevice& physicalDevice) const;

    /// Get the max sampler anisotropy value from the given sampler anisotropy quality enum
    /// @param anisotropyQuality Sampler anisotropy quality
    /// @returns The max sampler anisotropy value
    float getMaxSamplerAnisotropyValue(AxrSamplerAnisotropyQualityEnum anisotropyQuality) const;

    // ---- Logical Device ----

    /// Create the vulkan device
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createLogicalDevice();
    /// Destroy the vulkan device
    void destroyLogicalDevice();

    /// Create the device pNext chain using the given device create info
    /// @param deviceCreateInfo Device create info to use
    /// @returns The device structure chain
    [[nodiscard]] DeviceChain_T createDeviceChain(const vk::DeviceCreateInfo& deviceCreateInfo) const;

    // ---- Command Pools ----

    /// Create the command pools
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createCommandPools();
    /// Destroy the command pools
    void destroyCommandPools();

    /// Create a new command pool
    /// @param queueFamilyIndex Command pool queue family index
    /// @param commandPoolFlags Command pool create flags
    /// @param commandPool Output created command pool
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createCommandPool(
        uint32_t queueFamilyIndex,
        vk::CommandPoolCreateFlags commandPoolFlags,
        vk::CommandPool& commandPool
    ) const;
    /// Destroy the given command pool
    /// @param commandPool Command pool to destroy
    void destroyCommandPool(
        vk::CommandPool& commandPool
    ) const;

    // ---- Scene Data ----

    /// Set up the scene data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupSceneData();
    /// Reset the setupSceneData() function
    void resetSetupSceneData();

    // ---- Window Related Functions ----

    /// Set up window graphics
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupWindowGraphics();
    /// Reset the setupWindowGraphics() function
    void resetSetupWindowGraphics();

    // ---- Xr Related Functions ----

    /// Set up xr graphics
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupXrGraphics();
    /// Reset the setupXrGraphics() function
    void resetSetupXrGraphics();

    // ---- Rendering ----

    /// Render the current frame using the given render target specific commands
    /// @tparam RenderTarget Render target class. Like a window or xr device
    /// @param renderTarget Render target to use
    /// @returns AXR_SUCCESS if the function succeeded
    template <typename RenderTarget>
    [[nodiscard]] AxrResult renderCurrentFrame(RenderTarget& renderTarget) const;
    /// Render the clay UI elements according to the given uiCanvasConfig
    /// @tparam RenderTarget Render target class. Like a window or xr device
    /// @param viewIndex View index to render for
    /// @param renderCommands Vulkan render commands
    /// @param sceneData Active scene data
    /// @param uiCanvasConfig UI Canvas config
    template <typename RenderTarget>
    void renderClayUI(
        uint32_t viewIndex,
        const AxrVulkanRenderCommands<RenderTarget>& renderCommands,
        AxrVulkanSceneData* sceneData,
        const AxrUICanvasConfig& uiCanvasConfig
    ) const;
    /// Blit the current frame from the xr graphics to the window graphics
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult blitToWindowFromXrDevice() const;

    /// Get a collection of each mesh in the given 'materials for rendering' sorted by depth
    /// @param viewMatrix Camera view matrix
    /// @param nearPlane Camera near plane
    /// @param farPlane Camera far plane
    /// @param materialsForRendering 'Materials for rendering' to sort
    /// @returns The sorted mesh references
    [[nodiscard]] std::vector<SortableMeshReference> getSortedMeshReferences(
        const glm::mat4& viewMatrix,
        float nearPlane,
        float farPlane,
        const std::vector<AxrVulkanMaterialForRendering>& materialsForRendering
    ) const;
    /// Calculate the squared depth of the given transform component
    /// @param viewMatrix Camera view matrix
    /// @param transformComponent Transform component to use
    /// @returns The depth
    [[nodiscard]] float calculateSquaredDepth(
        const glm::mat4& viewMatrix,
        const AxrTransformComponent* transformComponent
    ) const;
    /// Convert the given depth to a uint32_t value. Ranging from `nearPlane` to `farPlane`
    /// @param nearPlane Camera near plane
    /// @param farPlane Camera far plane
    /// @param depth Depth value
    /// @returns The given depth as a uint32_t value
    [[nodiscard]] uint32_t depthToUint(float nearPlane, float farPlane, float depth) const;
    /// Create a sort key for the given depth and material index
    /// @param depth Depth value
    /// @param materialIndex Material index
    /// @returns The sort key value
    [[nodiscard]] uint64_t createSortKey(uint32_t depth, uint32_t materialIndex) const;

    // ---- Clay ----

    /// Set up the clay data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupClay();
    /// Reset setupClay()
    void resetSetupClay();
    /// Callback function to handle clay errors
    /// @param errorData Clay error data
    void handleClayErrors(const Clay_ErrorData& errorData) const;

    // ----------------------------------------- //
    // Private Static Functions
    // ----------------------------------------- //

    /// Debug utils messages callback function
    /// @param messageSeverity The severity of the message
    /// @param messageType The type of the message
    /// @param pCallbackData Callback data
    /// @param pUserData User data
    /// @returns The application should always return VK_FALSE. VK_TRUE is typically only used in layer development
    static VkBool32 VKAPI_CALL debugUtilsCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );
};

#endif
