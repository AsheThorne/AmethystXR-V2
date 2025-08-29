#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/xrSystem.h"
#include "../extensionCollection.hpp"
#include "axr/graphicsSystem.h"
#include "axr/common/callback.h"
#include "axr/actionSystem.h"

// Vulkan headers are required for <openxr/openxr_platform.h>
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
// ----------------------------------------- //
// Vulkan Headers
// ----------------------------------------- //
#include <vulkan/vulkan_core.h>
#endif

// ----------------------------------------- //
// OpenXR Headers
// ----------------------------------------- //
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

// ----------------------------------------- //
// EnTT Headers
// ----------------------------------------- //
#include "entt/entt.hpp"

/// Axr Xr System
class AxrXrSystem {
public:
    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Xr System Config
    struct Config {
        std::string ApplicationName;
        uint32_t ApplicationVersion;
        AxrGraphicsApiEnum GraphicsApi;
        AxrXrReferenceSpaceEnum StageReferenceSpace;
        uint32_t ApiLayerCount;
        AxrXrApiLayer_T* ApiLayers;
        uint32_t ExtensionCount;
        AxrXrExtension_T* Extensions;
    };

    /// Action binding
    struct ActionBinding {
        XrAction Action;
        const char* bindingName;
    };

    // ----------------------------------------- //
    // Types
    // ----------------------------------------- //

    /// On xr session state changed callback function type
    /// @param 1: True if the xr session is running. False if it's not
    using OnXrSessionStateChangedCallback_T = AxrCallback<void(bool)>;

    // ----------------------------------------- //
    // Public Variables
    // ----------------------------------------- //

    /// On xr session state changed for the graphics system
    OnXrSessionStateChangedCallback_T OnXrSessionStateChangedCallbackGraphics;
    /// On xr session state changed for the action system
    OnXrSessionStateChangedCallback_T OnXrSessionStateChangedCallbackActions;

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Null config Constructor
    explicit AxrXrSystem(std::nullptr_t);
    /// Constructor
    /// @param config Xr system config
    explicit AxrXrSystem(const Config& config);
    /// Copy Constructor
    /// @param src Source AxrXrSystem to copy from
    AxrXrSystem(const AxrXrSystem& src) = delete;
    /// Move Constructor
    /// @param src Source AxrXrSystem to move from
    AxrXrSystem(AxrXrSystem&& src) noexcept = delete;

    // ---- Destructor ----

    /// Destructor
    ~AxrXrSystem();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrXrSystem to copy from
    AxrXrSystem& operator=(const AxrXrSystem& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrXrSystem to move from
    AxrXrSystem& operator=(AxrXrSystem&& src) noexcept = delete;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Check if the xr system is valid for use
    /// @returns True if the xr system is valid for use
    [[nodiscard]] bool isValid() const;

    /// Check if the xr session is running
    /// @returns True if the xr session is running
    [[nodiscard]] bool isXrSessionRunning() const;

    /// Set the xr rendering clipping planes
    /// @param zNear Near clipping plane
    /// @param zFar Far clipping plane
    void setClippingPlane(float zNear, float zFar);

    /// Start the xr session
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult startXrSession();
    /// Signal that we want to stop the xr session
    void stopXrSession();

    // ---- For Internal Use ----
    // These functions are only to be used internally in the AmethystXr engine.
    // They have not been given a publicly accessible function in the 'include headers' to be used by an application.

    /// Set up the xr system
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setup();
    /// Reset setup()
    void resetSetup();

    /// Process the xr events
    void processEvents();

    /// Get the supported swapchain formats to use
    /// @param formats Output formats
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult getSupportedSwapchainFormats(std::vector<int64_t>& formats) const;
    /// Get the environment blend mode
    /// @returns The environment blend mode
    [[nodiscard]] XrEnvironmentBlendMode getEnvironmentBlendMode() const;

    /// Get the view configuration type
    /// @returns The view configuration type
    [[nodiscard]] XrViewConfigurationType getViewConfigurationType() const;
    /// Get the xr views
    /// @returns The xr views
    [[nodiscard]] std::vector<XrViewConfigurationView> getViewConfigurations() const;

    /// Get the near clipping plane
    /// @returns The near clipping plane
    [[nodiscard]] float getNearClippingPlane() const;
    /// Get the far clipping plane
    /// @returns The far clipping plane
    [[nodiscard]] float getFarClippingPlane() const;

    /// Create an XrActionSet
    /// @param name Action set name
    /// @param localizedName Localized action set name
    /// @param priority Priority
    /// @param actionSet Output action set
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createActionSet(
        const std::string& name,
        const std::string& localizedName,
        uint32_t priority,
        XrActionSet& actionSet
    ) const;
    /// Destroy the given action set
    /// @param actionSet Action set to destroy
    void destroyActionSet(XrActionSet& actionSet) const;
    /// Create an XrAction
    /// @param name Action name
    /// @param localizedName Localized action name
    /// @param actionType Action type
    /// @param actionSet Action set
    /// @param action Output action
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createAction(
        const std::string& name,
        const std::string& localizedName,
        XrActionType actionType,
        XrActionSet actionSet,
        XrAction& action
    ) const;
    /// Destroy the given action
    /// @param action Action to destroy
    void destroyAction(XrAction& action) const;

    /// Suggest the given action bindings for the given interaction profile
    /// @param interactionProfileEnum Interaction profile
    /// @param actionBindings Action bindings
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult suggestBindings(
        AxrXrInteractionProfileEnum interactionProfileEnum,
        const std::vector<ActionBinding>& actionBindings
    ) const;

    /// Attach the given action sets to the xr session
    /// @param actionSets Action sets
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult attachActionSets(std::unordered_map<std::string, AxrActionSet>& actionSets);
    /// Detach all the attached action sets
    void detachActionSets();

    /// Get the bool action state for the given action
    /// @param action Xr action
    /// @returns The bool action state
    [[nodiscard]] XrActionStateBoolean getBoolActionState(XrAction action) const;
    /// Get the float action state for the given action
    /// @param action Xr action
    /// @returns The float action state
    [[nodiscard]] XrActionStateFloat getFloatActionState(XrAction action) const;
    /// Get the vec2 action state for the given action
    /// @param action Xr action
    /// @returns The vec2 action state
    [[nodiscard]] XrActionStateVector2f getVec2ActionState(XrAction action) const;

    /// Create a new action space
    /// @param action XrAction to use
    /// @param space Output created action space
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createActionSpace(XrAction action, XrSpace& space) const;
    /// Create a new HMD view space
    /// @param space Output created action space
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createViewSpace(XrSpace& space) const;
    /// Destroy the given XrSpace
    /// @param space Space to destroy
    void destroySpace(XrSpace& space) const;

    /// Update all pose actions with the given time
    /// @param time Predicted display time
    /// @param registryHandle Optional registry handle to process the AxrMirrorPoseInputActionComponent's
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult updatePoseActions(XrTime time, entt::registry* registryHandle) const;

    /// Apply the haptic feedback to the given action
    /// @param action Action to use
    /// @param duration Haptic duration in nanoseconds
    /// @param frequency Haptic frequency in Hz
    /// @param amplitude Haptic amplitude from 0.0f to 1.0f
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult applyHapticFeedback(
        XrAction action,
        int64_t duration,
        float frequency,
        float amplitude
    ) const;
    /// Stop the haptic feedback to the given action
    /// @param action Action to use
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult stopHapticFeedback(XrAction action) const;

    /// Create an xr swapchain
    /// @param usageFlags Usage flags
    /// @param format Format
    /// @param sampleCount Sample count
    /// @param width Width
    /// @param height Height
    /// @param swapchain Output created swapchain
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createSwapchain(
        XrSwapchainUsageFlags usageFlags,
        int64_t format,
        uint32_t sampleCount,
        uint32_t width,
        uint32_t height,
        XrSwapchain& swapchain
    ) const;

    /// Destroy the given swapchain
    /// @param swapchain Swapchain
    void destroySwapchain(XrSwapchain& swapchain);

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    // ---- Vulkan Functions ----

    /// Create the vulkan instance to use
    /// @param pfnGetInstanceProcAddr A function pointer to vkGetInstanceProcAddr or a compatible entry point
    /// @param createInfo The VkInstanceCreateInfo
    /// @param vkInstance Output created vkInstance
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createVulkanInstance(
        PFN_vkGetInstanceProcAddr pfnGetInstanceProcAddr,
        const VkInstanceCreateInfo& createInfo,
        VkInstance& vkInstance
    ) const;

    /// Get the vulkan physical device to use
    /// @param vkInstance The vkInstance to use
    /// @param vkPhysicalDevice The Output VkPhysicalDevice
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult getVulkanPhysicalDevice(VkInstance vkInstance, VkPhysicalDevice& vkPhysicalDevice) const;

    /// Create the vulkan device to use
    /// @param pfnGetInstanceProcAddr A function pointer to vkGetInstanceProcAddr or a compatible entry point
    /// @param vkPhysicalDevice VkPhysicalDevice to use
    /// @param createInfo The VkDeviceCreateInfo
    /// @param vkDevice Output created vkDevice
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createVulkanDevice(
        PFN_vkGetInstanceProcAddr pfnGetInstanceProcAddr,
        VkPhysicalDevice vkPhysicalDevice,
        const VkDeviceCreateInfo& createInfo,
        VkDevice& vkDevice
    ) const;

    /// Set the graphics binding
    /// @param graphicsBinding Graphics binding to use
    void setGraphicsBinding(const XrGraphicsBindingVulkan2KHR& graphicsBinding);

    /// Get the vulkan images for the given swapchain
    /// @param swapchain Swapchain
    /// @param images Output vulkan images
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult getVulkanSwapchainImages(XrSwapchain swapchain, std::vector<VkImage>& images) const;
#endif

    /// Reset setGraphicsBinding()
    void resetGraphicsBinding();

    /// Begin the next render frame
    /// @param predictedDisplayTime Output predicted display time
    /// @returns AXR_SUCCESS if the function succeeded.
    /// @returns AXR_DONT_RENDER if we should skip rendering this frame.
    [[nodiscard]] AxrResult beginFrame(XrTime& predictedDisplayTime) const;
    /// End the current frame
    /// @param displayTime Display time
    /// @param compositionLayerViews Composition layer projection views
    /// @returns AXR_SUCCESS if the function succeeded.
    [[nodiscard]] AxrResult endFrame(
        XrTime displayTime,
        const std::vector<XrCompositionLayerProjectionView>& compositionLayerViews
    ) const;

    /// Locate the views for rendering
    /// @param predictedDisplayTime Predicted display time
    /// @param xrViews Output xr views
    /// @returns AXR_SUCCESS if the function succeeded.
    [[nodiscard]] AxrResult locateViews(XrTime predictedDisplayTime, std::vector<XrView>& xrViews) const;

    /// Acquire the next swapchain image to use
    /// @param swapchain Swapchain to use
    /// @param imageIndex Output swapchain image index
    /// @returns AXR_SUCCESS if the function succeeded.
    [[nodiscard]] AxrResult acquireSwapchainImage(XrSwapchain swapchain, uint32_t& imageIndex) const;
    /// Release the current swapchain image
    /// @param swapchain Swapchain to use
    /// @returns AXR_SUCCESS if the function succeeded.
    [[nodiscard]] AxrResult releaseSwapchainImage(XrSwapchain swapchain) const;

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //
    bool m_IsValid = false;

    // ---- Config Variables ----
    std::string m_ApplicationName = "";
    uint32_t m_ApplicationVersion = 0;
    AxrGraphicsApiEnum m_GraphicsApi = AXR_GRAPHICS_API_UNDEFINED;
    AxrXrReferenceSpaceEnum m_StageReferenceSpaceType = AXR_XR_REFERENCE_SPACE_UNDEFINED;
    AxrExtensionCollection<AxrXrApiLayer_T, AxrXrApiLayerTypeEnum> m_ApiLayers;
    AxrExtensionCollection<AxrXrExtension_T, AxrXrExtensionTypeEnum> m_Extensions;

    // ---- Data ----
    XrInstance m_Instance = XR_NULL_HANDLE;
    XrDebugUtilsMessengerEXT m_DebugUtilsMessenger = XR_NULL_HANDLE;
    XrSystemId m_SystemId = XR_NULL_SYSTEM_ID;
    std::vector<XrViewConfigurationType> m_SupportedViewConfigurationTypes;
    XrViewConfigurationType m_ViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
    std::vector<XrViewConfigurationView> m_ViewConfigurations;
    std::vector<XrEnvironmentBlendMode> m_SupportedEnvironmentBlendModes;
    XrEnvironmentBlendMode m_EnvironmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;
    XrBaseInStructure* m_GraphicsBinding = nullptr;
    bool m_IsSessionRunning = false;
    XrSession m_Session = XR_NULL_HANDLE;
    XrSessionState m_SessionState = XR_SESSION_STATE_UNKNOWN;
    XrSpace m_StageReferenceSpace = XR_NULL_HANDLE;
    float m_NearClippingPlane = 0.01f;
    float m_FarClippingPlane = 1000.0f;
    std::unordered_map<std::string, AxrActionSet*> m_AttachedActionSets;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    // ---- Vulkan Functions ----

    /// Choose the vulkan api version to use
    /// @param desiredApiVersion The desired vulkan api version to use
    /// @param apiVersion Output vulkan api version to use
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodscard]] AxrResult chooseVulkanApiVersion(uint32_t desiredApiVersion, uint32_t& apiVersion) const;
#endif

    /// Check if the session is active
    /// @returns True if the session is active
    [[nodiscard]] bool isSessionActive() const;

    // ---- Instance ----

    /// Create the instance
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createInstance();
    /// Destroy the instance
    void destroyInstance();

    /// Create the instance pNext chain
    /// @returns The instance structure chain
    [[nodiscard]] XrBaseOutStructure* createInstanceChain() const;

    // ---- .Next Chain ----

    /// Destroy the given chain
    /// @param chain The head of the chain
    void destroyChain(XrBaseOutStructure* chain);

    /// Append the given structure to the given chain
    /// @param chain The head of the chain
    /// @param structure The structure to add to the chain
    /// @param structureSize The size of the structure
    [[nodiscard]] XrBaseOutStructure* appendNextChain(
        XrBaseOutStructure* chain,
        const XrBaseInStructure* structure,
        size_t structureSize
    ) const;

    // ---- Api Layers / Extensions ----

    /// Get a collection of all api layer names to use
    /// @returns A collection of api layer names
    [[nodiscard]] std::vector<const char*> getAllApiLayerNames() const;
    /// Get a collection of all extension names to use
    /// @returns A collection of extension names
    [[nodiscard]] std::vector<const char*> getAllExtensionNames() const;

    /// Add the required extensions to m_Extensions
    void addRequiredExtensions();

    /// Find the supported api layers
    /// @returns The supported api layers
    [[nodiscard]] std::vector<std::string> getSupportedApiLayers() const;
    /// Find the supported extensions
    /// @returns The supported extensions
    [[nodiscard]] std::vector<std::string> getSupportedExtensions() const;

    /// Remove api layers from m_ApiLayers that aren't supported by the instance
    void removeUnsupportedApiLayers();
    /// Remove extensions from m_Extensions that aren't supported by the instance
    /// @returns AXR_SUCCESS if we can continue.
    /// @returns AXR_ERROR if there was a required extension that wasn't supported.
    [[nodiscard]] AxrResult removeUnsupportedExtensions();

    // ---- Debug Utils ----

    /// Create the debug utils messenger create info
    /// @retrns The debug utils messenger create info
    [[nodiscard]] XrDebugUtilsMessengerCreateInfoEXT createDebugUtilsCreateInfo() const;

    /// Create the debug utils
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createDebugUtils();
    /// Destroy the debug utils
    void destroyDebugUtils();

    // ---- System ----

    /// Set the system ID
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setSystemId();
    /// Reset setSystemId()
    void resetSystemId();

    /// Log the system details
    void logSystemDetails() const;

    // ---- View Configuration ----

    /// Set the view configuration
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setViewConfiguration();
    /// Reset setViewConfiguration()
    void resetViewConfiguration();

    /// Get view configuration type
    [[nodiscard]] AxrResult getViewConfigurationType(XrViewConfigurationType& viewConfigurationType) const;

    // ---- Environment ----

    /// Set the environment blend mode
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setEnvironmentBlendMode();
    /// reset setEnvironmentBlendMode()
    void resetEnvironmentBlendMode();

    // ---- Graphics Binding ----

    /// Destroy the graphics binding
    void destroyGraphicsBinding();

    // ---- Session ----

    /// Destroy the session data
    void destroySessionData();

    /// Create the session
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createSession();
    /// Destroy the session
    void destroySession();

    // ---- Space ----

    /// Get the supported reference space types
    /// @param referenceSpaces Output reference space types
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult getSupportedReferenceSpaces(std::vector<XrReferenceSpaceType>& referenceSpaces) const;

    /// Create an XrSpace from the given reference space type
    /// @param referenceSpaceType Reference space type
    /// @param referenceSpace Output reference space
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createReferenceSpace(
        XrReferenceSpaceType referenceSpaceType,
        XrSpace& referenceSpace
    ) const;

    // ---- Events ----

    /// OpenXR 'Events Lost' event handler
    /// @param eventData Event data
    void xrEvent_EventsLost(const XrEventDataEventsLost& eventData);
    /// OpenXR 'Instance Loss Pending' event handler
    /// @param eventData Event data
    void xrEvent_InstanceLossPending(const XrEventDataInstanceLossPending& eventData);
    /// OpenXR 'Interaction Profile Changed' event handler
    /// @param eventData Event data
    void xrEvent_InteractionProfileChanged(const XrEventDataInteractionProfileChanged& eventData);
    /// OpenXR 'Reference Space Change Pending' event handler
    /// @param eventData Event data
    void xrEvent_ReferenceSpaceChangePending(const XrEventDataReferenceSpaceChangePending& eventData);
    /// OpenXR 'Session State Changed' event handler
    /// @param eventData Event data
    void xrEvent_SessionStateChanged(const XrEventDataSessionStateChanged& eventData);

    // ---- Actions ----

    /// Update the state of the attached input actions
    void syncAttachedActions() const;

    // ----------------------------------------- //
    // Private Static Functions
    // ----------------------------------------- //

    /// Debug messages callback function
    /// @param messageSeverities The severity of the message
    /// @param messageTypes The type of the message
    /// @param pCallbackData Callback data
    /// @param userData User data
    /// @returns Indicates to the calling layer the application's desire to abort the call.
    /// Applications should always return XR_FALSE
    static XrBool32 debugUtilsCallback(
        XrDebugUtilsMessageSeverityFlagsEXT messageSeverities,
        XrDebugUtilsMessageTypeFlagsEXT messageTypes,
        const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* userData
    );
};
