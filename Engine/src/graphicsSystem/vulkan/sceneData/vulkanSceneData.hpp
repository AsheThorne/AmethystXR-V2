#pragma once
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/assets.h"
#include "vulkanUniformBufferData.hpp"
#include "vulkanModelData.hpp"
#include "vulkanMaterialLayoutData.hpp"
#include "vulkanMaterialData.hpp"
#include "vulkanImageData.hpp"
#include "vulkanImageSamplerData.hpp"
#include "axr/scene.h"
#include "../../../xrSystem/xrUtils.hpp"
#include "../vulkanRenderStructs.hpp"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <string>
#include <unordered_map>

/// Vulkan scene data
class AxrVulkanSceneData {
public:
    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Vulkan scene data config
    struct Config {
        std::string SceneName;
        uint32_t UIImageResourcesPreloadCount;
        AxrAssetCollection_T AssetCollection;
        entt::registry* EcsRegistryHandle;
        AxrVulkanSceneData* GlobalSceneData;
        vk::PhysicalDevice PhysicalDevice;
        vk::Device Device;
        vk::CommandPool GraphicsCommandPool;
        vk::Queue GraphicsQueue;
        vk::CommandPool TransferCommandPool;
        vk::Queue TransferQueue;
        uint32_t MaxFramesInFlight;
        float MaxSamplerAnisotropy;
        vk::DispatchLoaderDynamic* DispatchHandle;
    };

    /// Load window data config
    struct LoadWindowDataConfig {
        vk::RenderPass RenderPass = VK_NULL_HANDLE;
        vk::SampleCountFlagBits MsaaSampleCount = vk::SampleCountFlagBits::e1;
    };

    /// Load xr session data config
    struct LoadXrSessionDataConfig {
        vk::RenderPass RenderPass = VK_NULL_HANDLE;
        vk::SampleCountFlagBits MsaaSampleCount = vk::SampleCountFlagBits::e1;
        uint32_t ViewCount = 0;
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Constructor
    /// @param config Vulkan scene data config
    explicit AxrVulkanSceneData(const Config& config);
    /// Copy Constructor
    /// @param src Source AxrVulkanSceneData to copy from
    AxrVulkanSceneData(const AxrVulkanSceneData& src) = delete;
    /// Move Constructor
    /// @param src Source AxrVulkanSceneData to move from
    AxrVulkanSceneData(AxrVulkanSceneData&& src) noexcept = delete;

    // ---- Destructor ----

    /// Destructor
    ~AxrVulkanSceneData();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrVulkanSceneData to copy from
    AxrVulkanSceneData& operator=(const AxrVulkanSceneData& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrVulkanSceneData to move from
    AxrVulkanSceneData& operator=(AxrVulkanSceneData&& src) noexcept = delete;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Get the scene name
    /// @returns The scene name
    [[nodiscard]] const std::string& getSceneName() const;
    /// Get the ecs registry handle
    /// @returns The ecs registry handle
    [[nodiscard]] entt::registry* getEcsRegistryHandle() const;

    /// Load the scene data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadScene();
    /// Unload the scene data
    void unloadScene();

    /// Load the window specific scene data
    /// @param config Load window data config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadWindowData(const LoadWindowDataConfig& config);
    /// Unload the window specific scene data
    void unloadWindowData();

    /// Load the xr session specific scene data
    /// @param config Load xr session data config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadXrSessionData(const LoadXrSessionDataConfig& config);
    /// Unload the xr session specific scene data
    void unloadXrSessionData();

    /// Get the materials, organized specifically for rendering
    /// @param alphaRenderMode Alpha render mode to get the materials for
    /// @returns The collection of materials for rendering
    [[nodiscard]] const std::vector<AxrVulkanMaterialForRendering>& getMaterialsForRendering(
        AxrMaterialAlphaRenderModeEnum alphaRenderMode
    ) const;
    /// Get the `UI Rectangle` material for rendering
    /// @returns The `UI Rectangle` material for rendering. Or nullptr if it doesn't exist.
    [[nodiscard]] const AxrVulkanMaterialForRendering* getUIRectangleMaterialForRendering() const;
    /// Get the `UI Border` material for rendering
    /// @returns The `UI Border` material for rendering. Or nullptr if it doesn't exist.
    [[nodiscard]] const AxrVulkanMaterialForRendering* getUIBorderMaterialForRendering() const;
    /// Get the `UI Image` material for rendering
    /// @param imageIndex Index of the image to use
    /// @returns The `UI Image` material for rendering. Or nullptr if it doesn't exist.
    [[nodiscard]] const AxrVulkanMaterialForRendering* getUIImageMaterialForRendering(uint32_t imageIndex) const;

    /// Set uniform buffer data.
    /// If `alignData` is true, `data` must contain whole instance objects without any padding between them.
    /// @param platformType Platform type
    /// @param bufferName Buffer name
    /// @param frameIndex Frame index to use
    /// @param viewIndex View index
    /// @param alignData True if we want to align it with the min uniform buffer offset alignment.
    /// @param offset Data offset
    /// @param dataSize Data size
    /// @param data Data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setUniformBufferData(
        AxrPlatformType platformType,
        const std::string& bufferName,
        uint32_t frameIndex,
        uint32_t viewIndex,
        bool alignData,
        vk::DeviceSize offset,
        vk::DeviceSize dataSize,
        const void* data
    ) const;

    /// Set the UI Image data
    /// @param platformType Platform to set ui image data for
    /// @param frameIndex Frame index to use
    /// @param uiImageData UI Image data to set
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setUIImageData(
        AxrPlatformType platformType,
        uint32_t frameIndex,
        const std::vector<AxrUIImageData*>& uiImageData
    );

    // ---- Find Assets ----

    /// Find the named push constant buffer, including the global data in the search
    /// @param name The name of the push constant buffer
    /// @returns A handle to the found push constant buffer. Or nullptr if it wasn't found
    [[nodiscard]] const AxrPushConstantBuffer* findPushConstantBuffer_shared(const std::string& name) const;

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    std::string m_SceneName;
    AxrAssetCollection_T m_AssetCollection;
    entt::registry* m_EcsRegistryHandle;
    AxrVulkanSceneData* m_GlobalSceneData;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_Device;
    vk::CommandPool m_GraphicsCommandPool;
    vk::Queue m_GraphicsQueue;
    vk::CommandPool m_TransferCommandPool;
    vk::Queue m_TransferQueue;
    uint32_t m_MaxFramesInFlight;
    float m_MaxSamplerAnisotropy;
    vk::DispatchLoaderDynamic* m_DispatchHandle;

    // ---- Window data ----
    bool m_IsWindowDataLoaded;
    LoadWindowDataConfig m_LoadWindowDataConfig;
    /// Window specific engine defined uniform buffers
    std::unordered_map<std::string, AxrVulkanUniformBufferData> m_WindowUniformBufferData;

    // ---- Xr session data ----
    bool m_IsXrSessionDataLoaded;
    LoadXrSessionDataConfig m_LoadXrSessionDataConfig;
    /// Xr session specific engine defined uniform buffers.
    /// There's one uniform buffer per view.
    std::unordered_map<std::string, std::array<AxrVulkanUniformBufferData, AXR_MAX_XR_VIEWS>>
    m_XrSessionUniformBufferData;

    std::vector<AxrEngineAssetEnum> m_PlatformUniformBuffers;
    std::vector<AxrUniformBuffer> m_LocalUniformBuffers;
    std::vector<AxrMaterial> m_LocalMaterials;
    std::unordered_map<std::string, AxrVulkanUniformBufferData> m_UniformBufferData;
    std::unordered_map<std::string, AxrVulkanModelData> m_ModelData;
    std::unordered_map<std::string, AxrVulkanImageData> m_ImageData;
    std::unordered_map<std::string, AxrVulkanImageSamplerData> m_ImageSamplerData;
    std::unordered_map<std::string, AxrVulkanMaterialLayoutData> m_MaterialLayoutData;
    std::unordered_map<std::string, AxrVulkanMaterialData> m_MaterialData;
    std::vector<AxrVulkanMaterialForRendering> m_OpaqueMaterialsForRendering;
    std::vector<AxrVulkanMaterialForRendering> m_AlphaBlendMaterialsForRendering;
    std::vector<AxrVulkanMaterialForRendering> m_OITMaterialsForRendering;
    std::vector<AxrVulkanMaterialForRendering> m_UIMaterialsForRendering;
    uint32_t m_UIImageResourceCount;
    int32_t m_UIRectangleMaterialForRenderingIndex = -1;
    int32_t m_UIBorderMaterialForRenderingIndex = -1;
    std::vector<int32_t> m_UIImageMaterialForRenderingIndices;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Check if this class is the global scene data
    /// @returns True if this class is the global scene data
    [[nodiscard]] bool isThisGlobalSceneData() const;

    /// Check if the given platform's data is loaded
    /// @param platformType Platform type
    /// @returns True if the given platform's data is loaded
    [[nodiscard]] bool isPlatformLoaded(AxrPlatformType platformType) const;

    // ---- Push Constant Buffer ----

    /// Validate all push constant buffers
    /// @returns AXR_SUCCESS if all push constant buffers are valid
    [[nodiscard]] AxrResult validateAllPushConstantBuffers() const;

    /// Validate the given push constant buffer
    /// @param properties Physical device properties to use
    /// @param pushConstantBuffer Push constant buffer to validate
    /// @returns AXR_SUCCESS if the push constant buffer is valid
    [[nodiscard]] AxrResult validatePushConstantBuffer(
        const vk::PhysicalDeviceProperties& properties,
        const AxrPushConstantBuffer& pushConstantBuffer
    ) const;

    /// 'On push constant buffer created' callback for the asset collection
    /// @param pushConstantBuffer Newly created push constant buffer 
    void onPushConstantBufferCreatedCallback(AxrPushConstantBufferConst_T pushConstantBuffer) const;

    // ---- Uniform Buffer ----

    /// Destroy all uniform buffer data
    /// @param uniformBufferData Uniform buffer data to destroy
    void destroyUniformBufferData(std::unordered_map<std::string, AxrVulkanUniformBufferData>& uniformBufferData);
    /// Destroy all uniform buffer data
    /// @param uniformBufferData Uniform buffer data to destroy
    void destroyUniformBufferData(
        std::unordered_map<std::string, std::array<AxrVulkanUniformBufferData, AXR_MAX_XR_VIEWS>>& uniformBufferData
    );

    /// Create all uniform buffer data
    /// @results AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllUniformBufferData();
    /// Destroy all uniform buffer data
    void destroyAllUniformBufferData();

    /// Initialize a single uniform buffer's data for the given uniform buffer
    /// @param uniformBufferHandle Uniform buffer handle to use
    /// @param uniformBufferData Output Uniform buffer data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult initializeUniformBufferData(
        const AxrUniformBuffer& uniformBufferHandle,
        AxrVulkanUniformBufferData& uniformBufferData
    ) const;

    /// Find the named uniform buffer data, including the global data in the search
    /// @param name The name of the uniform buffer
    /// @param platformType The platform type to check for platform specific uniform buffers
    /// @param viewIndex The view index
    /// @returns A handle to the found uniform buffer. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanUniformBufferData* findUniformBufferData_shared(
        const std::string& name,
        AxrPlatformType platformType,
        uint32_t viewIndex
    ) const;
    /// Find the named local uniform buffer
    /// @param name The name of the uniform buffer
    /// @returns A handle to the found uniform buffer. Or nullptr if it wasn't found
    [[nodiscard]] const AxrUniformBuffer* findLocalUniformBuffer(const std::string& name) const;

    /// Create all window uniform buffer data
    /// @results AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllWindowUniformBufferData();
    /// Destroy all window uniform buffer data
    void destroyAllWindowUniformBufferData();

    /// Find the named window uniform buffer data, including the global data in the search
    /// @param name The name of the window uniform buffer
    /// @returns A handle to the found window uniform buffer. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanUniformBufferData* findWindowUniformBufferData_shared(const std::string& name) const;

    /// Create all xr session uniform buffer data
    /// @results AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllXrSessionUniformBufferData();
    /// Destroy all xr session uniform buffer data
    void destroyAllXrSessionUniformBufferData();

    /// Find the named xr session uniform buffer data, including the global data in the search
    /// @param name The name of the xr session uniform buffer
    /// @param viewIndex The xr view index
    /// @returns A handle to the found xr session uniform buffer. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanUniformBufferData* findXrSessionUniformBufferData_shared(
        const std::string& name,
        uint32_t viewIndex
    ) const;

    /// 'On uniform buffer created' callback for the asset collection
    /// @param uniformBuffer Newly created uniform buffer 
    void onUniformBufferCreatedCallback(AxrUniformBufferConst_T uniformBuffer);

    // ---- Model ----

    /// Create all model data
    /// @results AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllModelData();
    /// Destroy all model data
    void destroyAllModelData();

    /// Initialize a single model's data for the given model
    /// @param model Model to use
    /// @param modelData Output model data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult initializeModelData(const AxrModel& model, AxrVulkanModelData& modelData) const;

    /// Find the named model data, including the global data in the search
    /// @param name The name of the model
    /// @returns A handle to the found model. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanModelData* findModelData_shared(const std::string& name) const;

    /// 'On model created' callback for the asset collection
    /// @param model Newly created model 
    void onModelCreatedCallback(AxrModelConst_T model);

    // ---- Image ----

    /// Create all image data
    /// @results AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllImageData();
    /// Destroy all image data
    void destroyAllImageData();

    /// Initialize a single image's data for the given image
    /// @param image Image to use
    /// @param imageData Output image data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult initializeImageData(const AxrImage& image, AxrVulkanImageData& imageData) const;

    /// Find the named image data, including the global data in the search
    /// @param name The name of the image
    /// @returns A handle to the found image. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanImageData* findImageData_shared(const std::string& name) const;

    /// 'On image created' callback for the asset collection
    /// @param image Newly created image 
    void onImageCreatedCallback(AxrImageConst_T image);

    // ---- Image Sampler ----

    /// Create all image sampler data
    /// @results AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllImageSamplerData();
    /// Destroy all image sampler data
    void destroyAllImageSamplerData();

    /// Initialize a single image sampler's data for the given image sampler
    /// @param imageSampler Image sampler to use
    /// @param imageSamplerData Output image sampler data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult initializeImageSamplerData(
        const AxrImageSampler& imageSampler,
        AxrVulkanImageSamplerData& imageSamplerData
    ) const;

    /// Find the named image sampler data, including the global data in the search
    /// @param name The name of the image sampler
    /// @returns A handle to the found image sampler. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanImageSamplerData* findImageSamplerData_shared(const std::string& name) const;

    /// 'On image sampler created' callback for the asset collection
    /// @param imageSampler Newly created image sampler
    void onImageSamplerCreatedCallback(AxrImageSamplerConst_T imageSampler);

    // ---- Shader ----

    /// Find the named shader, including the global data in the search
    /// @param name The name of the shader
    /// @returns A handle to the found shader. Or nullptr if it wasn't found
    [[nodiscard]] const AxrShader* findShader_shared(const std::string& name) const;

    // ---- Material Layout ----

    /// Create all material layouts data
    /// @results AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllMaterialLayoutData();
    /// Destroy all material layouts data
    void destroyAllMaterialLayoutData();

    /// Initialize a single material layout for the given material
    /// @param material Material to use
    /// @param materialLayoutData Output material layout data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult initializeMaterialLayoutData(
        const AxrMaterial& material,
        AxrVulkanMaterialLayoutData& materialLayoutData
    ) const;

    /// Find the named material layout data, including the global data in the search
    /// @param name The name of the material layout
    /// @returns A handle to the found material layout. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanMaterialLayoutData* findMaterialLayoutData_shared(const std::string& name) const;

    // ---- Material ----

    /// Create all material data
    /// @results AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllMaterialData();
    /// Destroy all material data
    void destroyAllMaterialData();

    /// Create a single material data
    /// @param material Material to use
    /// @param nameSuffix Material name suffix
    /// @results A handle to the created material data. Or nullptr if it failed.
    AxrVulkanMaterialData* createMaterialData(const AxrMaterial& material, const std::string& nameSuffix = "");

    /// Initialize a single material data for the given material
    /// @param material Material to use
    /// @param materialData Output material data
    /// @param nameSuffix Suffix to add to the material name
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult initializeMaterialData(
        const AxrMaterial& material,
        AxrVulkanMaterialData& materialData,
        const std::string& nameSuffix = ""
    ) const;

    /// Create more UI image materials to use
    /// @param minImageCount Minimum number of images to allow for
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAdditionalUIImageMaterials(uint32_t minImageCount);

    /// Create all window specific material data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllWindowMaterialData();
    /// Destroy all window specific material data
    void destroyAllWindowMaterialData();

    /// Create all xr session specific material data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllXrSessionMaterialData();
    /// Destroy all xr session specific material data
    void destroyAllXrSessionMaterialData();

    /// Find the named material data, including the global data in the search
    /// @param name The name of the material
    /// @returns A handle to the found material. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanMaterialData* findMaterialData_shared(const std::string& name) const;
    /// Find the named local material, including the global data in the search
    /// @param name The name of the material
    /// @returns A handle to the found material. Or nullptr if it wasn't found
    [[nodiscard]] const AxrMaterial* findLocalMaterial_shared(const std::string& name) const;

    /// 'On material created' callback for the asset collection
    /// @param material Newly created material
    void onMaterialCreatedCallback(AxrMaterialConst_T material);

    // ---- Write Descriptor Sets ----

    /// Write all material descriptor sets
    /// @param platformType The platform type to use
    /// @param viewCount The number of views for the xr device
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult writeAllDescriptorSets(AxrPlatformType platformType, uint32_t viewCount);
    /// Reset all material descriptor sets
    /// @param platformType The platform type to use
    void resetAllDescriptorSets(AxrPlatformType platformType);

    /// Write the descriptor sets for the given material data
    /// @param platformType The platform type to use
    /// @param viewCount The number of views for the xr device
    /// @param materialData The material data to use
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult writeDescriptorSets(
        AxrPlatformType platformType,
        uint32_t viewCount,
        AxrVulkanMaterialData& materialData
    ) const;
    /// Reset the descriptor sets for the given material data
    /// @param platformType The platform type to use
    /// @param materialData The material data
    void resetDescriptorSets(AxrPlatformType platformType, AxrVulkanMaterialData& materialData) const;

    /// Write to the UI Image descriptor sets
    /// @param platformType Platform type to use
    /// @param frameIndex Frame index to use
    /// @param viewCount View count
    /// @param uiImageData UI Image data to use
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult writeUIImageDescriptorSets(
        AxrPlatformType platformType,
        uint32_t frameIndex,
        uint32_t viewCount,
        const std::vector<AxrUIImageData*>& uiImageData
    ) const;

    // ---- Materials For Rendering ----

    /// Create all materials for rendering
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAllMaterialsForRendering();
    /// Destroy all materials for rendering
    void destroyAllMaterialsForRendering();

    /// Create all UI materials for rendering
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createUIMaterialsForRendering();
    /// Destroy all UI materials for rendering
    void destroyUIMaterialsForRendering();

    /// Add a material for rendering to either m_OpaqueMaterialsForRendering, m_AlphaBlendMaterialsForRendering
    /// or m_OITMaterialsForRendering depending on it's alpha rendering mode. 
    /// @param transformComponent Transform component
    /// @param modelComponent Model component
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult addMaterialForRendering(
        const AxrTransformComponent& transformComponent,
        const AxrModelComponent& modelComponent
    );
    /// Build a simple material for rendering using the given material and model data
    /// @param materialData Material data to use
    /// @param modelData Model data to use
    /// @param materialForRendering Output material for rendering
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult buildUIMaterialForRendering(
        const AxrVulkanMaterialData* materialData,
        const AxrVulkanModelData* modelData,
        AxrVulkanMaterialForRendering& materialForRendering
    ) const;

    /// 'On new renderable entity' callback for the entt registry
    /// @param registry The entt registry 
    /// @param entity The new renderable entity 
    void onNewRenderableEntityCallback(entt::registry& registry, entt::entity entity);

    /// Find the named material in the given collection of 'materials for rendering'
    /// @param materials 'Materials for rendering' collection
    /// @param materialName Material name
    /// @returns A handle to the found material or nullptr if it wasn't found.
    [[nodiscard]] AxrVulkanMaterialForRendering* findMaterialForRendering(
        std::vector<AxrVulkanMaterialForRendering>& materials,
        const std::string& materialName
    ) const;
};

#endif
