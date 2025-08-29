#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "vulkanXrGraphics.hpp"
#include "vulkanImage.hpp"
#include "vulkanUtils.hpp"
#include "vulkanSharedFunctions.hpp"
#include "../../scene/sceneUtils.hpp"

// ---- Special Functions ----

AxrVulkanXrGraphics::AxrVulkanXrGraphics(const Config& config):
    m_XrSystem(config.XrSystem),
    m_Dispatch(config.Dispatch),
    m_LoadedScenes(config.LoadedScenes),
    m_MaxFramesInFlight(config.MaxFramesInFlight),
    m_MaxMsaaSampleCount(config.MaxMsaaSampleCount),
    m_Instance(VK_NULL_HANDLE),
    m_PhysicalDevice(VK_NULL_HANDLE),
    m_Device(VK_NULL_HANDLE),
    m_GraphicsCommandPool(VK_NULL_HANDLE),
    m_ClearColor(AxrColor(0.0f, 0.0f, 0.0f, 1.0f)),
    m_IsReady(false),
    m_SwapchainColorFormat(vk::Format::eUndefined),
    m_SwapchainDepthFormat(vk::Format::eUndefined),
    m_SwapchainImageLayout(vk::ImageLayout::eColorAttachmentOptimal),
    m_RenderPass(VK_NULL_HANDLE),
    m_CurrentFrame(0),
    m_MsaaSampleCount(vk::SampleCountFlagBits::e1),
    m_ViewableRegionExtent(vk::Extent2D(0, 0)),
    m_FrameRenderData() {
}

AxrVulkanXrGraphics::~AxrVulkanXrGraphics() {
    resetSetup();
}

// ---- Public Functions ----

void AxrVulkanXrGraphics::setClearColor(const AxrColor& color) {
    m_ClearColor = color;
}

AxrResult AxrVulkanXrGraphics::setup(const SetupConfig& config) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance != VK_NULL_HANDLE) {
        axrLogErrorLocation("Instance isn't null.");
        return AXR_ERROR;
    }

    if (config.Instance == VK_NULL_HANDLE) {
        axrLogErrorLocation("Config instance is null.");
        return AXR_ERROR;
    }

    if (m_PhysicalDevice != VK_NULL_HANDLE) {
        axrLogErrorLocation("Physical device isn't null.");
        return AXR_ERROR;
    }

    if (config.PhysicalDevice == VK_NULL_HANDLE) {
        axrLogErrorLocation("Config physical device is null.");
        return AXR_ERROR;
    }

    if (m_GraphicsCommandPool != VK_NULL_HANDLE) {
        axrLogErrorLocation("Graphics command pool isn't null.");
        return AXR_ERROR;
    }

    if (config.GraphicsCommandPool == VK_NULL_HANDLE) {
        axrLogErrorLocation("Config graphics command pool is null.");
        return AXR_ERROR;
    }

    if (m_Device != VK_NULL_HANDLE) {
        axrLogErrorLocation("Logical device isn't null.");
        return AXR_ERROR;
    }

    if (config.Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Config logical device is null.");
        return AXR_ERROR;
    }

    if (m_QueueFamilies.isValid()) {
        axrLogErrorLocation("Queue families are already set.");
        return AXR_ERROR;
    }

    if (!config.QueueFamilies.isValid()) {
        axrLogErrorLocation("Config queue families aren't valid.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    m_Instance = config.Instance;
    m_PhysicalDevice = config.PhysicalDevice;
    m_Device = config.Device;
    m_GraphicsCommandPool = config.GraphicsCommandPool;
    m_QueueFamilies = config.QueueFamilies;

    const AxrResult axrResult = setSwapchainFormatOptions(
        config.PhysicalDevice,
        config.SwapchainColorFormatOptions,
        config.SwapchainDepthFormatOptions
    );
    if (AXR_FAILED(axrResult)) {
        resetSetup();
        return axrResult;
    }

    setXrGraphicsBinding();

    m_XrSystem.OnXrSessionStateChangedCallbackGraphics
              .connect<&AxrVulkanXrGraphics::onXrSessionStateChangedCallback>(this);

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::resetSetup() {
    resetSetupXrSessionGraphics();

    m_XrSystem.OnXrSessionStateChangedCallbackGraphics.reset();
    resetXrGraphicsBinding();
    resetSwapchainFormatOptions();

    m_Instance = VK_NULL_HANDLE;
    m_PhysicalDevice = VK_NULL_HANDLE;
    m_Device = VK_NULL_HANDLE;
    m_GraphicsCommandPool = VK_NULL_HANDLE;
    m_QueueFamilies.reset();
}

bool AxrVulkanXrGraphics::isReady() const {
    return m_IsReady;
}

AxrResult AxrVulkanXrGraphics::createVulkanInstance(
    const vk::InstanceCreateInfo& createInfo,
    vk::Instance& vkInstance
) const {
    return m_XrSystem.createVulkanInstance(
        vkGetInstanceProcAddr,
        createInfo,
        reinterpret_cast<VkInstance&>(vkInstance)
    );
}

AxrResult AxrVulkanXrGraphics::getVulkanPhysicalDevice(
    const vk::Instance vkInstance,
    vk::PhysicalDevice& vkPhysicalDevice
) const {
    return m_XrSystem.getVulkanPhysicalDevice(vkInstance, reinterpret_cast<VkPhysicalDevice&>(vkPhysicalDevice));
}

AxrResult AxrVulkanXrGraphics::createVulkanDevice(
    const vk::PhysicalDevice vkPhysicalDevice,
    const vk::DeviceCreateInfo& createInfo,
    vk::Device& vkDevice
) const {
    return m_XrSystem.createVulkanDevice(
        vkGetInstanceProcAddr,
        vkPhysicalDevice,
        createInfo,
        reinterpret_cast<VkDevice&>(vkDevice)
    );
}

AxrResult AxrVulkanXrGraphics::beginRendering(const AxrVulkanSceneData* sceneData) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Views.empty()) {
        axrLogErrorLocation("Views are empty.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = m_XrSystem.beginFrame(m_FrameRenderData.PredictedDisplayTime);
    if (AXR_FAILED(axrResult)) return axrResult;

    std::vector<XrView> xrViews;
    axrResult = m_XrSystem.locateViews(m_FrameRenderData.PredictedDisplayTime, xrViews);
    if (AXR_FAILED(axrResult)) {
        endRendering();
        return axrResult;
    }

    setViewableRegionExtent(xrViews);

    m_FrameRenderData.CompositionLayerViews.resize(xrViews.size());
    for (size_t i = 0; i < m_FrameRenderData.CompositionLayerViews.size(); ++i) {
        m_FrameRenderData.CompositionLayerViews[i].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
        m_FrameRenderData.CompositionLayerViews[i].fov = xrViews[i].fov;
        m_FrameRenderData.CompositionLayerViews[i].pose = xrViews[i].pose;
        m_FrameRenderData.CompositionLayerViews[i].subImage.swapchain = m_Views[i].ColorSwapchain.Swapchain;
        m_FrameRenderData.CompositionLayerViews[i].subImage.imageArrayIndex = 0;
        m_FrameRenderData.CompositionLayerViews[i].subImage.imageRect = XrRect2Di{
            .offset = XrOffset2Di{
                .x = 0,
                .y = 0,
            },
            .extent = XrExtent2Di{
                .width = static_cast<int32_t>(m_Views[i].SwapchainExtent.width),
                .height = static_cast<int32_t>(m_Views[i].SwapchainExtent.height),
            },
        };
    }

    axrResult = setCameraInfo();
    if (AXR_FAILED(axrResult)) {
        endRendering();
        return axrResult;
    }

    // We don't really care about the result of this
    axrResult = m_XrSystem.updatePoseActions(m_FrameRenderData.PredictedDisplayTime, sceneData->getEcsRegistryHandle());

    return AXR_SUCCESS;
}

AxrResult AxrVulkanXrGraphics::endRendering() {
    const AxrResult axrResult = m_XrSystem.endFrame(
        m_FrameRenderData.PredictedDisplayTime,
        m_FrameRenderData.CompositionLayerViews
    );
    if (AXR_FAILED(axrResult)) return axrResult;

    m_FrameRenderData.reset();
    return AXR_SUCCESS;
}

uint32_t AxrVulkanXrGraphics::getViewCount() const {
    return static_cast<uint32_t>(m_Views.size());
}

AxrPlatformType AxrVulkanXrGraphics::getPlatformType() const {
    return AXR_PLATFORM_TYPE_XR_DEVICE;
}

vk::Extent2D AxrVulkanXrGraphics::getUIRegion() const {
    return m_ViewableRegionExtent;
}

vk::RenderPass AxrVulkanXrGraphics::getRenderPass() const {
    return m_RenderPass;
}

vk::Framebuffer AxrVulkanXrGraphics::getFramebuffer(const uint32_t viewIndex) const {
    if (viewIndex > m_Views.size() - 1) {
        axrLogErrorLocation("View index out of bounds.");
        return VK_NULL_HANDLE;
    }

    return m_Views[viewIndex].SwapchainFramebuffers[m_Views[viewIndex].ColorSwapchain.AcquiredImageIndex];
}

vk::Extent2D AxrVulkanXrGraphics::getSwapchainExtent(const uint32_t viewIndex) const {
    if (viewIndex > m_Views.size() - 1) {
        axrLogErrorLocation("View index out of bounds.");
        return {};
    }

    return m_Views[viewIndex].SwapchainExtent;
}

vk::ClearColorValue AxrVulkanXrGraphics::getClearColorValue() const {
    if (m_XrSystem.getEnvironmentBlendMode() == XR_ENVIRONMENT_BLEND_MODE_ADDITIVE) {
        // The clear color needs to be black for XR_ENVIRONMENT_BLEND_MODE_ADDITIVE to work properly
        return vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f};
    }

    return vk::ClearColorValue{m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a};
}

vk::CommandBuffer AxrVulkanXrGraphics::getRenderingCommandBuffer(const uint32_t viewIndex) const {
    if (viewIndex > m_Views.size() - 1) {
        axrLogErrorLocation("View index out of bounds.");
        return VK_NULL_HANDLE;
    }

    return m_Views[viewIndex].RenderingCommandBuffers[m_CurrentFrame];
}

std::vector<vk::Semaphore> AxrVulkanXrGraphics::getRenderingWaitSemaphores(const uint32_t viewIndex) const {
    // Nothing here
    return {};
}

std::vector<vk::PipelineStageFlags> AxrVulkanXrGraphics::getRenderingWaitStages(const uint32_t viewIndex) const {
    // Nothing here
    return {};
}

std::vector<vk::Semaphore> AxrVulkanXrGraphics::getRenderingSignalSemaphores(const uint32_t viewIndex) const {
    return {};
}

vk::Fence AxrVulkanXrGraphics::getRenderingFence(const uint32_t viewIndex) const {
    if (viewIndex > m_Views.size() - 1) {
        axrLogErrorLocation("View index out of bounds.");
        return VK_NULL_HANDLE;
    }

    return m_Views[viewIndex].RenderingFences[m_CurrentFrame];
}

uint32_t AxrVulkanXrGraphics::getCurrentRenderingFrame() const {
    return m_CurrentFrame;
}

vk::Image AxrVulkanXrGraphics::getSwapchainImage(const uint32_t viewIndex) const {
    if (viewIndex > m_Views.size() - 1) {
        axrLogErrorLocation("View index out of bounds.");
        return VK_NULL_HANDLE;
    }

    const View::SwapchainData& swapchain = m_Views[viewIndex].ColorSwapchain;
    return swapchain.Images[swapchain.AcquiredImageIndex];
}

vk::Format AxrVulkanXrGraphics::getSwapchainImageFormat() const {
    return m_SwapchainColorFormat;
}

vk::ImageLayout AxrVulkanXrGraphics::getSwapchainImageLayout() const {
    return m_SwapchainImageLayout;
}

AxrResult AxrVulkanXrGraphics::acquireNextSwapchainImage(const uint32_t viewIndex) {
    if (viewIndex > m_Views.size() - 1) {
        axrLogErrorLocation("View index out of bounds.");
        return AXR_ERROR;
    }

    return m_XrSystem.acquireSwapchainImage(
        m_Views[viewIndex].ColorSwapchain.Swapchain,
        m_Views[viewIndex].ColorSwapchain.AcquiredImageIndex
    );
}

AxrResult AxrVulkanXrGraphics::presentFrame(const uint32_t viewIndex) {
    if (viewIndex > m_Views.size() - 1) {
        axrLogErrorLocation("View index out of bounds.");
        return AXR_ERROR;
    }

    const AxrResult axrResult = m_XrSystem.releaseSwapchainImage(m_Views[viewIndex].ColorSwapchain.Swapchain);
    if (AXR_FAILED(axrResult)) return axrResult;

    m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFramesInFlight;

    return AXR_SUCCESS;
}

AxrResult AxrVulkanXrGraphics::getCameraInfo(
    const uint32_t viewIndex,
    AxrCameraInfo& cameraInfo
) const {
    if (viewIndex > m_FrameRenderData.CameraInfos.size() - 1) {
        axrLogErrorLocation("View index out of bounds.");
        return AXR_ERROR;
    }

    cameraInfo = m_FrameRenderData.CameraInfos[viewIndex];
    return AXR_SUCCESS;
}

// ---- Private Functions ----

void AxrVulkanXrGraphics::setXrGraphicsBinding() const {
    if (!m_QueueFamilies.GraphicsQueueFamilyIndex.has_value()) {
        axrLogErrorLocation("Graphics queue family index is null.");
        return;
    }

    const XrGraphicsBindingVulkan2KHR graphicsBinding{
        .type = XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR,
        .next = nullptr,
        .instance = m_Instance,
        .physicalDevice = m_PhysicalDevice,
        .device = m_Device,
        .queueFamilyIndex = m_QueueFamilies.GraphicsQueueFamilyIndex.value(),
        .queueIndex = 0
    };
    m_XrSystem.setGraphicsBinding(graphicsBinding);
}

void AxrVulkanXrGraphics::resetXrGraphicsBinding() const {
    m_XrSystem.resetGraphicsBinding();
}

AxrResult AxrVulkanXrGraphics::setupXrSessionGraphics() {
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = setSwapchainFormats();
    if (AXR_FAILED(axrResult)) {
        resetSetupXrSessionGraphics();
        return axrResult;
    }

    axrResult = setMsaaSampleCount();
    if (AXR_FAILED(axrResult)) {
        resetSetupXrSessionGraphics();
        return axrResult;
    }

    axrResult = createRenderPass();
    if (AXR_FAILED(axrResult)) {
        resetSetupXrSessionGraphics();
        return axrResult;
    }

    axrResult = setupAllViews();
    if (AXR_FAILED(axrResult)) {
        resetSetupXrSessionGraphics();
        return axrResult;
    }

    axrResult = m_LoadedScenes.setupXrSessionData(
        m_RenderPass,
        m_MsaaSampleCount,
        static_cast<uint32_t>(m_Views.size())
    );
    if (AXR_FAILED(axrResult)) {
        resetSetupXrSessionGraphics();
        return axrResult;
    }

    m_IsReady = true;
    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::resetSetupXrSessionGraphics() {
    m_IsReady = false;

    m_LoadedScenes.resetSetupXrSessionData();
    resetSetupAllViews();
    destroyRenderPass();
    resetMsaaSampleCount();
    resetSwapchainFormats();
}

AxrResult AxrVulkanXrGraphics::setCameraInfo() {
    if (!m_FrameRenderData.CameraInfos.empty()) {
        axrLogErrorLocation("Camera info already set.");
        return AXR_ERROR;
    }
    
    m_FrameRenderData.CameraInfos.resize(m_FrameRenderData.CompositionLayerViews.size());
    for (size_t viewIndex = 0; viewIndex < m_FrameRenderData.CameraInfos.size(); ++viewIndex) {
        const XrCompositionLayerProjectionView& compositionLayer = m_FrameRenderData.CompositionLayerViews[viewIndex];
        const View& view = m_Views[viewIndex];

        const auto position = glm::vec3(
            compositionLayer.pose.position.x,
            compositionLayer.pose.position.y,
            compositionLayer.pose.position.z
        );

        const auto orientation = glm::quat(
            compositionLayer.pose.orientation.w,
            compositionLayer.pose.orientation.x,
            compositionLayer.pose.orientation.y,
            compositionLayer.pose.orientation.z
        );

        const auto fov = AxrCameraFov{
            .Up = compositionLayer.fov.angleUp,
            .Down = compositionLayer.fov.angleDown,
            .Left = compositionLayer.fov.angleLeft,
            .Right = compositionLayer.fov.angleRight,
        };

        const auto zNear = m_XrSystem.getNearClippingPlane();
        const auto zFar = m_XrSystem.getFarClippingPlane();

        m_FrameRenderData.CameraInfos[viewIndex] = AxrCameraInfo{
            .Position = position,
            .Orientation = orientation,
            .Fov = fov,
            .ViewMatrix = glm::inverse(
                glm::translate(glm::mat4(1.0f), position) *
                glm::toMat4(orientation)
            ),
            .ProjectionMatrix = createProjectionMatrix(
                fov,
                zNear,
                zFar
            ),
            .PixelWidth = static_cast<float>(view.SwapchainExtent.width),
            .PixelHeight = static_cast<float>(view.SwapchainExtent.height),
            .AspectRatio = static_cast<float>(view.SwapchainExtent.width) / static_cast<float>(view.SwapchainExtent.
                height),
            .ZNear = zNear,
            .ZFar = zFar,
        };
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::setViewableRegionExtent(const std::vector<XrView>& xrViews) {
    switch (m_XrSystem.getViewConfigurationType()) {
        case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO: {
            if (xrViews.size() != 1 || m_Views.size() != 1) {
                axrLogErrorLocation("Views aren't a size of 1.");
                break;
            }

            // Viewable region won't change so we only need to set it if it doesn't already have a value
            if (m_ViewableRegionExtent != vk::Extent2D(0, 0)) break;

            // We don't use this, but we'll still set it anyway for completeness.
            m_Views[0].WidthOverlap = m_Views[0].SwapchainExtent.width;

            m_ViewableRegionExtent = vk::Extent2D(
                m_Views[0].SwapchainExtent.width,
                m_Views[0].SwapchainExtent.height
            );

            // We don't use this, but we'll still set it anyway for completeness.
            m_Views[0].CachedFov = xrViews[0].fov;
            break;
        }
        case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO: {
            if (xrViews.size() != 2 || m_Views.size() != 2) {
                axrLogErrorLocation("Views aren't a size of 2.");
                break;
            }

            // Only update if the values have changed
            if (m_Views[0].CachedFov == xrViews[0].fov && m_Views[1].CachedFov == xrViews[1].fov) break;

            const float leftEyeFov = std::abs(xrViews[0].fov.angleLeft) + std::abs(xrViews[0].fov.angleRight);
            const float rightEyeFov = std::abs(xrViews[1].fov.angleLeft) + std::abs(xrViews[1].fov.angleRight);
            const float stereoFov = std::abs(xrViews[0].fov.angleLeft) + std::abs(xrViews[1].fov.angleRight);
            const float overlapAngle = (leftEyeFov + rightEyeFov) - stereoFov;

            const float pixelsPerRadianLeft = m_Views[0].SwapchainExtent.width / leftEyeFov;
            const float pixelsPerRadianRight = m_Views[1].SwapchainExtent.width / rightEyeFov;
            const float overlapPixelsLeft = overlapAngle / 2 * pixelsPerRadianLeft;
            const float overlapPixelsRight = overlapAngle / 2 * pixelsPerRadianRight;
            const float combinedStereoWidth = m_Views[0].SwapchainExtent.width + m_Views[1].SwapchainExtent.width - (
                overlapPixelsLeft + overlapPixelsRight);

            m_Views[0].WidthOverlap = overlapPixelsLeft;
            m_Views[1].WidthOverlap = overlapPixelsRight;
            m_ViewableRegionExtent = vk::Extent2D(
                std::ceil(combinedStereoWidth),
                m_Views[0].SwapchainExtent.height
            );

            m_Views[0].CachedFov = xrViews[0].fov;
            m_Views[1].CachedFov = xrViews[1].fov;
            break;
        }
        case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_QUAD_VARJO:
        case XR_VIEW_CONFIGURATION_TYPE_SECONDARY_MONO_FIRST_PERSON_OBSERVER_MSFT:
        case XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM:
        default: {
            axrLogError("Unsupported view configuration type.");
            break;
        }
    }
}

AxrResult AxrVulkanXrGraphics::setSwapchainFormatOptions(
    const vk::PhysicalDevice& physicalDevice,
    const std::vector<vk::Format>& swapchainColorFormatOptions,
    const std::vector<vk::Format>& swapchainDepthFormatOptions
) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_SwapchainColorFormatOptions.empty()) {
        axrLogErrorLocation("Swapchain color format options aren't empty.");
        return AXR_ERROR;
    }

    if (!m_SwapchainDepthFormatOptions.empty()) {
        axrLogErrorLocation("Swapchain depth format options aren't empty.");
        return AXR_ERROR;
    }

    if (swapchainColorFormatOptions.empty()) {
        axrLogErrorLocation("Swapchain color format options are empty.");
        return AXR_ERROR;
    }

    if (swapchainDepthFormatOptions.empty()) {
        axrLogErrorLocation("Swapchain depth format options are empty.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    for (const vk::Format format : swapchainColorFormatOptions) {
        if (axrAreFormatFeaturesSupported(
            format,
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eSampledImage &
            vk::FormatFeatureFlagBits::eColorAttachment,
            physicalDevice,
            m_Dispatch
        )) {
            m_SwapchainColorFormatOptions.push_back(format);
        }
    }

    for (const vk::Format format : swapchainDepthFormatOptions) {
        if (axrAreFormatFeaturesSupported(
            format,
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eSampledImage &
            vk::FormatFeatureFlagBits::eDepthStencilAttachment,
            physicalDevice,
            m_Dispatch
        )) {
            m_SwapchainDepthFormatOptions.push_back(format);
        }
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::resetSwapchainFormatOptions() {
    m_SwapchainColorFormatOptions.clear();
    m_SwapchainDepthFormatOptions.clear();
}

AxrResult AxrVulkanXrGraphics::setSwapchainFormats() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_SwapchainColorFormat != vk::Format::eUndefined) {
        axrLogErrorLocation("Swapchain color format has already been set.");
        return AXR_ERROR;
    }

    if (m_SwapchainDepthFormat != vk::Format::eUndefined) {
        axrLogErrorLocation("Swapchain depth format has already been set.");
        return AXR_ERROR;
    }

    if (m_SwapchainColorFormatOptions.empty()) {
        axrLogErrorLocation("Swapchain color format options are empty.");
        return AXR_ERROR;
    }

    if (m_SwapchainDepthFormatOptions.empty()) {
        axrLogErrorLocation("Swapchain depth format options are empty.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    std::vector<int64_t> supportedSwapchainFormats;
    const AxrResult axrResult = m_XrSystem.getSupportedSwapchainFormats(supportedSwapchainFormats);
    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to get supported swapchain formats.");
        return AXR_ERROR;
    }

    std::vector<vk::Format> supportedSwapchainVkFormats(supportedSwapchainFormats.size());
    for (size_t i = 0; i < supportedSwapchainVkFormats.size(); ++i) {
        supportedSwapchainVkFormats[i] = static_cast<vk::Format>(supportedSwapchainFormats[i]);
    }

    // ---- Find color format ----

    auto foundFormatIt = std::ranges::find_first_of(
        m_SwapchainColorFormatOptions,
        supportedSwapchainVkFormats
    );

    if (foundFormatIt == m_SwapchainColorFormatOptions.end()) {
        axrLogErrorLocation("Failed to find a supported swapchain color format.");
        resetSwapchainFormats();
        return AXR_ERROR;
    }

    m_SwapchainColorFormat = *foundFormatIt;

    // ---- Find depth format ----

    foundFormatIt = std::ranges::find_first_of(
        m_SwapchainDepthFormatOptions,
        supportedSwapchainVkFormats
    );

    if (foundFormatIt == m_SwapchainDepthFormatOptions.end()) {
        axrLogErrorLocation("Failed to find a supported swapchain color format.");
        resetSwapchainFormats();
        return AXR_ERROR;
    }

    m_SwapchainDepthFormat = *foundFormatIt;

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::resetSwapchainFormats() {
    m_SwapchainColorFormat = vk::Format::eUndefined;
    m_SwapchainDepthFormat = vk::Format::eUndefined;
}

AxrResult AxrVulkanXrGraphics::setupSwapchain(const XrViewConfigurationView& viewConfiguration, View& view) const {
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = setSwapchainExtent(viewConfiguration, view);
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain(view);
        return axrResult;
    }

    axrResult = createSwapchain(view);
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain(view);
        return axrResult;
    }

    axrResult = createDepthBufferImages(view);
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain(view);
        return axrResult;
    }

    axrResult = createMsaaImages(view);
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain(view);
        return axrResult;
    }

    axrResult = createFramebuffers(view);
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain(view);
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::resetSetupSwapchain(View& view) const {
    destroyFramebuffers(view);
    destroyMsaaImages(view);
    destroyDepthBufferImages(view);
    destroySwapchain(view);
    resetSwapchainExtent(view);
}

AxrResult AxrVulkanXrGraphics::setSwapchainExtent(const XrViewConfigurationView& viewConfiguration, View& view) const {
    view.SwapchainExtent = vk::Extent2D(
        viewConfiguration.recommendedImageRectWidth,
        viewConfiguration.recommendedImageRectHeight
    );

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::resetSwapchainExtent(View& view) const {
    view.SwapchainExtent = vk::Extent2D(0, 0);
}

AxrResult AxrVulkanXrGraphics::createSwapchain(View& view) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (view.ColorSwapchain.Swapchain != XR_NULL_HANDLE) {
        axrLogErrorLocation("Color swapchain already exists.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    // ---- Color swapchain ----

    axrResult = m_XrSystem.createSwapchain(
        XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
        static_cast<int64_t>(m_SwapchainColorFormat),
        static_cast<uint32_t>(vk::SampleCountFlagBits::e1),
        view.SwapchainExtent.width,
        view.SwapchainExtent.height,
        view.ColorSwapchain.Swapchain
    );
    if (AXR_FAILED(axrResult)) {
        destroySwapchain(view);
        return axrResult;
    }

    axrResult = setupSwapchainImages(
        view.ColorSwapchain.Swapchain,
        vk::ImageAspectFlagBits::eColor,
        m_SwapchainColorFormat,
        view.ColorSwapchain.Images,
        view.ColorSwapchain.ImageViews
    );
    if (AXR_FAILED(axrResult)) {
        destroySwapchain(view);
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::destroySwapchain(View& view) const {
    resetSetupSwapchainImages(view.ColorSwapchain.Images, view.ColorSwapchain.ImageViews);
    m_XrSystem.destroySwapchain(view.ColorSwapchain.Swapchain);
}

AxrResult AxrVulkanXrGraphics::setupSwapchainImages(
    const XrSwapchain swapchain,
    const vk::ImageAspectFlags imageAspectFlags,
    const vk::Format imageFormat,
    std::vector<vk::Image>& images,
    std::vector<vk::ImageView>& imageViews
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (swapchain == XR_NULL_HANDLE) {
        axrLogErrorLocation("Swapchain is null.");
        return AXR_ERROR;
    }

    if (!images.empty()) {
        axrLogErrorLocation("Images already exist.");
        return AXR_ERROR;
    }

    if (!imageViews.empty()) {
        axrLogErrorLocation("Image views already exist.");
        return AXR_ERROR;
    }

    if (m_Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Device is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    axrResult = m_XrSystem.getVulkanSwapchainImages(
        swapchain,
        *reinterpret_cast<std::vector<VkImage>*>(&images)
    );
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchainImages(images, imageViews);
        return axrResult;
    }

    imageViews.resize(images.size());
    for (int i = 0; i < images.size(); ++i) {
        axrResult = AxrVulkanImage::createImageView(
            m_Device,
            images[i],
            imageFormat,
            imageAspectFlags,
            1,
            imageViews[i],
            m_Dispatch
        );
        if (AXR_FAILED(axrResult)) {
            break;
        }
    }

    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchainImages(images, imageViews);
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::resetSetupSwapchainImages(
    std::vector<vk::Image>& images,
    std::vector<vk::ImageView>& imageViews
) const {
    for (vk::ImageView& imageView : imageViews) {
        AxrVulkanImage::destroyImageView(m_Device, imageView, m_Dispatch);
    }
    imageViews.clear();

    images.clear();
}

AxrResult AxrVulkanXrGraphics::createDepthBufferImages(View& view) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!view.SwapchainDepthImages.empty()) {
        axrLogErrorLocation("Depth buffer images already exist.");
        return AXR_ERROR;
    }

    if (view.ColorSwapchain.Images.empty()) {
        axrLogErrorLocation("Swapchain color image images don't exist.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    vk::ImageAspectFlags imageAspectFlags = vk::ImageAspectFlagBits::eDepth;
    if (axrFormatHasStencilComponent(m_SwapchainDepthFormat)) {
        imageAspectFlags |= vk::ImageAspectFlagBits::eStencil;
    }

    view.SwapchainDepthImages.resize(view.ColorSwapchain.Images.size());

    for (AxrVulkanImage& depthBufferImage : view.SwapchainDepthImages) {
        depthBufferImage = AxrVulkanImage(
            {
                .PhysicalDevice = m_PhysicalDevice,
                .Device = m_Device,
                .GraphicsCommandPool = m_GraphicsCommandPool,
                .GraphicsQueue = m_QueueFamilies.GraphicsQueue,
                .DispatchHandle = &m_Dispatch
            }
        );

        axrResult = depthBufferImage.createImage(
            view.SwapchainExtent,
            m_MsaaSampleCount,
            m_SwapchainDepthFormat,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            imageAspectFlags
        );

        if (AXR_FAILED(axrResult)) {
            break;
        }

        axrResult = depthBufferImage.transitionImageLayout(
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthStencilAttachmentOptimal,
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eEarlyFragmentTests
        );

        if (AXR_FAILED(axrResult)) {
            break;
        }
    }

    if (AXR_FAILED(axrResult)) {
        destroyDepthBufferImages(view);
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::destroyDepthBufferImages(View& view) const {
    for (AxrVulkanImage& depthBufferImage : view.SwapchainDepthImages) {
        depthBufferImage.destroyImage();
    }
    view.SwapchainDepthImages.clear();
}

AxrResult AxrVulkanXrGraphics::createRenderPass() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_RenderPass != VK_NULL_HANDLE) {
        axrLogErrorLocation("Render pass already exists.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    return axrCreateRenderPass(
        m_Device,
        m_SwapchainColorFormat,
        m_SwapchainDepthFormat,
        m_SwapchainImageLayout,
        m_MsaaSampleCount,
        m_RenderPass,
        m_Dispatch
    );
}

void AxrVulkanXrGraphics::destroyRenderPass() {
    axrDestroyRenderPass(m_Device, m_RenderPass, m_Dispatch);
}

AxrResult AxrVulkanXrGraphics::setupAllViews() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_Views.empty()) {
        axrLogErrorLocation("Views already exist.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    const std::vector<XrViewConfigurationView> viewConfigurations = m_XrSystem.getViewConfigurations();
    m_Views.resize(viewConfigurations.size());

    for (size_t i = 0; i < m_Views.size(); ++i) {
        axrResult = setupView(viewConfigurations[i], m_Views[i]);
        if (AXR_FAILED(axrResult)) {
            break;
        }
    }

    if (AXR_FAILED(axrResult)) {
        resetSetupAllViews();
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::resetSetupAllViews() {
    for (View& view : m_Views) {
        resetSetupView(view);
    }

    m_Views.clear();
}

AxrResult AxrVulkanXrGraphics::setupView(const XrViewConfigurationView& viewConfiguration, View& view) const {
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = createSyncObjects(view);
    if (AXR_FAILED(axrResult)) {
        resetSetupView(view);
        return axrResult;
    }

    axrResult = createCommandBuffers(view);
    if (AXR_FAILED(axrResult)) {
        resetSetupView(view);
        return axrResult;
    }

    axrResult = setupSwapchain(viewConfiguration, view);
    if (AXR_FAILED(axrResult)) {
        resetSetupView(view);
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::resetSetupView(View& view) const {
    resetSetupSwapchain(view);
    destroyCommandBuffers(view);
    destroySyncObjects(view);
}

AxrResult AxrVulkanXrGraphics::createSyncObjects(View& view) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!view.RenderingFinishedSemaphores.empty()) {
        axrLogErrorLocation("Rendering finished semaphores already exist.");
        return AXR_ERROR;
    }

    if (!view.RenderingFences.empty()) {
        axrLogErrorLocation("Rendering fences already exist.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = axrCreateSemaphores(m_Device, m_MaxFramesInFlight, view.RenderingFinishedSemaphores, m_Dispatch);
    if (AXR_FAILED(axrResult)) {
        destroySyncObjects(view);
        return axrResult;
    }

    axrResult = axrCreateFences(m_Device, m_MaxFramesInFlight, view.RenderingFences, m_Dispatch);
    if (AXR_FAILED(axrResult)) {
        destroySyncObjects(view);
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::destroySyncObjects(View& view) const {
    axrDestroySemaphores(m_Device, view.RenderingFinishedSemaphores, m_Dispatch);
    axrDestroyFences(m_Device, view.RenderingFences, m_Dispatch);
}

AxrResult AxrVulkanXrGraphics::createCommandBuffers(View& view) const {
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = axrCreateCommandBuffers(
        m_Device,
        m_GraphicsCommandPool,
        m_MaxFramesInFlight,
        view.RenderingCommandBuffers,
        m_Dispatch
    );
    if (AXR_FAILED(axrResult)) {
        destroyCommandBuffers(view);
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::destroyCommandBuffers(View& view) const {
    axrDestroyCommandBuffers(m_Device, m_GraphicsCommandPool, view.RenderingCommandBuffers, m_Dispatch);
}

AxrResult AxrVulkanXrGraphics::createFramebuffers(View& view) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!view.SwapchainFramebuffers.empty()) {
        axrLogErrorLocation("Swapchain framebuffers already exist.");
        return AXR_ERROR;
    }

    if (view.ColorSwapchain.ImageViews.empty()) {
        axrLogErrorLocation("Swapchain color image views don't exist.");
        return AXR_ERROR;
    }

    if (view.SwapchainDepthImages.empty()) {
        axrLogErrorLocation("Swapchain color images don't exist.");
        return AXR_ERROR;
    }

    if (axrIsVulkanMsaaEnabled(m_MsaaSampleCount)) {
        if (view.SwapchainMsaaImages.empty()) {
            axrLogErrorLocation("Swapchain msaa images don't exist.");
            return AXR_ERROR;
        }
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    std::vector<vk::ImageView> depthImageViews(view.SwapchainDepthImages.size());
    for (int i = 0; i < depthImageViews.size(); ++i) {
        depthImageViews[i] = view.SwapchainDepthImages[i].getImageView();
    }

    std::vector<vk::ImageView> msaaImageViews(view.SwapchainMsaaImages.size());
    for (int i = 0; i < msaaImageViews.size(); ++i) {
        msaaImageViews[i] = view.SwapchainMsaaImages[i].getImageView();
    }

    const AxrResult axrResult = axrCreateFramebuffers(
        m_Device,
        m_RenderPass,
        view.SwapchainExtent,
        m_MsaaSampleCount,
        view.ColorSwapchain.ImageViews,
        depthImageViews,
        msaaImageViews,
        view.SwapchainFramebuffers,
        m_Dispatch
    );

    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to create swapchain framebuffers.");
        destroyFramebuffers(view);
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::destroyFramebuffers(View& view) const {
    axrDestroyFramebuffers(m_Device, view.SwapchainFramebuffers, m_Dispatch);
}

glm::mat4 AxrVulkanXrGraphics::createProjectionMatrix(
    const AxrCameraFov& fov,
    const float nearClip,
    const float farClip
) const {
    const float l = glm::tan(fov.Left);
    const float r = glm::tan(fov.Right);
    const float d = glm::tan(fov.Down);
    const float u = glm::tan(fov.Up);

    const float w = r - l;
    const float h = d - u;

    glm::mat4 projectionMatrix;
    projectionMatrix[0] = {2.0f / w, 0.0f, 0.0f, 0.0f};
    projectionMatrix[1] = {0.0f, 2.0f / h, 0.0f, 0.0f};
    projectionMatrix[2] = {(r + l) / w, (u + d) / h, -(farClip + nearClip) / (farClip - nearClip), -1.0f};
    projectionMatrix[3] = {0.0f, 0.0f, -(farClip * (nearClip + nearClip)) / (farClip - nearClip), 0.0f};
    return projectionMatrix;
}

AxrResult AxrVulkanXrGraphics::setMsaaSampleCount() {
    m_MsaaSampleCount = axrGetVulkanSampleCountToUse(m_PhysicalDevice, m_MaxMsaaSampleCount, m_Dispatch);

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::resetMsaaSampleCount() {
    m_MsaaSampleCount = vk::SampleCountFlagBits::e1;
}

AxrResult AxrVulkanXrGraphics::createMsaaImages(View& view) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!axrIsVulkanMsaaEnabled(m_MsaaSampleCount)) {
        return AXR_SUCCESS;
    }

    if (!view.SwapchainMsaaImages.empty()) {
        axrLogErrorLocation("Msaa images already exist.");
        return AXR_ERROR;
    }

    if (view.ColorSwapchain.Images.empty()) {
        axrLogErrorLocation("Swapchain color image images don't exist.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    view.SwapchainMsaaImages.resize(view.ColorSwapchain.Images.size());

    for (AxrVulkanImage& msaaImage : view.SwapchainMsaaImages) {
        msaaImage = AxrVulkanImage(
            {
                .PhysicalDevice = m_PhysicalDevice,
                .Device = m_Device,
                .GraphicsCommandPool = m_GraphicsCommandPool,
                .GraphicsQueue = m_QueueFamilies.GraphicsQueue,
                .DispatchHandle = &m_Dispatch
            }
        );

        axrResult = msaaImage.createImage(
            view.SwapchainExtent,
            m_MsaaSampleCount,
            m_SwapchainColorFormat,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            vk::ImageAspectFlagBits::eColor
        );

        if (AXR_FAILED(axrResult)) {
            break;
        }
    }

    if (AXR_FAILED(axrResult)) {
        destroyMsaaImages(view);
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanXrGraphics::destroyMsaaImages(View& view) const {
    for (AxrVulkanImage& msaaImage : view.SwapchainMsaaImages) {
        msaaImage.destroyImage();
    }
    view.SwapchainMsaaImages.clear();
}

AxrResult AxrVulkanXrGraphics::onXrSessionStateChangedCallback(const bool isSessionRunning) {
    if (isSessionRunning) {
        return setupXrSessionGraphics();
    }

    resetSetupXrSessionGraphics();
    return AXR_SUCCESS;
}

#endif
