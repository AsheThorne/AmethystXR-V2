#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "vulkanWindowGraphics.hpp"
#include "axr/logger.h"
#include "vulkanUtils.hpp"
#include "../../windowSystem/windowSystem.hpp"
#include "vulkanSharedFunctions.hpp"
#include "vulkanImage.hpp"
#include "../../assets/engineAssets.hpp"
#include "../../scene/scene.hpp"

// ---- Special Functions ----

AxrVulkanWindowGraphics::AxrVulkanWindowGraphics(const Config& config):
    m_WindowSystem(config.WindowSystem),
    m_Dispatch(config.Dispatch),
    m_LoadedScenes(config.LoadedScenes),
    m_MaxFramesInFlight(config.MaxFramesInFlight),
    m_PreferredPresentationMode(config.PresentationMode),
    m_MaxMsaaSampleCount(config.MaxMsaaSampleCount),
    m_Instance(VK_NULL_HANDLE),
    m_PhysicalDevice(VK_NULL_HANDLE),
    m_Device(VK_NULL_HANDLE),
    m_GraphicsCommandPool(VK_NULL_HANDLE),
    m_ClearColor(AxrColor(0.0f, 0.0f, 0.0f, 1.0f)),
    m_RenderSource(AXR_WINDOW_RENDER_SOURCE_SCENE_MAIN_CAMERA),
    m_SwapchainImageLayout(vk::ImageLayout::ePresentSrcKHR),
    m_Surface(VK_NULL_HANDLE),
    m_DoesSwapchainSupportBlitting(false),
    m_SwapchainColorFormat(vk::Format::eUndefined),
    m_SwapchainDepthFormat(vk::Format::eUndefined),
    m_SwapchainPresentationMode(static_cast<vk::PresentModeKHR>(VK_PRESENT_MODE_MAX_ENUM_KHR)),
    m_RenderPass(VK_NULL_HANDLE),
    m_SwapchainExtent(0, 0),
    m_Swapchain(VK_NULL_HANDLE),
    m_IsReady(false),
    m_CurrentImageIndex(0),
    m_CurrentFrame(0),
    m_IsSwapchainOutOfDate(false),
    m_MsaaSampleCount(vk::SampleCountFlagBits::e1) {
}

AxrVulkanWindowGraphics::~AxrVulkanWindowGraphics() {
    resetSetup();
}

// ---- Public Functions ----

void AxrVulkanWindowGraphics::addRequiredInstanceExtensions(
    AxrExtensionCollection<AxrVulkanExtension_T, AxrVulkanExtensionTypeEnum>& extensions
) const {
    auto surfaceExtension = AxrVulkanExtensionSurface{
        .IsRequired = true,
    };
    extensions.add(reinterpret_cast<AxrVulkanExtension_T>(&surfaceExtension));

#ifdef AXR_USE_PLATFORM_WIN32
    auto win32SurfaceExtension = AxrVulkanExtensionWin32Surface{
        .IsRequired = true,
    };
    extensions.add(reinterpret_cast<AxrVulkanExtension_T>(&win32SurfaceExtension));
#endif
}

void AxrVulkanWindowGraphics::addRequiredDeviceExtensions(
    AxrExtensionCollection<AxrVulkanExtension_T, AxrVulkanExtensionTypeEnum>& extensions
) const {
    auto swapchainExtension = AxrVulkanExtensionSwapchain{
        .IsRequired = true,
    };
    extensions.add(reinterpret_cast<AxrVulkanExtension_T>(&swapchainExtension));
}

void AxrVulkanWindowGraphics::setClearColor(const AxrColor& color) {
    m_ClearColor = color;
}

void AxrVulkanWindowGraphics::setRenderSource(const AxrWindowRenderSourceEnum renderSource) {
    m_RenderSource = renderSource;
}

AxrWindowRenderSourceEnum AxrVulkanWindowGraphics::getRenderSource() const {
    return m_RenderSource;
}

AxrResult AxrVulkanWindowGraphics::setup(const SetupConfig& config) {
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

    if (m_Device != VK_NULL_HANDLE) {
        axrLogErrorLocation("Logical device isn't null.");
        return AXR_ERROR;
    }

    if (config.Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Config logical device is null.");
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

    m_WindowSystem.OnWindowOpenStateChangedCallbackGraphics
                  .connect<&AxrVulkanWindowGraphics::onWindowOpenStateChangedCallback>(this);

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::resetSetup() {
    resetSetupWindowGraphics();

    m_WindowSystem.OnWindowOpenStateChangedCallbackGraphics.reset();
    resetSwapchainFormatOptions();

    m_Instance = VK_NULL_HANDLE;
    m_PhysicalDevice = VK_NULL_HANDLE;
    m_Device = VK_NULL_HANDLE;
    m_GraphicsCommandPool = VK_NULL_HANDLE;
    m_QueueFamilies.reset();
}

bool AxrVulkanWindowGraphics::isReady() const {
    return m_IsReady;
}

AxrResult AxrVulkanWindowGraphics::beginRendering(const AxrVulkanSceneData* sceneData) const {
    // Nothing needed here
    return AXR_SUCCESS;
}

AxrResult AxrVulkanWindowGraphics::endRendering() const {
    // Nothing needed here
    return AXR_SUCCESS;
}

uint32_t AxrVulkanWindowGraphics::getViewCount() const {
    return 1;
}

AxrPlatformType AxrVulkanWindowGraphics::getPlatformType() const {
    return AXR_PLATFORM_TYPE_WINDOW;
}

vk::Extent2D AxrVulkanWindowGraphics::getUIRegion() const {
    return m_SwapchainExtent;
}

vk::RenderPass AxrVulkanWindowGraphics::getRenderPass() const {
    return m_RenderPass;
}

vk::Framebuffer AxrVulkanWindowGraphics::getFramebuffer(const uint32_t viewIndex) const {
    return m_SwapchainFramebuffers[m_CurrentImageIndex];
}

vk::Extent2D AxrVulkanWindowGraphics::getSwapchainExtent(const uint32_t viewIndex) const {
    return m_SwapchainExtent;
}

vk::ClearColorValue AxrVulkanWindowGraphics::getClearColorValue() const {
    return vk::ClearColorValue(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
}

vk::CommandBuffer AxrVulkanWindowGraphics::getRenderingCommandBuffer(const uint32_t viewIndex) const {
    return m_RenderingCommandBuffers[m_CurrentFrame];
}

std::vector<vk::Semaphore> AxrVulkanWindowGraphics::getRenderingWaitSemaphores(const uint32_t viewIndex) const {
    return {m_ImageAvailableSemaphores[m_CurrentFrame]};
}

std::vector<vk::PipelineStageFlags> AxrVulkanWindowGraphics::getRenderingWaitStages(const uint32_t viewIndex) const {
    return {vk::PipelineStageFlagBits::eColorAttachmentOutput};
}

std::vector<vk::Semaphore> AxrVulkanWindowGraphics::getRenderingSignalSemaphores(const uint32_t viewIndex) const {
    return {m_RenderingFinishedSemaphores[m_CurrentFrame]};
}

vk::Fence AxrVulkanWindowGraphics::getRenderingFence(const uint32_t viewIndex) const {
    return {m_RenderingFences[m_CurrentFrame]};
}

uint32_t AxrVulkanWindowGraphics::getCurrentRenderingFrame() const {
    return m_CurrentFrame;
}

vk::Image AxrVulkanWindowGraphics::getSwapchainImage(uint32_t viewIndex) const {
    return m_SwapchainColorImages[m_CurrentImageIndex];
}

vk::Format AxrVulkanWindowGraphics::getSwapchainImageFormat() const {
    return m_SwapchainColorFormat.format;
}

vk::ImageLayout AxrVulkanWindowGraphics::getSwapchainImageLayout() const {
    return m_SwapchainImageLayout;
}

AxrResult AxrVulkanWindowGraphics::acquireNextSwapchainImage(const uint32_t viewIndex) {
    AxrResult axrResult = AXR_SUCCESS;

    if (m_IsSwapchainOutOfDate) {
        axrResult = recreateSwapchain();
        if (axrResult == AXR_DONT_RENDER) {
            return AXR_DONT_RENDER;
        }

        if (AXR_FAILED(axrResult)) {
            axrLogErrorLocation("Failed to recreate swapchain.");
            return AXR_ERROR;
        }
    }

    const vk::Result vkResult = m_Device.acquireNextImageKHR(
        m_Swapchain,
        UINT64_MAX,
        m_ImageAvailableSemaphores[m_CurrentFrame],
        VK_NULL_HANDLE,
        &m_CurrentImageIndex,
        m_Dispatch
    );

    if (vkResult == vk::Result::eErrorOutOfDateKHR || vkResult == vk::Result::eSuboptimalKHR) {
        axrResult = recreateSwapchain();
        if (axrResult == AXR_DONT_RENDER) {
            return AXR_DONT_RENDER;
        }

        if (AXR_FAILED(axrResult)) {
            axrLogErrorLocation("Failed to recreate swapchain.");
            return axrResult;
        }
    } else {
        axrLogVkResult(vkResult, "m_Device.acquireNextImageKHR");
        if (VK_FAILED(vkResult)) {
            return AXR_ERROR;
        }
    }

    return AXR_SUCCESS;
}

AxrResult AxrVulkanWindowGraphics::presentFrame(const uint32_t viewIndex) {
    const std::vector<vk::Semaphore> waitSemaphores = getRenderingSignalSemaphores(viewIndex);

    const vk::PresentInfoKHR presentInfo(
        static_cast<uint32_t>(waitSemaphores.size()),
        waitSemaphores.data(),
        1,
        &m_Swapchain,
        &m_CurrentImageIndex,
        nullptr
    );

    const vk::Result vkResult = m_QueueFamilies.PresentationQueue.presentKHR(
        &presentInfo,
        m_Dispatch
    );

    if (vkResult == vk::Result::eErrorOutOfDateKHR || vkResult == vk::Result::eSuboptimalKHR) {
        const AxrResult axrResult = recreateSwapchain();
        if (axrResult == AXR_DONT_RENDER) {
            return AXR_DONT_RENDER;
        }

        if (AXR_FAILED(axrResult)) {
            axrLogErrorLocation("Failed to recreate swapchain.");
            return axrResult;
        }
    } else {
        axrLogVkResult(vkResult, "PresentationQueue.presentKHR");
        if (AXR_FAILED(vkResult)) {
            return AXR_ERROR;
        }
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFramesInFlight;

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::getRenderingMatrices(
    const uint32_t viewIndex,
    glm::mat4& viewMatrix,
    glm::mat4& projectionMatrix
) const {
    AxrCameraInfo cameraInfo{};
    if (AXR_FAILED(getCameraInfo(viewIndex, cameraInfo))) {
        return;
    }

    viewMatrix = glm::inverse(
        glm::translate(glm::mat4(1.0f), cameraInfo.Position) *
        glm::toMat4(cameraInfo.Orientation)
    );

    projectionMatrix = glm::perspective(
        cameraInfo.Fov.Up + cameraInfo.Fov.Down,
        static_cast<float>(m_SwapchainExtent.width) / static_cast<float>(m_SwapchainExtent.height),
        cameraInfo.ZNear,
        cameraInfo.ZFar
    );
    projectionMatrix[1][1] *= -1.0f;
}

AxrResult AxrVulkanWindowGraphics::getCameraInfo(
    const uint32_t viewIndex,
    AxrCameraInfo& cameraInfo
) const {
    AxrTransformComponent cameraTransform{};
    AxrCameraComponent camera{};
    if (AXR_FAILED(getCameraComponents(cameraTransform, camera))) {
        return AXR_ERROR;
    }

    const float halfHorizontalFovRadians = glm::radians(camera.Fov) * 0.5f;
    const float halfVerticalFovRadians = atan(
        tan(halfHorizontalFovRadians) *
        (static_cast<float>(m_SwapchainExtent.height) / static_cast<float>(m_SwapchainExtent.width))
    );

    cameraInfo = AxrCameraInfo{
        .Position = cameraTransform.Position,
        .Orientation = cameraTransform.Orientation,
        .Fov = AxrCameraFov{
            .Up = halfVerticalFovRadians,
            .Down = halfVerticalFovRadians,
            .Left = halfHorizontalFovRadians,
            .Right = halfHorizontalFovRadians,
        },
        .PixelWidth = static_cast<float>(m_SwapchainExtent.width),
        .PixelHeight = static_cast<float>(m_SwapchainExtent.height),
        .AspectRatio = static_cast<float>(m_SwapchainExtent.width) /
        static_cast<float>(m_SwapchainExtent.height),
        // TODO: Maybe rename 'NearPlane' and 'FarPlane' to 'ZNear' and 'ZFar' everywhere it's mentioned.
        //  `Plane` indicates more than what it is. which is just the distance.
        .ZNear = camera.NearPlane,
        .ZFar = camera.FarPlane
    };

    return AXR_SUCCESS;
}

// ---- Private Functions ----

AxrResult AxrVulkanWindowGraphics::setupWindowGraphics() {
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = createSurface();
    if (AXR_FAILED(axrResult)) {
        resetSetupWindowGraphics();
        return axrResult;
    }

    const auto surfaceDetails = AxrVulkanSurfaceDetails(m_PhysicalDevice, m_Surface, m_Dispatch);
    if (!surfaceDetails.isValid()) {
        return AXR_ERROR;
    }

    axrResult = setSwapchainFormats(surfaceDetails.Formats);
    if (AXR_FAILED(axrResult)) {
        resetSetupWindowGraphics();
        return axrResult;
    }

    axrResult = setMsaaSampleCount();
    if (AXR_FAILED(axrResult)) {
        resetSetupWindowGraphics();
        return axrResult;
    }

    axrResult = createRenderPass();
    if (AXR_FAILED(axrResult)) {
        resetSetupWindowGraphics();
        return axrResult;
    }

    axrResult = createSyncObjects();
    if (AXR_FAILED(axrResult)) {
        resetSetupWindowGraphics();
        return axrResult;
    }

    axrResult = createCommandBuffers();
    if (AXR_FAILED(axrResult)) {
        resetSetupWindowGraphics();
        return axrResult;
    }

    axrResult = setupSwapchain(surfaceDetails);
    if (AXR_FAILED(axrResult)) {
        resetSetupWindowGraphics();
        return axrResult;
    }

    axrResult = m_LoadedScenes.setupWindowData(m_RenderPass, m_MsaaSampleCount);
    if (AXR_FAILED(axrResult)) {
        resetSetupWindowGraphics();
        return axrResult;
    }

    m_IsReady = true;
    m_WindowSystem.OnWindowResizedCallbackGraphics
                  .connect<&AxrVulkanWindowGraphics::onWindowResizedCallback>(this);

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::resetSetupWindowGraphics() {
    m_IsReady = false;
    m_WindowSystem.OnWindowResizedCallbackGraphics.reset();

    m_LoadedScenes.resetSetupWindowData();
    resetSetupSwapchain();
    destroyCommandBuffers();
    destroySyncObjects();
    destroyRenderPass();
    resetMsaaSampleCount();
    resetSwapchainFormats();
    destroySurface();
}

AxrResult AxrVulkanWindowGraphics::getCameraComponents(
    AxrTransformComponent& transform,
    AxrCameraComponent& camera
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    const AxrScene_T activeScene = m_LoadedScenes.getActiveScene();
    if (activeScene == nullptr) {
        axrLogErrorLocation("No active scene.");
        return AXR_ERROR;
    }

    if (!activeScene->isMainCameraValid()) {
        axrLogErrorLocation("No main camera.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const AxrEntityConst_T cameraEntity = activeScene->getMainCamera();
    auto [cameraComponent, cameraTransformComponent] = cameraEntity
        .get<AxrCameraComponent, AxrTransformComponent>();

    transform = cameraTransformComponent;
    camera = cameraComponent;

    return AXR_SUCCESS;
}

AxrResult AxrVulkanWindowGraphics::createSurface() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        axrLogErrorLocation("Physical device is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_ERROR;

#ifdef AXR_USE_PLATFORM_WIN32
    axrResult = createWin32Surface();
#else
    axrLogErrorLocation("Unknown platform.");
#endif

    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::destroySurface() {
    if (m_Surface != VK_NULL_HANDLE) {
        m_Instance.destroySurfaceKHR(m_Surface, nullptr, m_Dispatch);
        m_Surface = VK_NULL_HANDLE;
    }
}

#ifdef AXR_USE_PLATFORM_WIN32
AxrResult AxrVulkanWindowGraphics::createWin32Surface() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //
    if (m_Surface != VK_NULL_HANDLE) {
        axrLogErrorLocation("Surface already exists.");
        return AXR_ERROR;
    }

    const AxrWin32WindowSystem* win32WindowSystem = m_WindowSystem.getWin32WindowSystem();
    if (win32WindowSystem == nullptr) {
        axrLogErrorLocation("Win32 Window system is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const vk::Win32SurfaceCreateInfoKHR createInfo(
        {},
        win32WindowSystem->getInstance(),
        win32WindowSystem->getWindowHandle()
    );

    const vk::Result vkResult = m_Instance.createWin32SurfaceKHR(&createInfo, nullptr, &m_Surface, m_Dispatch);
    axrLogVkResult(vkResult, "m_Instance.createWin32SurfaceKHR");

    if (VK_FAILED(vkResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}
#endif

AxrResult AxrVulkanWindowGraphics::setupSwapchain(const AxrVulkanSurfaceDetails& surfaceDetails) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = setSwapchainPresentationMode(surfaceDetails.PresentationModes);
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain();
        return axrResult;
    }

    axrResult = setSwapchainExtent(surfaceDetails.Capabilities);
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain();
        return axrResult;
    }

    axrResult = createSwapchain(surfaceDetails.Capabilities);
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain();
        return axrResult;
    }

    axrResult = getSwapchainImages();
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain();
        return axrResult;
    }

    axrResult = createDepthBufferImages();
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain();
        return axrResult;
    }

    axrResult = createMsaaImages();
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain();
        return axrResult;
    }

    axrResult = createFramebuffers();
    if (AXR_FAILED(axrResult)) {
        resetSetupSwapchain();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::resetSetupSwapchain() {
    destroyFramebuffers();
    destroyMsaaImages();
    destroyDepthBufferImages();
    resetSwapchainImages();
    destroySwapchain();
    resetSwapchainExtent();
    resetSwapchainPresentationMode();
}

AxrResult AxrVulkanWindowGraphics::recreateSwapchain() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Device is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    // Don't bother recreating the swapchain if the window isn't visible
    uint32_t width = 0;
    uint32_t height = 0;
    axrResult = m_WindowSystem.getClientSize(width, height);
    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to get client size.");
        return axrResult;
    }

    if (width == 0 || height == 0) {
        m_IsSwapchainOutOfDate = true;
        return AXR_DONT_RENDER;
    }

    const vk::Result vkResult = m_Device.waitIdle(m_Dispatch);
    axrLogVkResult(vkResult, "m_Device.waitIdle");
    if (VK_FAILED(vkResult)) {
        return AXR_ERROR;
    }

    resetSetupSwapchain();

    const auto surfaceDetails = AxrVulkanSurfaceDetails(m_PhysicalDevice, m_Surface, m_Dispatch);
    if (!surfaceDetails.isValid()) {
        return AXR_ERROR;
    }

    axrResult = setupSwapchain(surfaceDetails);
    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to setup swapchain.");
        return axrResult;
    }

    if (AXR_FAILED(axrResult)) {
        resetSetupWindowGraphics();
        return axrResult;
    }

    m_IsSwapchainOutOfDate = false;

    return AXR_SUCCESS;
}

AxrResult AxrVulkanWindowGraphics::setSwapchainFormatOptions(
    const vk::PhysicalDevice& physicalDevice,
    const std::vector<vk::SurfaceFormatKHR>& swapchainColorFormatOptions,
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

    for (const vk::SurfaceFormatKHR surfaceFormat : swapchainColorFormatOptions) {
        if (axrAreFormatFeaturesSupported(
            surfaceFormat.format,
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eSampledImage &
            vk::FormatFeatureFlagBits::eColorAttachment &
            vk::FormatFeatureFlagBits::eBlitDst &
            vk::FormatFeatureFlagBits::eTransferDst,
            physicalDevice,
            m_Dispatch
        )) {
            m_SwapchainColorFormatOptions.push_back(surfaceFormat);
        }
    }

    // No format with BlitDst and TransferDst was found. Try to find formats without them.
    if (m_SwapchainColorFormatOptions.empty()) {
        for (const vk::SurfaceFormatKHR surfaceFormat : swapchainColorFormatOptions) {
            if (axrAreFormatFeaturesSupported(
                surfaceFormat.format,
                vk::ImageTiling::eOptimal,
                vk::FormatFeatureFlagBits::eSampledImage &
                vk::FormatFeatureFlagBits::eColorAttachment,
                physicalDevice,
                m_Dispatch
            )) {
                m_SwapchainColorFormatOptions.push_back(surfaceFormat);
            }
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

void AxrVulkanWindowGraphics::resetSwapchainFormatOptions() {
    m_SwapchainColorFormatOptions.clear();
    m_SwapchainDepthFormatOptions.clear();
}

AxrResult AxrVulkanWindowGraphics::setSwapchainFormats(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats) {
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

    if (surfaceFormats.empty()) {
        axrLogErrorLocation("Surface formats are empty.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const auto foundFormatIt = std::find_first_of(
        m_SwapchainColorFormatOptions.begin(),
        m_SwapchainColorFormatOptions.end(),
        surfaceFormats.begin(),
        surfaceFormats.end()
    );

    if (foundFormatIt != m_SwapchainColorFormatOptions.end()) {
        m_SwapchainColorFormat = *foundFormatIt;
    } else {
        axrLogErrorLocation("Failed to find a supported swapchain color format.");
        resetSwapchainFormats();
        return AXR_ERROR;
    }

    if (axrAreFormatFeaturesSupported(
        m_SwapchainColorFormat.format,
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eBlitDst &
        vk::FormatFeatureFlagBits::eTransferDst,
        m_PhysicalDevice,
        m_Dispatch
    )) {
        m_DoesSwapchainSupportBlitting = true;
    }

    // We don't need to check surface compatibility for the depth format.
    // As long as the options we're given are supported by the physical device, that's all we need.
    // And we just pick the first option since they're already ordered from most desired to least desired.
    m_SwapchainDepthFormat = m_SwapchainDepthFormatOptions[0];

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::resetSwapchainFormats() {
    m_DoesSwapchainSupportBlitting = false;
    m_SwapchainColorFormat = vk::Format::eUndefined;
    m_SwapchainDepthFormat = vk::Format::eUndefined;
}

AxrResult AxrVulkanWindowGraphics::setSwapchainPresentationMode(
    const std::vector<vk::PresentModeKHR>& surfacePresentationModes
) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_SwapchainPresentationMode != static_cast<vk::PresentModeKHR>(VK_PRESENT_MODE_MAX_ENUM_KHR)) {
        axrLogErrorLocation("Swapchain presentation mode has already been set.");
        return AXR_ERROR;
    }

    if (m_PreferredPresentationMode == AXR_VULKAN_PRESENTATION_MODE_UNDEFINED) {
        axrLogErrorLocation("Preferred presentation mode is undefined.");
        return AXR_ERROR;
    }

    if (surfacePresentationModes.empty()) {
        axrLogErrorLocation("Surface presentation modes are empty.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    // Find the preferred presentation mode
    for (const vk::PresentModeKHR presentationMode : surfacePresentationModes) {
        if (presentationMode == axrToVkPresentMode(m_PreferredPresentationMode)) {
            m_SwapchainPresentationMode = presentationMode;
            return AXR_SUCCESS;
        }
    }

    // Find FIFO presentation mode as a backup if preferred presentation mode isn't supported
    for (const vk::PresentModeKHR presentationMode : surfacePresentationModes) {
        if (presentationMode == vk::PresentModeKHR::eFifo) {
            m_SwapchainPresentationMode = presentationMode;
            return AXR_SUCCESS;
        }
    }

    // Just use what ever we can that's supported
    m_SwapchainPresentationMode = surfacePresentationModes[0];

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::resetSwapchainPresentationMode() {
    m_SwapchainPresentationMode = static_cast<vk::PresentModeKHR>(VK_PRESENT_MODE_MAX_ENUM_KHR);
}

AxrResult AxrVulkanWindowGraphics::setSwapchainExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities) {
    // If the current extent width is the max uint32_t value, then we need to get the extent manually
    if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
        m_SwapchainExtent = surfaceCapabilities.currentExtent;
        return AXR_SUCCESS;
    }

    uint32_t width;
    uint32_t height;
    const AxrResult axrResult = m_WindowSystem.getClientSize(width, height);
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    width = std::clamp(
        width,
        surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width
    );
    height = std::clamp(
        height,
        surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height
    );

    m_SwapchainExtent = vk::Extent2D(width, height);
    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::resetSwapchainExtent() {
    m_SwapchainExtent.width = 0;
    m_SwapchainExtent.height = 0;
}

AxrResult AxrVulkanWindowGraphics::createSwapchain(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Swapchain != VK_NULL_HANDLE) {
        axrLogErrorLocation("Swapchain already exists.");
        return AXR_ERROR;
    }

    if (m_Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Logical device is null.");
        return AXR_ERROR;
    }

    if (m_Surface == VK_NULL_HANDLE) {
        axrLogErrorLocation("Surface is null.");
        return AXR_ERROR;
    }

    if (!m_QueueFamilies.GraphicsQueueFamilyIndex.has_value()) {
        axrLogErrorLocation("Graphics queue family index is undefined.");
        return AXR_ERROR;
    }

    if (!m_QueueFamilies.PresentationQueueFamilyIndex.has_value()) {
        axrLogErrorLocation("Presentation queue family index is undefined.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;

    // 0 is a special value that indicates that there is no maximum
    if (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.
        maxImageCount) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }

    vk::ImageUsageFlags imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    if (m_DoesSwapchainSupportBlitting) {
        imageUsage |= vk::ImageUsageFlagBits::eTransferDst;
    }

    vk::SwapchainCreateInfoKHR swapchainCreateInfo(
        {},
        m_Surface,
        minImageCount,
        m_SwapchainColorFormat.format,
        m_SwapchainColorFormat.colorSpace,
        m_SwapchainExtent,
        1,
        imageUsage,
        vk::SharingMode::eExclusive,
        0,
        nullptr,
        surfaceCapabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        m_SwapchainPresentationMode,
        vk::True,
        VK_NULL_HANDLE
    );

    const uint32_t graphicsFamilyIndex = m_QueueFamilies.GraphicsQueueFamilyIndex.value();
    const uint32_t presentationFamilyIndex = m_QueueFamilies.PresentationQueueFamilyIndex.value();
    const uint32_t queueFamilyIndices[] = {graphicsFamilyIndex, presentationFamilyIndex};

    if (graphicsFamilyIndex != presentationFamilyIndex) {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    const vk::Result vkResult = m_Device.createSwapchainKHR(
        &swapchainCreateInfo,
        nullptr,
        &m_Swapchain,
        m_Dispatch
    );
    axrLogVkResult(vkResult, "m_Device.createSwapchainKHR");

    if (VK_FAILED(vkResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::destroySwapchain() {
    if (m_Swapchain != VK_NULL_HANDLE) {
        m_Device.destroySwapchainKHR(m_Swapchain, nullptr, m_Dispatch);
        m_Swapchain = VK_NULL_HANDLE;
    }
}

AxrResult AxrVulkanWindowGraphics::getSwapchainImages() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_SwapchainColorImages.empty()) {
        axrLogErrorLocation("Swapchain color images already exist.");
        return AXR_ERROR;
    }

    if (!m_SwapchainColorImageViews.empty()) {
        axrLogErrorLocation("Swapchain color image views already exist.");
        return AXR_ERROR;
    }

    if (m_Device == VK_NULL_HANDLE) {
        axrLogErrorLocation("Logical device is null.");
        return AXR_ERROR;
    }

    if (m_Swapchain == VK_NULL_HANDLE) {
        axrLogErrorLocation("Swapchain is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const auto swapchainImagesResult = m_Device.getSwapchainImagesKHR(m_Swapchain, m_Dispatch);
    axrLogVkResult(swapchainImagesResult.result, "m_Device.getSwapchainImagesKHR");
    if (VK_FAILED(swapchainImagesResult.result)) {
        resetSwapchainImages();
        return AXR_ERROR;
    }

    m_SwapchainColorImages = swapchainImagesResult.value;
    m_SwapchainColorImageViews.resize(m_SwapchainColorImages.size());
    AxrResult axrResult = AXR_SUCCESS;

    for (size_t i = 0; i < m_SwapchainColorImageViews.size(); ++i) {
        axrResult = AxrVulkanImage::createImageView(
            m_Device,
            m_SwapchainColorImages[i],
            m_SwapchainColorFormat.format,
            vk::ImageAspectFlagBits::eColor,
            1,
            m_SwapchainColorImageViews[i],
            m_Dispatch
        );
        if (AXR_FAILED(axrResult)) {
            break;
        }
    }

    if (AXR_FAILED(axrResult)) {
        resetSwapchainImages();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::resetSwapchainImages() {
    for (vk::ImageView& imageView : m_SwapchainColorImageViews) {
        AxrVulkanImage::destroyImageView(m_Device, imageView, m_Dispatch);
    }
    m_SwapchainColorImageViews.clear();
    m_SwapchainColorImages.clear();
}

AxrResult AxrVulkanWindowGraphics::createRenderPass() {
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
        m_SwapchainColorFormat.format,
        m_SwapchainDepthFormat,
        m_SwapchainImageLayout,
        m_MsaaSampleCount,
        m_RenderPass,
        m_Dispatch
    );
}

void AxrVulkanWindowGraphics::destroyRenderPass() {
    axrDestroyRenderPass(m_Device, m_RenderPass, m_Dispatch);
}

AxrResult AxrVulkanWindowGraphics::createSyncObjects() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_ImageAvailableSemaphores.empty()) {
        axrLogErrorLocation("Image available semaphores already exist.");
        return AXR_ERROR;
    }

    if (!m_RenderingFinishedSemaphores.empty()) {
        axrLogErrorLocation("Rendering finished semaphores already exist.");
        return AXR_ERROR;
    }

    if (!m_RenderingFences.empty()) {
        axrLogErrorLocation("Rendering fences already exist.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = axrCreateSemaphores(m_Device, m_MaxFramesInFlight, m_ImageAvailableSemaphores, m_Dispatch);
    if (AXR_FAILED(axrResult)) {
        destroySyncObjects();
        return axrResult;
    }

    axrResult = axrCreateSemaphores(m_Device, m_MaxFramesInFlight, m_RenderingFinishedSemaphores, m_Dispatch);
    if (AXR_FAILED(axrResult)) {
        destroySyncObjects();
        return axrResult;
    }

    axrResult = axrCreateFences(m_Device, m_MaxFramesInFlight, m_RenderingFences, m_Dispatch);
    if (AXR_FAILED(axrResult)) {
        destroySyncObjects();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::destroySyncObjects() {
    axrDestroySemaphores(m_Device, m_ImageAvailableSemaphores, m_Dispatch);
    axrDestroySemaphores(m_Device, m_RenderingFinishedSemaphores, m_Dispatch);
    axrDestroyFences(m_Device, m_RenderingFences, m_Dispatch);
}

AxrResult AxrVulkanWindowGraphics::createCommandBuffers() {
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = axrCreateCommandBuffers(
        m_Device,
        m_GraphicsCommandPool,
        m_MaxFramesInFlight,
        m_RenderingCommandBuffers,
        m_Dispatch
    );
    if (AXR_FAILED(axrResult)) {
        destroyCommandBuffers();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::destroyCommandBuffers() {
    axrDestroyCommandBuffers(m_Device, m_GraphicsCommandPool, m_RenderingCommandBuffers, m_Dispatch);
}

AxrResult AxrVulkanWindowGraphics::createFramebuffers() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_SwapchainFramebuffers.empty()) {
        axrLogErrorLocation("Swapchain framebuffers already exist.");
        return AXR_ERROR;
    }

    if (m_SwapchainColorImageViews.empty()) {
        axrLogErrorLocation("Swapchain color image views don't exist.");
        return AXR_ERROR;
    }

    if (m_SwapchainDepthImages.empty()) {
        axrLogErrorLocation("Swapchain depth images don't exist.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    std::vector<vk::ImageView> depthBufferImageViews(m_SwapchainDepthImages.size());
    for (int i = 0; i < depthBufferImageViews.size(); ++i) {
        depthBufferImageViews[i] = m_SwapchainDepthImages[i].getImageView();
    }

    std::vector<vk::ImageView> msaaImageViews(m_SwapchainMsaaImages.size());
    for (int i = 0; i < msaaImageViews.size(); ++i) {
        msaaImageViews[i] = m_SwapchainMsaaImages[i].getImageView();
    }

    const AxrResult axrResult = axrCreateFramebuffers(
        m_Device,
        m_RenderPass,
        m_SwapchainExtent,
        m_MsaaSampleCount,
        m_SwapchainColorImageViews,
        depthBufferImageViews,
        msaaImageViews,
        m_SwapchainFramebuffers,
        m_Dispatch
    );

    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to create swapchain framebuffers.");
        destroyFramebuffers();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::destroyFramebuffers() {
    axrDestroyFramebuffers(m_Device, m_SwapchainFramebuffers, m_Dispatch);
}

AxrResult AxrVulkanWindowGraphics::createDepthBufferImages() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_SwapchainDepthImages.empty()) {
        axrLogErrorLocation("Depth buffer images already exist.");
        return AXR_ERROR;
    }

    if (m_SwapchainColorImages.empty()) {
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

    m_SwapchainDepthImages.resize(m_SwapchainColorImages.size());

    for (AxrVulkanImage& depthBufferImage : m_SwapchainDepthImages) {
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
            m_SwapchainExtent,
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
        destroyDepthBufferImages();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::destroyDepthBufferImages() {
    for (AxrVulkanImage& depthBufferImage : m_SwapchainDepthImages) {
        depthBufferImage.destroyImage();
    }
    m_SwapchainDepthImages.clear();
}

AxrResult AxrVulkanWindowGraphics::setMsaaSampleCount() {
    m_MsaaSampleCount = axrGetVulkanSampleCountToUse(m_PhysicalDevice, m_MaxMsaaSampleCount, m_Dispatch);

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::resetMsaaSampleCount() {
    m_MsaaSampleCount = vk::SampleCountFlagBits::e1;
}

AxrResult AxrVulkanWindowGraphics::createMsaaImages() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!axrIsVulkanMsaaEnabled(m_MsaaSampleCount)) {
        return AXR_SUCCESS;
    }

    if (!m_SwapchainMsaaImages.empty()) {
        axrLogErrorLocation("Msaa images already exist.");
        return AXR_ERROR;
    }

    if (m_SwapchainColorImages.empty()) {
        axrLogErrorLocation("Swapchain color image images don't exist.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    m_SwapchainMsaaImages.resize(m_SwapchainColorImages.size());

    for (AxrVulkanImage& msaaImage : m_SwapchainMsaaImages) {
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
            m_SwapchainExtent,
            m_MsaaSampleCount,
            m_SwapchainColorFormat.format,
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
        destroyMsaaImages();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::destroyMsaaImages() {
    for (AxrVulkanImage& msaaImage : m_SwapchainMsaaImages) {
        msaaImage.destroyImage();
    }
    m_SwapchainMsaaImages.clear();
}

AxrResult AxrVulkanWindowGraphics::onWindowOpenStateChangedCallback(const bool isWindowOpen) {
    if (isWindowOpen) {
        return setupWindowGraphics();
    }

    resetSetupWindowGraphics();
    return AXR_SUCCESS;
}

void AxrVulkanWindowGraphics::onWindowResizedCallback(uint32_t width, uint32_t height) {
    const AxrResult axrResult = recreateSwapchain();

    if (axrResult != AXR_DONT_RENDER && AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to recreate swapchain.");
    }
}

#endif
