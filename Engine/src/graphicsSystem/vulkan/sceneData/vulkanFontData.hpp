#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "../../../assets/font.hpp"
#include "vulkanMaterialData.hpp"

// ----------------------------------------- //
// Vulkan Headers
// ----------------------------------------- //
#include <vulkan/vulkan.hpp>

/// Vulkan font data
class AxrVulkanFontData {
public:
    // ----------------------------------------- //
    // Types
    // ----------------------------------------- //

    /// Find uniform buffer callback function type
    /// @param 1: The name of the uniform buffer
    /// @param 2: The platform type to check for platform specific uniform buffers
    /// @param 3: The view index
    using FindUniformBufferCallback_T = AxrCallback<const AxrVulkanUniformBufferData*(
        const std::string&,
        AxrPlatformType,
        uint32_t
    )>;
    /// Find image sampler callback function type
    /// @param 1: The name of the uniform buffer
    using FindImageSamplerCallback_T = AxrCallback<const AxrVulkanImageSamplerData*(const std::string&)>;
    /// Find image callback function type
    /// @param 1: The name of the uniform buffer
    using FindImageCallback_T = AxrCallback<const AxrVulkanImageData*(const std::string&)>;

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

    /// Font data config
    struct Config {
        const AxrFont* FontHandle;
        const AxrVulkanMaterialLayoutData* MaterialLayoutData;
        uint32_t MaxFramesInFlight;
        vk::Device Device;
        vk::PhysicalDevice PhysicalDevice;
        vk::CommandPool TransferCommandPool;
        vk::Queue TransferQueue;
        vk::DispatchLoaderDynamic* DispatchHandle;
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Default Constructor
    AxrVulkanFontData();
    /// Constructor
    /// @param config Font data config
    explicit AxrVulkanFontData(const Config& config);
    /// Copy Constructor
    /// @param src Source AxrVulkanFontData to copy from
    AxrVulkanFontData(const AxrVulkanFontData& src) = delete;
    /// Move Constructor
    /// @param src Source AxrVulkanFontData to move from
    AxrVulkanFontData(AxrVulkanFontData&& src) noexcept;

    // ---- Destructor ----

    /// Destructor
    ~AxrVulkanFontData();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrVulkanFontData to copy from
    AxrVulkanFontData& operator=(const AxrVulkanFontData& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrVulkanFontData to move from
    AxrVulkanFontData& operator=(AxrVulkanFontData&& src) noexcept;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Get the font name
    /// @returns The font name
    [[nodiscard]] const std::string& getName() const;
    /// Get the font's ID
    /// @returns The font's ID
    [[nodiscard]] uint16_t getID() const;
    /// Get the font's material data
    /// @returns The font's material data
    [[nodiscard]] const AxrVulkanMaterialData& getMaterialData() const;
    /// Get the font data uniform buffer data
    /// @returns The font data uniform buffer data
    [[nodiscard]] const AxrVulkanUniformBufferData& getFontUniformBufferData() const;
    /// Get the font's glyph uniform buffer data
    /// @returns The font's glyph uniform buffer data
    [[nodiscard]] const AxrVulkanUniformBufferData& getGlyphUniformBufferData() const;

    /// Check if the data exists
    /// @returns True if the data exists
    [[nodiscard]] bool doesDataExist() const;
    /// Check if the window specific data exists
    /// @returns True if the window specific data exists
    [[nodiscard]] bool doesWindowDataExist() const;
    /// Check if the xr session specific data exists
    /// @returns True if the xr session specific data exists
    [[nodiscard]] bool doesXrSessionDataExist() const;

    /// Create the font data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createData();
    /// Destroy the font data
    void destroyData();

    /// Create the window specific font data
    /// @param renderPass Render pass to use
    /// @param msaaSampleCount Msaa sample count
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createWindowData(
        vk::RenderPass renderPass,
        vk::SampleCountFlagBits msaaSampleCount
    );
    /// Destroy the window specific font data
    void destroyWindowData();

    /// Create the xr session specific font data
    /// @param renderPass Render pass to use
    /// @param msaaSampleCount Msaa sample count
    /// @param viewCount The number of views for the xr device
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createXrSessionData(
        vk::RenderPass renderPass,
        vk::SampleCountFlagBits msaaSampleCount,
        uint32_t viewCount
    );
    /// Destroy the xr session specific font data
    void destroyXrSessionData();

    /// Update all 'dirty' uniform buffer data
    /// @param frameIndex Frame index to use
    void updateDirtyUniformBufferData(uint32_t frameIndex) const;

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    const AxrFont* m_FontHandle = nullptr;
    vk::Device m_Device = VK_NULL_HANDLE;
    vk::DispatchLoaderDynamic* m_DispatchHandle = nullptr;

    // ---- Data ----
    /// This should never be used for anything other than returning a reference to the name if no name can be found.
    std::string m_DummyName;
    AxrVulkanMaterialData m_MaterialData = {};
    AxrVulkanUniformBufferData m_FontUniformBufferData = {};
    AxrVulkanUniformBufferData m_GlyphUniformBufferData = {};

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Clean up this class
    void cleanup();

    /// Set the material data function callbacks
    void setMaterialDataCallbacks();

    /// Find the named uniform buffer data
    /// @param name The name of the uniform buffer
    /// @param platformType The platform type to check for platform specific uniform buffers
    /// @param viewIndex The view index
    /// @returns A handle to the found uniform buffer. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanUniformBufferData* findUniformBufferData(
        const std::string& name,
        AxrPlatformType platformType,
        uint32_t viewIndex
    ) const;
    /// Find the named image sampler data
    /// @param name The name of the image sampler
    /// @returns A handle to the found image sampler. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanImageSamplerData* findImageSamplerData(const std::string& name) const;
    /// Find the named image data
    /// @param name The name of the image
    /// @returns A handle to the found image. Or nullptr if it wasn't found
    [[nodiscard]] const AxrVulkanImageData* findImageData(const std::string& name) const;
};
