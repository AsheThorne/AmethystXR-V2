#pragma once
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/windowSystem.h"
#include "../../extensionCollection.hpp"
#include "vulkanQueueFamilies.hpp"
#include "sceneData/vulkanLoadedScenesCollection.hpp"
#include "vulkanSurfaceDetails.hpp"
#include "axr/graphicsSystem.h"
#include "../graphicsCommonStructs.hpp"

// ----------------------------------------- //
// Vulkan Headers
// ----------------------------------------- //
#include <vulkan/vulkan.hpp>

/// Vulkan Window Graphics
class AxrVulkanWindowGraphics {
public:
    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// AxrVulkanWindowGraphics Config
    struct Config {
        AxrWindowSystem& WindowSystem;
        vk::DispatchLoaderDynamic& Dispatch;
        AxrVulkanLoadedScenesCollection& LoadedScenes;
        uint32_t MaxFramesInFlight;
        AxrVulkanPresentationModeEnum PresentationMode;
        AxrMsaaSampleCountEnum MaxMsaaSampleCount;
    };

    /// AxrVulkanWindowGraphics Setup Config
    struct SetupConfig {
        vk::Instance Instance;
        vk::PhysicalDevice PhysicalDevice;
        vk::Device Device;
        vk::CommandPool GraphicsCommandPool;
        const AxrVulkanQueueFamilies& QueueFamilies;
        /// Ordered from most desired to the least desired
        const std::vector<vk::SurfaceFormatKHR>& SwapchainColorFormatOptions;
        /// Ordered from most desired to the least desired
        const std::vector<vk::Format>& SwapchainDepthFormatOptions;
    };

    /// Render matrices
    struct RenderMatrices {
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
    };

    // Render data for each frame
    struct RenderData {
        RenderMatrices RenderMatrices;

        void reset() {
            RenderMatrices = {};
        }
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Constructor
    /// @param config window graphics config
    explicit AxrVulkanWindowGraphics(const Config& config);
    /// Copy Constructor
    /// @param src Source AxrVulkanWindowGraphics to copy from
    AxrVulkanWindowGraphics(const AxrVulkanWindowGraphics& src) = delete;
    /// Move Constructor
    /// @param src Source AxrVulkanWindowGraphics to move from
    AxrVulkanWindowGraphics(AxrVulkanWindowGraphics&& src) noexcept = delete;

    // ---- Destructor ----

    /// Destructor
    ~AxrVulkanWindowGraphics();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrVulkanWindowGraphics to copy from
    AxrVulkanWindowGraphics& operator=(const AxrVulkanWindowGraphics& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrVulkanWindowGraphics to move from
    AxrVulkanWindowGraphics& operator=(AxrVulkanWindowGraphics&& src) noexcept = delete;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Add the instance extensions required for window graphics to the given extension collection
    /// @param extensions The extension collection to add to
    void addRequiredInstanceExtensions(
        AxrExtensionCollection<AxrVulkanExtension_T, AxrVulkanExtensionTypeEnum>& extensions
    ) const;
    /// Add the device extensions required for window graphics to the given extension collection
    /// @param extensions The extension collection to add to
    void addRequiredDeviceExtensions(
        AxrExtensionCollection<AxrVulkanExtension_T, AxrVulkanExtensionTypeEnum>& extensions
    ) const;

    /// Set the clear color
    /// @param color Clear color
    void setClearColor(const AxrColor& color);

    /// Set the window render source option
    /// @param renderSource Window render source
    void setRenderSource(AxrWindowRenderSourceEnum renderSource);
    /// Get the window render source option
    /// @returns The window render source option
    [[nodiscard]] AxrWindowRenderSourceEnum getRenderSource() const;

    /// Set up vulkan window graphics
    /// @param config Setup config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setup(const SetupConfig& config);
    /// Reset the setup() function
    void resetSetup();

    /// Check if the window graphics are ready for rendering
    /// @returns True if the window graphics are ready for rendering
    [[nodiscard]] bool isReady() const;

    /// Begin rendering
    /// @param sceneData Active scene data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult beginRendering(const AxrVulkanSceneData* sceneData);
    /// End rendering
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult endRendering();
    /// Get the number of views
    /// @returns The number of views
    [[nodiscard]] uint32_t getViewCount() const;
    /// Get the platform type
    /// @returns the platform type
    [[nodiscard]] AxrPlatformType getPlatformType() const;
    /// Get the UI region
    /// @returns The UI region
    [[nodiscard]] vk::Extent2D getUIRegion() const;
    /// Get the render pass
    /// @returns The render pass
    [[nodiscard]] vk::RenderPass getRenderPass() const;
    /// Get the framebuffer for the current swapchain image
    /// @param viewIndex View index
    /// @returns The framebuffer for the current swapchain image
    [[nodiscard]] vk::Framebuffer getFramebuffer(uint32_t viewIndex) const;
    /// Get the swapchain extent
    /// @param viewIndex View index
    /// @returns The swapchain extent
    [[nodiscard]] vk::Extent2D getSwapchainExtent(uint32_t viewIndex) const;
    /// Get the clear color value
    /// @returns The clear color value
    [[nodiscard]] vk::ClearColorValue getClearColorValue() const;
    /// Get the command buffer to use for rendering for the current frame
    /// @param viewIndex View index
    /// @returns The rendering command buffer for the current frame
    [[nodiscard]] vk::CommandBuffer getRenderingCommandBuffer(uint32_t viewIndex) const;
    /// Get the rendering wait semaphores to use for the current frame
    /// @param viewIndex View index
    /// @returns The wait semaphores for the current frame
    [[nodiscard]] std::vector<vk::Semaphore> getRenderingWaitSemaphores(uint32_t viewIndex) const;
    /// Get the rendering wait stages to use for the current frame
    /// @param viewIndex View index
    /// @returns The wait stages for the current frame
    [[nodiscard]] std::vector<vk::PipelineStageFlags> getRenderingWaitStages(uint32_t viewIndex) const;
    /// Get the rendering signal semaphores to use for the current frame
    /// @param viewIndex View index
    /// @returns The signal semaphores for the current frame
    [[nodiscard]] std::vector<vk::Semaphore> getRenderingSignalSemaphores(uint32_t viewIndex) const;
    /// Get the rendering fence to use for the current frame
    /// @param viewIndex View index
    /// @returns The rendering fence for the current frame
    [[nodiscard]] vk::Fence getRenderingFence(uint32_t viewIndex) const;
    /// Get the current rendering frame index
    /// @returns The current rendering frame index
    [[nodiscard]] uint32_t getCurrentRenderingFrame() const;
    /// Get the vk::image for the current swapchain image
    /// @param viewIndex View index
    /// @returns The vk::image for the current swapchain image
    [[nodiscard]] vk::Image getSwapchainImage(uint32_t viewIndex) const;
    /// Get the swapchain image format
    /// @returns The swapchain image format
    [[nodiscard]] vk::Format getSwapchainImageFormat() const;
    /// Get the swapchain image layout
    /// @returns The swapchain image layout
    [[nodiscard]] vk::ImageLayout getSwapchainImageLayout() const;

    /// Acquire the next swapchain image
    /// @param viewIndex View index
    /// @returns AXR_SUCCESS if the function succeeded.
    /// @returns AXR_DONT_RENDER if we should skip rendering this frame.
    [[nodiscard]] AxrResult acquireNextSwapchainImage(uint32_t viewIndex);
    /// Present the current frame to the window
    /// @param viewIndex View index
    /// @returns AXR_SUCCESS if the function succeeded.
    /// @returns AXR_DONT_RENDER if we should skip rendering this frame.
    [[nodiscard]] AxrResult presentFrame(uint32_t viewIndex);

    /// Get the rendering matrices for the given view
    /// @param viewIndex View index
    /// @param viewMatrix Output view matrix
    /// @param projectionMatrix Output projection matrix
    void getRenderingMatrices(uint32_t viewIndex, glm::mat4& viewMatrix, glm::mat4& projectionMatrix) const;
    /// Get the camera info for the given view
    /// @param viewIndex View index
    /// @param cameraInfo Output camera info
    /// @returns AXR_SUCCESS if the function succeeded.
    [[nodiscard]] AxrResult getCameraInfo(
        uint32_t viewIndex,
        AxrCameraInfo& cameraInfo
    ) const;

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config ----
    AxrWindowSystem& m_WindowSystem;
    vk::DispatchLoaderDynamic& m_Dispatch;
    AxrVulkanLoadedScenesCollection& m_LoadedScenes;
    uint32_t m_MaxFramesInFlight;
    AxrVulkanPresentationModeEnum m_PreferredPresentationMode;
    AxrMsaaSampleCountEnum m_MaxMsaaSampleCount;

    // ---- Setup Config ----
    vk::Instance m_Instance;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_Device;
    vk::CommandPool m_GraphicsCommandPool;
    AxrVulkanQueueFamilies m_QueueFamilies;
    /// Ordered from most desired to the least desired
    std::vector<vk::SurfaceFormatKHR> m_SwapchainColorFormatOptions;
    /// Ordered from most desired to the least desired
    std::vector<vk::Format> m_SwapchainDepthFormatOptions;

    AxrColor m_ClearColor;
    AxrWindowRenderSourceEnum m_RenderSource;
    vk::ImageLayout m_SwapchainImageLayout;
    vk::SurfaceKHR m_Surface;
    bool m_DoesSwapchainSupportBlitting;
    vk::SurfaceFormatKHR m_SwapchainColorFormat;
    vk::Format m_SwapchainDepthFormat;
    vk::PresentModeKHR m_SwapchainPresentationMode;
    vk::RenderPass m_RenderPass;
    vk::Extent2D m_SwapchainExtent;
    vk::SwapchainKHR m_Swapchain;
    std::vector<vk::Image> m_SwapchainColorImages;
    std::vector<vk::ImageView> m_SwapchainColorImageViews;
    std::vector<AxrVulkanImage> m_SwapchainDepthImages;
    std::vector<AxrVulkanImage> m_SwapchainMsaaImages;
    std::vector<vk::Framebuffer> m_SwapchainFramebuffers;
    /// One command buffer per frame in flight
    std::vector<vk::CommandBuffer> m_RenderingCommandBuffers;
    /// One semaphore per frame in flight
    std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
    /// One semaphore per frame in flight
    std::vector<vk::Semaphore> m_RenderingFinishedSemaphores;
    /// One fence per frame in flight
    std::vector<vk::Fence> m_RenderingFences;
    bool m_IsReady;
    uint32_t m_CurrentImageIndex;
    uint32_t m_CurrentFrame;
    bool m_IsSwapchainOutOfDate;
    vk::SampleCountFlagBits m_MsaaSampleCount;

    RenderData m_FrameRenderData;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Set up the window graphics
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupWindowGraphics();
    /// Reset the setupWindowGraphics() function 
    void resetSetupWindowGraphics();

    /// Get the camera components
    /// @param transform Camera transform component
    /// @param camera Camera component
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult getCameraComponents(
        AxrTransformComponent& transform,
        AxrCameraComponent& camera
    ) const;

    // ---- Surface ----

    /// Create the vulkan surface
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createSurface();
    /// Destroy the vulkan surface
    void destroySurface();

#ifdef AXR_USE_PLATFORM_WIN32
    /// Create the vulkan win32 surface
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createWin32Surface();
#endif

    // ---- Swapchain ----

    /// Set up swapchain related data
    /// @param surfaceDetails Surface details to use
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupSwapchain(const AxrVulkanSurfaceDetails& surfaceDetails);
    /// Reset the setupSwapchain() function
    void resetSetupSwapchain();

    /// Recreate the swapchain and it's related data
    /// @returns AXR_SUCCESS if the function succeeded.
    /// @returns AXR_DONT_RENDER if we should skip rendering this frame.
    [[nodiscard]] AxrResult recreateSwapchain();

    /// Set the swapchain color and depth format options
    /// @param physicalDevice Physical device to use
    /// @param swapchainColorFormatOptions Swapchain color format options to choose from
    /// @param swapchainDepthFormatOptions Swapchain depth format options to choose from
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setSwapchainFormatOptions(
        const vk::PhysicalDevice& physicalDevice,
        const std::vector<vk::SurfaceFormatKHR>& swapchainColorFormatOptions,
        const std::vector<vk::Format>& swapchainDepthFormatOptions
    );
    /// Reset the setSwapchainFormatOptions() function 
    void resetSwapchainFormatOptions();

    /// Set the swapchain color and depth formats
    /// @param surfaceFormats Collection of surface formats that are available to us
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setSwapchainFormats(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats);
    /// Reset the setSwapchainFormats() function
    void resetSwapchainFormats();

    /// Set the swapchain presentation mode
    /// @param surfacePresentationModes Collection of surface presentation modes that are available to use
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setSwapchainPresentationMode(
        const std::vector<vk::PresentModeKHR>& surfacePresentationModes
    );
    /// Reset the setSwapchainPresentationMode() function
    void resetSwapchainPresentationMode();

    /// Set the swapchain extent
    /// @param surfaceCapabilities Surface capabilities
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setSwapchainExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities);
    /// Reset the setSwapchainExtent() function
    void resetSwapchainExtent();

    /// Create the vulkan swapchain
    /// @param surfaceCapabilities Surface capabilities
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createSwapchain(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities);
    /// Destroy the createSwapchain() function
    void destroySwapchain();

    /// Get the swapchain images
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult getSwapchainImages();
    /// Reset the getSwapchainImages() function
    void resetSwapchainImages();

    // ---- Render pass ----

    /// Create the render pass
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createRenderPass();
    /// Destroy the render pass
    void destroyRenderPass();

    // ---- Sync Objects ----

    /// Create the rendering sync objects
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createSyncObjects();
    /// Destroy the rendering sync objects
    void destroySyncObjects();

    // ---- Command Buffers ----

    /// Create command buffers
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createCommandBuffers();
    /// Destroy command buffers
    void destroyCommandBuffers();

    // ---- Framebuffer ----

    /// Create the swapchain framebuffers
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createFramebuffers();
    /// Destroy the swapchain framebuffers
    void destroyFramebuffers();

    // ---- Depth Buffer ----

    /// Create the depth buffer images
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createDepthBufferImages();
    /// Destroy the depth buffer images
    void destroyDepthBufferImages();

    // ---- Msaa ----

    /// Set the msaa sample count
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setMsaaSampleCount();
    /// Reset the msaa sample count
    void resetMsaaSampleCount();

    /// Create the msaa images
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createMsaaImages();
    /// Destroy the msaa images
    void destroyMsaaImages();

    // ---- Callbacks ----

    /// 'On window open state changed' callback function
    /// @param isWindowOpen If true, the window is open. If false, the window is closed.
    /// @returns AXR_SUCCESS if the function succeeded
    AxrResult onWindowOpenStateChangedCallback(bool isWindowOpen);

    /// 'On window resized' callback function
    /// @param width New window width
    /// @param height New window height
    void onWindowResizedCallback(uint32_t width, uint32_t height);
};


#endif
