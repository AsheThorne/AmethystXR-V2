#pragma once
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "../../xrSystem/xrSystem.hpp"
#include "vulkanQueueFamilies.hpp"
#include "sceneData/vulkanLoadedScenesCollection.hpp"
#include "../graphicsCommonStructs.hpp"

// ----------------------------------------- //
// Vulkan Headers
// ----------------------------------------- //
#include <vulkan/vulkan.hpp>

/// Vulkan Xr Graphics
class AxrVulkanXrGraphics {
public:
    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// AxrVulkanXrGraphics config
    struct Config {
        AxrXrSystem& XrSystem;
        vk::DispatchLoaderDynamic& Dispatch;
        AxrVulkanLoadedScenesCollection& LoadedScenes;
        uint32_t MaxFramesInFlight;
        AxrMsaaSampleCountEnum MaxMsaaSampleCount;
    };

    /// AxrVulkanXrGraphics Setup Config
    struct SetupConfig {
        vk::Instance Instance;
        vk::PhysicalDevice PhysicalDevice;
        vk::Device Device;
        vk::CommandPool GraphicsCommandPool;
        const AxrVulkanQueueFamilies& QueueFamilies;
        /// Ordered from most desired to the least desired
        const std::vector<vk::Format>& SwapchainColorFormatOptions;
        /// Ordered from most desired to the least desired
        const std::vector<vk::Format>& SwapchainDepthFormatOptions;
    };

    // Per view data
    struct View {
        struct SwapchainData {
            XrSwapchain Swapchain = XR_NULL_HANDLE;
            uint32_t AcquiredImageIndex = 0;
            std::vector<vk::Image> Images;
            std::vector<vk::ImageView> ImageViews;
        };

        /// One semaphore per frame in flight
        std::vector<vk::Semaphore> RenderingFinishedSemaphores;
        /// One fence per frame in flight
        std::vector<vk::Fence> RenderingFences;
        /// One command buffer per frame in flight
        std::vector<vk::CommandBuffer> RenderingCommandBuffers;
        vk::Extent2D SwapchainExtent;
        SwapchainData ColorSwapchain;
        std::vector<AxrVulkanImage> SwapchainDepthImages;
        std::vector<AxrVulkanImage> SwapchainMsaaImages;
        std::vector<vk::Framebuffer> SwapchainFramebuffers;
        XrFovf CachedFov;
        float WidthOverlap;
    };

    // Render data for each frame
    struct RenderData {
        XrTime PredictedDisplayTime;
        std::vector<XrCompositionLayerProjectionView> CompositionLayerViews;

        void reset() {
            PredictedDisplayTime = 0;
            CompositionLayerViews.clear();
        }
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Constructor
    /// @param config Xr graphics config
    AxrVulkanXrGraphics(const Config& config);
    /// Copy Constructor
    /// @param src Source AxrVulkanXrGraphics to copy from
    AxrVulkanXrGraphics(const AxrVulkanXrGraphics& src) = delete;
    /// Move Constructor
    /// @param src Source AxrVulkanXrGraphics to move from
    AxrVulkanXrGraphics(AxrVulkanXrGraphics&& src) noexcept = delete;

    // ---- Destructor ----

    /// Destructor
    ~AxrVulkanXrGraphics();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrVulkanXrGraphics to copy from
    AxrVulkanXrGraphics& operator=(const AxrVulkanXrGraphics& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrVulkanXrGraphics to move from
    AxrVulkanXrGraphics& operator=(AxrVulkanXrGraphics&& src) noexcept = delete;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Set the clear color
    /// @param color Clear color
    void setClearColor(const AxrColor& color);

    /// Set up vulkan xr graphics
    /// @param config Setup config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setup(const SetupConfig& config);
    /// Reset the setup() function
    void resetSetup();

    /// Check if the xr session graphics are ready for rendering
    /// @returns True if the xr session graphics are ready for rendering
    [[nodiscard]] bool isReady() const;

    /// Create the vulkan instance to use.
    /// @param createInfo The VkInstanceCreateInfo
    /// @param vkInstance Output created vkInstance
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createVulkanInstance(
        const vk::InstanceCreateInfo& createInfo,
        vk::Instance& vkInstance
    ) const;

    /// Get the vulkan physical device to use
    /// @param vkInstance The vkInstance to use
    /// @param vkPhysicalDevice The Output VkPhysicalDevice
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult getVulkanPhysicalDevice(
        vk::Instance vkInstance,
        vk::PhysicalDevice& vkPhysicalDevice
    ) const;

    /// Create the vulkan device to use
    /// @param vkPhysicalDevice VkPhysicalDevice to use
    /// @param createInfo The VkDeviceCreateInfo
    /// @param vkDevice Output created vkDevice
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createVulkanDevice(
        vk::PhysicalDevice vkPhysicalDevice,
        const vk::DeviceCreateInfo& createInfo,
        vk::Device& vkDevice
    ) const;

    /// Begin rendering
    /// @param sceneData Active scene data
    /// @returns AXR_SUCCESS if the function succeeded
    /// @returns AXR_DONT_RENDER if we should skip rendering this frame.
    [[nodiscard]] AxrResult beginRendering(const AxrVulkanSceneData* sceneData);
    /// End rendering
    /// @returns AXR_SUCCESS if the function succeeded
    AxrResult endRendering();
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
    /// Present the current frame to the xr device
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
    AxrXrSystem& m_XrSystem;
    vk::DispatchLoaderDynamic& m_Dispatch;
    AxrVulkanLoadedScenesCollection& m_LoadedScenes;
    uint32_t m_MaxFramesInFlight;
    AxrMsaaSampleCountEnum m_MaxMsaaSampleCount;

    // ---- Setup Config ----
    vk::Instance m_Instance;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_Device;
    vk::CommandPool m_GraphicsCommandPool;
    AxrVulkanQueueFamilies m_QueueFamilies;
    /// Ordered from most desired to the least desired
    std::vector<vk::Format> m_SwapchainColorFormatOptions;
    /// Ordered from most desired to the least desired
    std::vector<vk::Format> m_SwapchainDepthFormatOptions;

    // ---- Data ----
    AxrColor m_ClearColor;
    bool m_IsReady;
    vk::Format m_SwapchainColorFormat;
    vk::Format m_SwapchainDepthFormat;
    vk::ImageLayout m_SwapchainImageLayout;
    vk::RenderPass m_RenderPass;
    std::vector<View> m_Views;
    uint32_t m_CurrentFrame;
    vk::SampleCountFlagBits m_MsaaSampleCount;
    vk::Extent2D m_ViewableRegionExtent;

    RenderData m_FrameRenderData;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Set the xr session graphics binding
    void setXrGraphicsBinding() const;
    /// Reset setXrGraphicsBinding()
    void resetXrGraphicsBinding() const;

    /// Set up the xr session graphics
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupXrSessionGraphics();
    /// Reset the setupXrSessionGraphics() function 
    void resetSetupXrSessionGraphics();

    /// Set the viewable region extent
    /// @param xrViews Xr views to use
    void setViewableRegionExtent(const std::vector<XrView>& xrViews);

    // ---- Swapchain ----

    /// Set the swapchain color and depth format options
    /// @param physicalDevice Physical device to use
    /// @param swapchainColorFormatOptions Swapchain color format options to choose from
    /// @param swapchainDepthFormatOptions Swapchain depth format options to choose from
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setSwapchainFormatOptions(
        const vk::PhysicalDevice& physicalDevice,
        const std::vector<vk::Format>& swapchainColorFormatOptions,
        const std::vector<vk::Format>& swapchainDepthFormatOptions
    );
    /// Reset the setSwapchainFormatOptions() function 
    void resetSwapchainFormatOptions();

    /// Set the swapchain color and depth formats
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setSwapchainFormats();
    /// Reset the setSwapchainFormats() function
    void resetSwapchainFormats();

    /// Set up swapchain related data for the given view
    /// @param viewConfiguration Xr view configuration
    /// @param view Output view graphics data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupSwapchain(const XrViewConfigurationView& viewConfiguration, View& view) const;
    /// Reset the setupSwapchain() function for the given view
    /// @param view View data
    void resetSetupSwapchain(View& view) const;

    /// Set the swapchain extent for the given view
    /// @param viewConfiguration Xr view configuration
    /// @param view Output view graphics data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setSwapchainExtent(const XrViewConfigurationView& viewConfiguration, View& view) const;
    /// Reset the setSwapchainExtent() function for the given view
    /// @param view View data
    void resetSwapchainExtent(View& view) const;

    /// Create the swapchain for the given view
    /// @param view Output view graphics data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createSwapchain(View& view) const;
    /// Destroy the swapchain for the given view
    /// @param view View data
    void destroySwapchain(View& view) const;

    /// Set up swapchain images
    /// @param swapchain Swapchain to use
    /// @param imageAspectFlags Image aspect flags
    /// @param imageFormat Image format
    /// @param images Output Images
    /// @param imageViews Output image views
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupSwapchainImages(
        XrSwapchain swapchain,
        vk::ImageAspectFlags imageAspectFlags,
        vk::Format imageFormat,
        std::vector<vk::Image>& images,
        std::vector<vk::ImageView>& imageViews
    ) const;
    /// Reset setupSwapchainImages()
    /// @param images Swapchain images
    /// @param imageViews Swapchain image views
    void resetSetupSwapchainImages(
        std::vector<vk::Image>& images,
        std::vector<vk::ImageView>& imageViews
    ) const;

    // ---- Depth Buffer ----

    /// Create the depth buffer images
    /// @param view Output view graphics data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createDepthBufferImages(View& view) const;
    /// Destroy the depth buffer images
    /// @param view View data
    void destroyDepthBufferImages(View& view) const;

    // ---- Render pass ----

    /// Create the render pass
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createRenderPass();
    /// Destroy the render pass
    void destroyRenderPass();

    // ---- View ----

    /// Set up all view related data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupAllViews();
    /// Reset setupViews();
    void resetSetupAllViews();

    /// Set up the given view
    /// @param viewConfiguration Xr view configuration
    /// @param view Output view graphics data
    [[nodiscard]] AxrResult setupView(const XrViewConfigurationView& viewConfiguration, View& view) const;
    /// Reset the given view
    /// @param view View data
    void resetSetupView(View& view) const;

    // ---- Sync Objects ----

    /// Create the rendering sync objects for the given view
    /// @param view Output view graphics data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createSyncObjects(View& view) const;
    /// Destroy the rendering sync objects for the given view
    /// @param view View data
    void destroySyncObjects(View& view) const;

    // ---- Command Buffers ----

    /// Create command buffers for the given view
    /// @param view Output view graphics data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createCommandBuffers(View& view) const;
    /// Destroy command buffers for the given view
    /// @param view View data
    void destroyCommandBuffers(View& view) const;

    // ---- Framebuffer ----

    /// Create the swapchain framebuffers for the given view
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createFramebuffers(View& view) const;
    /// Destroy the swapchain framebuffers for the given view
    void destroyFramebuffers(View& view) const;

    // ---- Matrix ----

    /// Create the vulkan Xr session projection matrix
    /// @param fov Xr session fov
    /// @param nearClip Near clipping plane
    /// @param farClip Far clipping plane
    /// @returns The projection matrix
    [[nodiscard]] glm::mat4 createProjectionMatrix(const AxrCameraFov& fov, float nearClip, float farClip) const;

    // ---- Msaa ----

    /// Set the msaa sample count
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setMsaaSampleCount();
    /// Reset the msaa sample count
    void resetMsaaSampleCount();

    /// Create the msaa images
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createMsaaImages(View& view) const;
    /// Destroy the msaa images
    void destroyMsaaImages(View& view) const;

    // ---- Callbacks ----

    /// 'On xr session state changed' callback function
    /// @param isSessionRunning If true, the xr session is running. If false, the xr session is not running.
    /// @returns AXR_SUCCESS if the function succeeded
    AxrResult onXrSessionStateChangedCallback(bool isSessionRunning);
};

#endif
