#pragma once
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "../../../assets/uniformBuffer.hpp"
#include "../vulkanBuffer.hpp"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <string>

// ----------------------------------------- //
// Vulkan Headers
// ----------------------------------------- //
#include <vulkan/vulkan.hpp>

/// Vulkan uniform buffer data
class AxrVulkanUniformBufferData {
public:
    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Uniform buffer data config
    struct Config {
        const AxrUniformBuffer* UniformBufferHandle;
        uint32_t MaxFramesInFlight;
        vk::PhysicalDevice PhysicalDevice;
        vk::Device Device;
        vk::CommandPool TransferCommandPool;
        vk::Queue TransferQueue;
        vk::DispatchLoaderDynamic* DispatchHandle;
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Default Constructor
    AxrVulkanUniformBufferData();
    /// Constructor
    /// @param config Uniform buffer data config
    explicit AxrVulkanUniformBufferData(const Config& config);
    /// Copy Constructor
    /// @param src Source AxrVulkanUniformBufferData to copy from
    AxrVulkanUniformBufferData(const AxrVulkanUniformBufferData& src) = delete;
    /// Move Constructor
    /// @param src Source AxrVulkanUniformBufferData to move from
    AxrVulkanUniformBufferData(AxrVulkanUniformBufferData&& src) noexcept;

    // ---- Destructor ----

    /// Destructor
    ~AxrVulkanUniformBufferData();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrVulkanUniformBufferData to copy from
    AxrVulkanUniformBufferData& operator=(const AxrVulkanUniformBufferData& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrVulkanUniformBufferData to move from
    AxrVulkanUniformBufferData& operator=(AxrVulkanUniformBufferData&& src) noexcept;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Get the uniform buffer name
    /// @returns The uniform buffer name
    [[nodiscard]] const std::string& getName() const;
    /// Get the vulkan buffer at the given frame
    /// @param frameIndex Frame index to use
    /// @returns The vulkan buffer
    [[nodiscard]] const AxrVulkanBuffer& getBuffer(uint32_t frameIndex) const;
    /// Get the uniform buffer size
    /// @returns The uniform buffer size
    [[nodiscard]] vk::DeviceSize getBufferSize() const;
    /// Get the uniform buffer instance size
    /// @returns The uniform buffer instance size
    [[nodiscard]] vk::DeviceSize getInstanceSize() const;
    /// Get the uniform buffer type
    /// @returns The uniform buffer type
    [[nodiscard]] AxrUniformBufferTypeEnum getBufferType() const;

    /// Check if the data exists
    /// @returns True if the data exists
    [[nodiscard]] bool doesDataExist() const;

    /// Create the uniform buffer data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createData();
    /// Destroy the uniform buffer data
    void destroyData();

    /// Set the buffer data.
    /// If `alignData` is true, `data` must contain whole instance objects without any padding between them.
    /// @param frameIndex Frame in flight index
    /// @param alignData True if we want to align it with the min uniform buffer offset alignment.
    /// @param offset Offset of the data to set
    /// @param size Size of the data to set
    /// @param data Data to set
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setData(
        uint32_t frameIndex,
        bool alignData,
        vk::DeviceSize offset,
        vk::DeviceSize size,
        const void* data
    ) const;
    /// Update 'dirty' data
    /// @param frameIndex Frame in flight index
    void updateDirtyData(uint32_t frameIndex) const;

    // ----------------------------------------- //
    // Public Static Functions
    // ----------------------------------------- //

    /// Calculate the uniform buffer offset alignment for the given instance size
    /// @param physicalDevice Physical device to use
    /// @param instanceSize Instance size
    /// @param dispatch Dispatch to use
    /// @returns The uniform buffer offset alignment
    [[nodiscard]] static vk::DeviceSize calculateUniformBufferAlignment(
        vk::PhysicalDevice physicalDevice,
        vk::DeviceSize instanceSize,
        const vk::DispatchLoaderDynamic& dispatch
    );

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    const AxrUniformBuffer* m_UniformBufferHandle = nullptr;
    uint32_t m_MaxFramesInFlight = 0;
    vk::PhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    vk::Device m_Device = VK_NULL_HANDLE;
    vk::CommandPool m_TransferCommandPool = VK_NULL_HANDLE;
    vk::Queue m_TransferQueue = VK_NULL_HANDLE;
    vk::DispatchLoaderDynamic* m_DispatchHandle = nullptr;

    // ---- Data ----
    /// This should never be used for anything other than returning a reference to the name if no name can be found.
    std::string m_DummyName;
    vk::DeviceSize m_UniformBufferAlignment = 0;
    /// One buffer per frame in flight
    std::vector<AxrVulkanBuffer> m_UniformBuffers;
    bool m_IsDataDirty = false;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Clean up this class
    void cleanup();

    // ---- Data ----

    /// Set the uniform buffer alignment
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setAlignment();
    /// Reset the uniform buffer alignment
    void resetAlignment();

    /// Create a vulkan buffer to use as the uniform buffer
    /// @param buffer Output vulkan buffer
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createUniformBuffer(AxrVulkanBuffer& buffer) const;
    /// Destroy a vulkan buffer
    void destroyUniformBuffer(AxrVulkanBuffer& buffer) const;

    /// Set the buffer data.
    /// If `alignData` is true, `data` must contain whole instance objects without any padding between them.
    /// @param buffer Buffer to set the data for
    /// @param alignData True if we want to align it with the min uniform buffer offset alignment.
    /// @param offset Offset of the data to set
    /// @param size Size of the data to set
    /// @param data Data to set
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setData(
        const AxrVulkanBuffer& buffer,
        bool alignData,
        vk::DeviceSize offset,
        vk::DeviceSize size,
        const void* data
    ) const;

    /// Uniform buffer 'On Data Changed' callback function
    void onDataChangedCallback();
};
#endif
