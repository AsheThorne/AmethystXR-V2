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
    // Structs
    // ----------------------------------------- //

    /// Font data config
    struct Config {
        const AxrFont* FontHandle;
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
    /// Get the font's material data
    /// @returns The font's material data
    [[nodiscard]] AxrVulkanMaterialData& getMaterialData();

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

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Clean up this class
    void cleanup();
};
