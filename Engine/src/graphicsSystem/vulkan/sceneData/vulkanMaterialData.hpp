#pragma once
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/common/enums.h"
#include "vulkanMaterialLayoutData.hpp"
#include "axr/common/callback.h"
#include "vulkanUniformBufferData.hpp"
#include "vulkanImageSamplerData.hpp"
#include "vulkanImageData.hpp"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <string>

// ----------------------------------------- //
// Vulkan Headers
// ----------------------------------------- //
#include <vulkan/vulkan.hpp>

/// Vulkan material data
class AxrVulkanMaterialData {
public:
    // ----------------------------------------- //
    // Types
    // ----------------------------------------- //

    /// Find uniform buffer callback function type
    /// @param 1: The name of the uniform buffer
    /// @param 2: The platform type to check for platform specific uniform buffers
    /// @param 3: The view index
    using FindUniformBufferCallback_T = AxrCallback<const AxrVulkanUniformBufferData*(
        const std::string& name,
        AxrPlatformType platformType,
        uint32_t viewIndex
    )>;
    /// Find image sampler callback function type
    /// @param 1: The name of the uniform buffer
    using FindImageSamplerCallback_T = AxrCallback<const AxrVulkanImageSamplerData*(
        const std::string& name
    )>;
    /// Find image callback function type
    /// @param 1: The name of the uniform buffer
    using FindImageCallback_T = AxrCallback<const AxrVulkanImageData*(
        const std::string& name
    )>;

    // ----------------------------------------- //
    // Public Variables
    // ----------------------------------------- //

    /// Find uniform buffer callback function
    FindUniformBufferCallback_T FindUniformBufferCallback;
    /// Find image sampler callback function
    FindImageSamplerCallback_T FindImageSamplerCallback;
    /// Find image callback function
    FindImageCallback_T FindImageCallback;

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Material data config
    struct Config {
        const AxrMaterial* MaterialHandle;
        std::string MaterialNameSuffix;
        const AxrVulkanMaterialLayoutData* MaterialLayoutData;
        uint32_t MaxFramesInFlight;
        vk::Device Device;
        vk::DispatchLoaderDynamic* DispatchHandle;
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Default Constructor
    AxrVulkanMaterialData();
    /// Constructor
    /// @param config Material data config
    explicit AxrVulkanMaterialData(const Config& config);
    /// Copy Constructor
    /// @param src Source AxrVulkanMaterialData to copy from
    AxrVulkanMaterialData(const AxrVulkanMaterialData& src) = delete;
    /// Move Constructor
    /// @param src Source AxrVulkanMaterialData to move from
    AxrVulkanMaterialData(AxrVulkanMaterialData&& src) noexcept;

    // ---- Destructor ----

    /// Destructor
    ~AxrVulkanMaterialData();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrVulkanMaterialData to copy from
    AxrVulkanMaterialData& operator=(const AxrVulkanMaterialData& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrVulkanMaterialData to move from
    AxrVulkanMaterialData& operator=(AxrVulkanMaterialData&& src) noexcept;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Get the material name
    /// @returns The material name
    [[nodiscard]] const std::string& getName() const;
    /// Get the material layout data used for this material data
    /// @returns The material layout data
    [[nodiscard]] const AxrVulkanMaterialLayoutData* getMaterialLayoutData() const;
    /// Get the window pipeline
    /// @returns The window pipeline
    [[nodiscard]] const vk::Pipeline& getWindowPipeline() const;
    /// Get the xr session pipeline
    /// @returns The xr session pipeline
    [[nodiscard]] const vk::Pipeline& getXrSessionPipeline() const;
    /// Get the descriptor sets for the given platform
    /// @param platformType Platform to get the descriptor sets of
    /// @returns The descriptor sets
    [[nodiscard]] const std::vector<vk::DescriptorSet>& getDescriptorSets(AxrPlatformType platformType) const;
    /// Reset the descriptor sets for the given platform
    /// @param platformType Platform to reset the descriptor sets of
    void resetDescriptorSets(AxrPlatformType platformType);
    /// Get a handle to the material this class is built from
    /// @returns A handle to the material this class is built from 
    [[nodiscard]] const AxrMaterial* getMaterial() const;
    /// Get the dynamic uniform buffer offsets
    /// @returns The dynamic uniform buffer offsets
    [[nodiscard]] const std::vector<AxrDynamicUniformBufferOffsetConfig>& getDynamicUniformBufferOffsets() const;

    /// Check if the data exists
    /// @returns True if the data exists
    [[nodiscard]] bool doesDataExist() const;
    /// Check if the window specific data exists
    /// @returns True if the window specific data exists
    [[nodiscard]] bool doesWindowDataExist() const;
    /// Check if the xr session specific data exists
    /// @returns True if the xr session specific data exists
    [[nodiscard]] bool doesXrSessionDataExist() const;

    /// Create the material data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createData();
    /// Destroy the material data
    void destroyData();

    /// Create the window specific material data
    /// @param renderPass Render pass to use
    /// @param msaaSampleCount Msaa sample count
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createWindowData(
        vk::RenderPass renderPass,
        vk::SampleCountFlagBits msaaSampleCount
    );
    /// Destroy the window specific material data
    void destroyWindowData();

    /// Create the xr session specific material data
    /// @param renderPass Render pass to use
    /// @param msaaSampleCount Msaa sample count
    /// @param viewCount The number of views for the xr device
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createXrSessionData(
        vk::RenderPass renderPass,
        vk::SampleCountFlagBits msaaSampleCount,
        uint32_t viewCount
    );
    /// Destroy the xr session specific material data
    void destroyXrSessionData();

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    const AxrMaterial* m_MaterialHandle;
    const AxrVulkanMaterialLayoutData* m_MaterialLayoutData;
    uint32_t m_MaxFramesInFlight;
    vk::Device m_Device;
    vk::DispatchLoaderDynamic* m_DispatchHandle;

    // ---- Data ----
    /// These should never be used for anything other than returning a reference to descriptor sets when there's no other option.
    std::vector<vk::DescriptorSet> m_DummyDescriptorSets;
    std::string m_Name;

    // ---- Window Data ----
    vk::DescriptorPool m_WindowDescriptorPool;
    /// One for each frame in flight
    std::vector<vk::DescriptorSet> m_WindowDescriptorSets;
    vk::Pipeline m_WindowPipeline;

    // ---- Xr Session Data ----
    vk::DescriptorPool m_XrSessionDescriptorPool;
    /// One for each frame in flight and for each view.
    /// Data organised view first, like this: [View1-Frame1, View1-Frame2, ..., View2-Frame1, View2-Frame2, ...].
    std::vector<vk::DescriptorSet> m_XrSessionDescriptorSets;
    vk::Pipeline m_XrSessionPipeline;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Clean up this class
    void cleanup();

    // ---- Descriptor Pool ----

    /// Create a descriptor pool
    /// @param viewCount How many views will be used in rendering
    /// @param descriptorPool The output created descriptor pool
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createDescriptorPool(
        uint32_t viewCount,
        vk::DescriptorPool& descriptorPool
    ) const;
    /// Destroy the given descriptor pool
    /// @param descriptorPool The descriptor pool to destroy
    void destroyDescriptorPool(vk::DescriptorPool& descriptorPool) const;

    // ---- Descriptor Sets ----

    /// Create the descriptor sets
    /// @param viewCount The number of views for the xr device
    /// @param descriptorPool The descriptor pool to use
    /// @param descriptorSets The output descriptor sets
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createDescriptorSets(
        uint32_t viewCount,
        vk::DescriptorPool descriptorPool,
        std::vector<vk::DescriptorSet>& descriptorSets
    ) const;
    /// Reset the given descriptor pool and descriptor sets
    /// @param descriptorPool The descriptor pool to reset
    /// @param descriptorSets The descriptor sets to reset
    void resetDescriptorSets(
        vk::DescriptorPool descriptorPool,
        std::vector<vk::DescriptorSet>& descriptorSets
    ) const;

    /// Write the descriptor sets for the given platform type
    /// @param platformType The platform type to use
    /// @param viewCount The number of views for the xr device
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult writeDescriptorSets(
        AxrPlatformType platformType,
        uint32_t viewCount
    ) const;

    // ---- Pipeline ----

    /// Create a pipeline
    /// @param renderPass Render pass to use
    /// @param msaaSampleCount Msaa sample count
    /// @param pipeline Output created pipeline
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createPipeline(
        vk::RenderPass renderPass,
        vk::SampleCountFlagBits msaaSampleCount,
        vk::Pipeline& pipeline
    ) const;
    /// Destroy the given pipeline
    /// @param pipeline Pipeline to destroy
    void destroyPipeline(vk::Pipeline& pipeline);

    /// Clean up the data that's used in a pipeline's creation
    /// @param shaderModules Shader modules
    void cleanupPipelineCreationData(std::vector<vk::ShaderModule>& shaderModules) const;

    /// Create a shader module
    /// @param shaderFileData Shader file data
    /// @param shaderModule Output created shader module
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createShaderModule(
        const std::vector<char>& shaderFileData,
        vk::ShaderModule& shaderModule
    ) const;
    /// Destroy the given shader module
    /// @param shaderModule Shader module to destroy
    void destroyShaderModule(vk::ShaderModule& shaderModule) const;

    /// Get the format for the given vertex attribute enum
    /// @param vertexAttribute Vertex attribute
    /// @returns The format
    [[nodiscard]] vk::Format getVertexAttributeFormat(AxrShaderVertexAttributeEnum vertexAttribute) const;
    /// Get the offset for the given vertex attribute enum
    /// @param vertexAttribute Vertex attribute
    /// @returns The offset
    [[nodiscard]] uint32_t getVertexAttributeOffset(AxrShaderVertexAttributeEnum vertexAttribute) const;
};

#endif
