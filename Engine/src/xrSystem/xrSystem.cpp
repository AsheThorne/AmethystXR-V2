// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "xrSystem.hpp"
#include "axr/logger.h"
#include "xrUtils.hpp"
#include "../common.hpp"
#include "../utils.hpp"
#include "xrExtensionFunctions.hpp"
#include "../actionSystem/actionSet.hpp"
#include "../actionSystem/actionUtils.hpp"
#include "axr/scene.h"

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
#include "../graphicsSystem/vulkan/vulkanUtils.hpp"
#endif

// ----------------------------------------- //
// External Functions
// ----------------------------------------- //

bool axrXrSystemIsValid(const AxrXrSystemConst_T xrSystem) {
    if (xrSystem == nullptr) {
        axrLogErrorLocation("`xrSystem` is null.");
        return false;
    }

    return xrSystem->isValid();
}

bool axrXrSystemIsXrSessionRunning(const AxrXrSystemConst_T xrSystem) {
    if (xrSystem == nullptr) {
        axrLogErrorLocation("`xrSystem` is null.");
        return false;
    }

    return xrSystem->isXrSessionRunning();
}

void axrXrSystemSetClippingPlane(const AxrXrSystem_T xrSystem, const float zNear, const float zFar) {
    if (xrSystem == nullptr) {
        axrLogErrorLocation("`xrSystem` is null.");
        return;
    }

    xrSystem->setClippingPlane(zNear, zFar);
}

AxrResult axrXrSystemStartXrSession(const AxrXrSystem_T xrSystem) {
    if (xrSystem == nullptr) {
        axrLogErrorLocation("`xrSystem` is null.");
        return AXR_ERROR;
    }

    return xrSystem->startXrSession();
}

void axrXrSystemStopXrSession(const AxrXrSystem_T xrSystem) {
    if (xrSystem == nullptr) {
        axrLogErrorLocation("`xrSystem` is null.");
        return;
    }

    xrSystem->stopXrSession();
}

// ----------------------------------------- //
// Internal Functions
// ----------------------------------------- //

// ---- Special Functions ----

AxrXrSystem::AxrXrSystem(std::nullptr_t) {
}

AxrXrSystem::AxrXrSystem(const Config& config):
    m_IsValid(true),
    m_ApplicationName(config.ApplicationName),
    m_ApplicationVersion(config.ApplicationVersion),
    m_GraphicsApi(config.GraphicsApi),
    m_StageReferenceSpaceType(config.StageReferenceSpace),
    m_SupportedViewConfigurationTypes(
        {
            XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
            XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO,
        }
    ),
    m_SupportedEnvironmentBlendModes(
        {
            XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
            XR_ENVIRONMENT_BLEND_MODE_ADDITIVE,
        }
    ) {
    m_ApiLayers.add(config.ApiLayerCount, config.ApiLayers);
    m_Extensions.add(config.ExtensionCount, config.Extensions);

    addRequiredExtensions();
}

AxrXrSystem::~AxrXrSystem() {
    resetSetup();

    m_Extensions.clear();
    m_ApiLayers.clear();
}

// ---- Public Functions ----

bool AxrXrSystem::isValid() const {
    return m_IsValid;
}

bool AxrXrSystem::isXrSessionRunning() const {
    return m_IsSessionRunning;
}

void AxrXrSystem::setClippingPlane(const float zNear, const float zFar) {
    m_NearClippingPlane = zNear;
    m_FarClippingPlane = zFar;
}

AxrResult AxrXrSystem::startXrSession() {
    if (m_IsSessionRunning) {
        return AXR_SUCCESS;
    }

    AxrResult axrResult = AXR_SUCCESS;

    axrResult = createSession();
    if (AXR_FAILED(axrResult)) {
        destroySessionData();
        return axrResult;
    }

    axrResult = createReferenceSpace(
        axrToReferenceSpace(m_StageReferenceSpaceType),
        m_StageReferenceSpace
    );
    if (AXR_FAILED(axrResult)) {
        destroySessionData();
        return axrResult;
    }

    m_IsSessionRunning = true;
    return AXR_SUCCESS;
}

void AxrXrSystem::stopXrSession() {
    if (!m_IsSessionRunning) {
        return;
    }

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return;
    }

    const XrResult xrResult = xrRequestExitSession(m_Session);
    axrLogXrResult(xrResult, "xrRequestExitSession");
}

AxrResult AxrXrSystem::setup() {
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = createInstance();
    if (AXR_FAILED(axrResult)) {
        resetSetup();
        return axrResult;
    }

    axrResult = createDebugUtils();
    if (AXR_FAILED(axrResult)) {
        resetSetup();
        return axrResult;
    }

    axrResult = setSystemId();
    if (AXR_FAILED(axrResult)) {
        resetSetup();
        return axrResult;
    }

    logSystemDetails();

    axrResult = setViewConfiguration();
    if (AXR_FAILED(axrResult)) {
        resetSetup();
        return axrResult;
    }

    axrResult = setEnvironmentBlendMode();
    if (AXR_FAILED(axrResult)) {
        resetSetup();
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrXrSystem::resetSetup() {
    destroySessionData();

    resetEnvironmentBlendMode();
    resetViewConfiguration();
    resetSystemId();
    destroyDebugUtils();
    destroyInstance();
}

void AxrXrSystem::processEvents() {
    XrEventDataBuffer eventData{.type = XR_TYPE_EVENT_DATA_BUFFER};
    auto xrPollEvents = [&]() -> bool {
        eventData = {.type = XR_TYPE_EVENT_DATA_BUFFER};
        return xrPollEvent(m_Instance, &eventData) == XR_SUCCESS;
    };

    while (xrPollEvents()) {
        // NOLINTNEXTLINE(clang-diagnostic-switch-enum)
        switch (eventData.type) {
            case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
                xrEvent_EventsLost(*reinterpret_cast<XrEventDataEventsLost*>(&eventData));
                break;
            }
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                xrEvent_InstanceLossPending(*reinterpret_cast<XrEventDataInstanceLossPending*>(&eventData));
                break;
            }
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
                xrEvent_InteractionProfileChanged(*reinterpret_cast<XrEventDataInteractionProfileChanged*>(&eventData));
                break;
            }
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
                xrEvent_ReferenceSpaceChangePending(
                    *reinterpret_cast<XrEventDataReferenceSpaceChangePending*>(&eventData)
                );
                break;
            }
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                xrEvent_SessionStateChanged(*reinterpret_cast<XrEventDataSessionStateChanged*>(&eventData));
                break;
            }
            default: {
                break;
            }
        }
    }

    syncAttachedActions();
}

AxrResult AxrXrSystem::getSupportedSwapchainFormats(std::vector<int64_t>& formats) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    uint32_t availableSwapchainFormatsCount;
    XrResult xrResult = xrEnumerateSwapchainFormats(
        m_Session,
        0,
        &availableSwapchainFormatsCount,
        nullptr
    );
    axrLogXrResult(xrResult, "xrEnumerateSwapchainFormats");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    std::vector<int64_t> availableSwapchainFormats(availableSwapchainFormatsCount);
    xrResult = xrEnumerateSwapchainFormats(
        m_Session,
        availableSwapchainFormatsCount,
        &availableSwapchainFormatsCount,
        availableSwapchainFormats.data()
    );
    axrLogXrResult(xrResult, "xrEnumerateSwapchainFormats");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    formats = availableSwapchainFormats;
    return AXR_SUCCESS;
}

XrEnvironmentBlendMode AxrXrSystem::getEnvironmentBlendMode() const {
    return m_EnvironmentBlendMode;
}

XrViewConfigurationType AxrXrSystem::getViewConfigurationType() const {
    return m_ViewConfigurationType;
}

std::vector<XrViewConfigurationView> AxrXrSystem::getViewConfigurations() const {
    return m_ViewConfigurations;
}

float AxrXrSystem::getNearClippingPlane() const {
    return m_NearClippingPlane;
}

float AxrXrSystem::getFarClippingPlane() const {
    return m_FarClippingPlane;
}

AxrResult AxrXrSystem::createActionSet(
    const std::string& name,
    const std::string& localizedName,
    const uint32_t priority,
    XrActionSet& actionSet
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    XrActionSetCreateInfo actionSetCreateInfo{
        .type = XR_TYPE_ACTION_SET_CREATE_INFO,
        .next = nullptr,
        .actionSetName = {},
        .localizedActionSetName = {},
        .priority = priority,
    };

    strncpy_s(
        actionSetCreateInfo.actionSetName,
        name.c_str(),
        XR_MAX_ACTION_SET_NAME_SIZE
    );
    strncpy_s(
        actionSetCreateInfo.localizedActionSetName,
        localizedName.c_str(),
        XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE
    );

    const XrResult xrResult = xrCreateActionSet(m_Instance, &actionSetCreateInfo, &actionSet);
    axrLogXrResult(xrResult, "xrSyncActions");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    return AXR_SUCCESS;
}

void AxrXrSystem::destroyActionSet(XrActionSet& actionSet) const {
    if (actionSet == XR_NULL_HANDLE) return;

    const XrResult xrResult = xrDestroyActionSet(actionSet);
    axrLogXrResult(xrResult, "xrDestroyActionSet");
    if (XR_SUCCEEDED(xrResult)) {
        actionSet = XR_NULL_HANDLE;
    }
}

AxrResult AxrXrSystem::createAction(
    const std::string& name,
    const std::string& localizedName,
    const XrActionType actionType,
    const XrActionSet actionSet,
    XrAction& action
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (actionSet == XR_NULL_HANDLE) {
        axrLogErrorLocation("Action set is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    XrActionCreateInfo actionCreateInfo{
        .type = XR_TYPE_ACTION_CREATE_INFO,
        .next = nullptr,
        .actionName = {},
        .actionType = actionType,
        .countSubactionPaths = 0,
        .subactionPaths = nullptr,
        .localizedActionName = {},
    };

    strncpy_s(
        actionCreateInfo.actionName,
        name.c_str(),
        XR_MAX_ACTION_NAME_SIZE
    );
    strncpy_s(
        actionCreateInfo.localizedActionName,
        localizedName.c_str(),
        XR_MAX_LOCALIZED_ACTION_NAME_SIZE
    );

    const XrResult xrResult = xrCreateAction(actionSet, &actionCreateInfo, &action);
    axrLogXrResult(xrResult, "xrSyncActions");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    return AXR_SUCCESS;
}

void AxrXrSystem::destroyAction(XrAction& action) const {
    if (action == XR_NULL_HANDLE) return;

    const XrResult xrResult = xrDestroyAction(action);
    axrLogXrResult(xrResult, "xrDestroyAction");
    if (XR_SUCCEEDED(xrResult)) {
        action = XR_NULL_HANDLE;
    }
}

AxrResult AxrXrSystem::suggestBindings(
    const AxrXrInteractionProfileEnum interactionProfileEnum,
    const std::vector<ActionBinding>& actionBindings
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (actionBindings.empty()) {
        return AXR_SUCCESS;
    }

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    const char* interactionProfileName = axrGetXrInteractionProfileName(interactionProfileEnum);
    if (axrStringIsEmpty(interactionProfileName)) {
        return AXR_ERROR;
    }

    XrPath interactionProfilePath = XR_NULL_PATH;
    XrResult xrResult = xrStringToPath(m_Instance, interactionProfileName, &interactionProfilePath);
    axrLogXrResult(xrResult, "xrStringToPath");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    const std::unordered_set<const char*> availableBindings = axrGetXrInteractionProfileBindingNames(
        interactionProfileEnum
    );

    std::vector<XrActionSuggestedBinding> suggestedBindings;
    suggestedBindings.reserve(actionBindings.size());
    for (const ActionBinding& actionBinding : actionBindings) {
        if (!availableBindings.contains(actionBinding.bindingName)) continue;

        XrPath path = XR_NULL_PATH;
        xrResult = xrStringToPath(m_Instance, actionBinding.bindingName, &path);
        axrLogXrResult(xrResult, "xrStringToPath");
        if (XR_FAILED(xrResult)) return AXR_ERROR;

        suggestedBindings.push_back(
            XrActionSuggestedBinding{
                .action = actionBinding.Action,
                .binding = path,
            }
        );
    }

    if (suggestedBindings.empty()) {
        return AXR_SUCCESS;
    }

    const XrInteractionProfileSuggestedBinding interactionProfileSuggestedBindings{
        .type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING,
        .next = nullptr,
        .interactionProfile = interactionProfilePath,
        .countSuggestedBindings = static_cast<uint32_t>(suggestedBindings.size()),
        .suggestedBindings = suggestedBindings.data(),
    };

    xrResult = xrSuggestInteractionProfileBindings(m_Instance, &interactionProfileSuggestedBindings);
    axrLogXrResult(xrResult, "xrSuggestInteractionProfileBindings");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::attachActionSets(std::unordered_map<std::string, AxrActionSet>& actionSets) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (actionSets.empty()) return AXR_SUCCESS;

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    std::vector<XrActionSet> xrActionSets;
    xrActionSets.reserve(actionSets.size());

    for (auto& [actionSetName, actionSet] : actionSets) {
        const XrActionSet xrActionSet = actionSet.getXrActionSet();
        if (xrActionSet == XR_NULL_HANDLE) continue;

        xrActionSets.push_back(xrActionSet);
        m_AttachedActionSets.insert(std::pair(actionSetName, &actionSet));
    }

    const XrSessionActionSetsAttachInfo attachInfo{
        .type = XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO,
        .next = nullptr,
        .countActionSets = static_cast<uint32_t>(xrActionSets.size()),
        .actionSets = xrActionSets.data(),
    };

    const XrResult xrResult = xrAttachSessionActionSets(m_Session, &attachInfo);
    axrLogXrResult(xrResult, "xrAttachSessionActionSets");
    if (XR_FAILED(xrResult)) {
        detachActionSets();
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrXrSystem::detachActionSets() {
    m_AttachedActionSets.clear();
}

XrActionStateBoolean AxrXrSystem::getBoolActionState(const XrAction action) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!isSessionActive()) return {};

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return {};
    }

    if (action == XR_NULL_HANDLE) {
        axrLogErrorLocation("Action is null.");
        return {};
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrActionStateGetInfo actionStateGetInfo{
        .type = XR_TYPE_ACTION_STATE_GET_INFO,
        .next = nullptr,
        .action = action,
        .subactionPath = XR_NULL_PATH,
    };

    XrActionStateBoolean actionState{
        .type = XR_TYPE_ACTION_STATE_BOOLEAN,
    };

    const XrResult xrResult = xrGetActionStateBoolean(m_Session, &actionStateGetInfo, &actionState);
    axrLogXrResult(xrResult, "xrGetActionStateBoolean");
    if (XR_FAILED(xrResult)) return {};

    return actionState;
}

XrActionStateFloat AxrXrSystem::getFloatActionState(const XrAction action) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!isSessionActive()) return {};

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return {};
    }

    if (action == XR_NULL_HANDLE) {
        axrLogErrorLocation("Action is null.");
        return {};
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrActionStateGetInfo actionStateGetInfo{
        .type = XR_TYPE_ACTION_STATE_GET_INFO,
        .next = nullptr,
        .action = action,
        .subactionPath = XR_NULL_PATH,
    };

    XrActionStateFloat actionState{
        .type = XR_TYPE_ACTION_STATE_FLOAT,
    };

    const XrResult xrResult = xrGetActionStateFloat(m_Session, &actionStateGetInfo, &actionState);
    axrLogXrResult(xrResult, "xrGetActionStateFloat");
    if (XR_FAILED(xrResult)) return {};

    return actionState;
}

XrActionStateVector2f AxrXrSystem::getVec2ActionState(const XrAction action) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!isSessionActive()) return {};

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return {};
    }

    if (action == XR_NULL_HANDLE) {
        axrLogErrorLocation("Action is null.");
        return {};
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrActionStateGetInfo actionStateGetInfo{
        .type = XR_TYPE_ACTION_STATE_GET_INFO,
        .next = nullptr,
        .action = action,
        .subactionPath = XR_NULL_PATH,
    };

    XrActionStateVector2f actionState{
        .type = XR_TYPE_ACTION_STATE_VECTOR2F,
    };

    const XrResult xrResult = xrGetActionStateVector2f(m_Session, &actionStateGetInfo, &actionState);
    axrLogXrResult(xrResult, "xrGetActionStateVector2f");
    if (XR_FAILED(xrResult)) return {};

    return actionState;
}

AxrResult AxrXrSystem::createActionSpace(const XrAction action, XrSpace& space) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (space != XR_NULL_HANDLE) {
        axrLogErrorLocation("Space already exists.");
        return AXR_ERROR;
    }

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrActionSpaceCreateInfo actionSpaceCreateInfo{
        .type = XR_TYPE_ACTION_SPACE_CREATE_INFO,
        .next = nullptr,
        .action = action,
        .subactionPath = XR_NULL_PATH,
        .poseInActionSpace = XrPosef{
            .orientation = XrQuaternionf{
                .x = 0.0f,
                .y = 0.0f,
                .z = 0.0f,
                .w = 1.0f
            },
            .position = XrVector3f{
                .x = 0.0f,
                .y = 0.0f,
                .z = 0.0f
            },
        },
    };

    const XrResult xrResult = xrCreateActionSpace(m_Session, &actionSpaceCreateInfo, &space);
    axrLogXrResult(xrResult, "xrLocateSpace");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::createViewSpace(XrSpace& space) const {
    return createReferenceSpace(XR_REFERENCE_SPACE_TYPE_VIEW, space);
}

void AxrXrSystem::destroySpace(XrSpace& space) const {
    if (space == XR_NULL_HANDLE) return;

    const XrResult xrResult = xrDestroySpace(space);
    axrLogXrResult(xrResult, "xrDestroySpace");

    if (XR_SUCCEEDED(xrResult)) {
        space = XR_NULL_HANDLE;
    }
}

AxrResult AxrXrSystem::updatePoseActions(const XrTime time, entt::registry* registryHandle) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_AttachedActionSets.empty()) return AXR_SUCCESS;

    if (m_StageReferenceSpace == XR_NULL_HANDLE) {
        axrLogErrorLocation("Stage reference space is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    for (AxrActionSet* actionSet : m_AttachedActionSets | std::views::values) {
        if (actionSet == nullptr || !actionSet->isEnabled()) continue;

        for (AxrPoseInputAction& poseAction : actionSet->getPoseInputActions() | std::views::values) {
            if (!poseAction.isEnabled()) continue;

            const XrSpace xrSpace = poseAction.getXrSpace();
            if (xrSpace == XR_NULL_HANDLE) {
                axrLogWarningLocation("XrSpace is null.");
                continue;
            }

            XrSpaceVelocity spaceVelocity{
                .type = XR_TYPE_SPACE_VELOCITY,
            };

            XrSpaceLocation spaceLocation{
                .type = XR_TYPE_SPACE_LOCATION,
                .next = &spaceVelocity,
            };

            const XrResult xrResult = xrLocateSpace(xrSpace, m_StageReferenceSpace, time, &spaceLocation);
            axrLogXrResult(xrResult, "xrLocateSpace");
            if (XR_FAILED(xrResult)) return AXR_ERROR;

            poseAction.trigger(
                AxrPose{
                    .position = AxrVec3{
                        .x = spaceLocation.pose.position.x,
                        .y = spaceLocation.pose.position.y,
                        .z = spaceLocation.pose.position.z,
                    },
                    .orientation = AxrQuaternion{
                        .x = spaceLocation.pose.orientation.x,
                        .y = spaceLocation.pose.orientation.y,
                        .z = spaceLocation.pose.orientation.z,
                        .w = spaceLocation.pose.orientation.w,
                    }
                }
            );
        }
    }

    if (registryHandle != nullptr) {
        for (const auto& [entity, poseInputActionComponent, transformComponent] :
             registryHandle->view<AxrMirrorPoseInputActionComponent, AxrTransformComponent>().each()) {
            registryHandle->patch<AxrTransformComponent>(
                entity,
                [&](AxrTransformComponent& transform) {
                    const auto foundActionSet = m_AttachedActionSets.find(poseInputActionComponent.ActionSetName);
                    if (foundActionSet == m_AttachedActionSets.end()) {
                        return;
                    }

                    const auto foundPoseAction = foundActionSet->second->getPoseInputAction(
                        poseInputActionComponent.PoseInputActionName
                    );
                    if (foundPoseAction == nullptr) {
                        return;
                    }

                    const AxrPose poseValue = foundPoseAction->getValue();

                    transform.Position = poseInputActionComponent.OffsetPosition + glm::vec3(
                        poseValue.position.x,
                        poseValue.position.y,
                        poseValue.position.z
                    );
                    transform.Orientation = poseInputActionComponent.OffsetOrientation * glm::quat(
                        poseValue.orientation.w,
                        poseValue.orientation.x,
                        poseValue.orientation.y,
                        poseValue.orientation.z
                    );
                }
            );
        }
    }

    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::applyHapticFeedback(
    const XrAction action,
    const int64_t duration,
    const float frequency,
    const float amplitude
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!isSessionActive()) return AXR_SUCCESS;

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrHapticActionInfo hapticActionInfo{
        .type = XR_TYPE_HAPTIC_ACTION_INFO,
        .next = nullptr,
        .action = action,
        .subactionPath = XR_NULL_PATH,
    };

    const XrHapticVibration vibration{
        .type = XR_TYPE_HAPTIC_VIBRATION,
        .next = nullptr,
        .duration = duration,
        .frequency = frequency,
        .amplitude = amplitude,
    };

    const XrResult xrResult = xrApplyHapticFeedback(
        m_Session,
        &hapticActionInfo,
        reinterpret_cast<const XrHapticBaseHeader*>(&vibration)
    );
    axrLogXrResult(xrResult, "xrApplyHapticFeedback");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::stopHapticFeedback(const XrAction action) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!isSessionActive()) return AXR_SUCCESS;

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrHapticActionInfo hapticActionInfo{
        .type = XR_TYPE_HAPTIC_ACTION_INFO,
        .next = nullptr,
        .action = action,
        .subactionPath = XR_NULL_PATH,
    };

    const XrResult xrResult = xrStopHapticFeedback(m_Session, &hapticActionInfo);
    axrLogXrResult(xrResult, "xrStopHapticFeedback");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::createSwapchain(
    const XrSwapchainUsageFlags usageFlags,
    const int64_t format,
    const uint32_t sampleCount,
    const uint32_t width,
    const uint32_t height,
    XrSwapchain& swapchain
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (swapchain != XR_NULL_HANDLE) {
        axrLogErrorLocation("Swapchain already exists.");
        return AXR_ERROR;
    }

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrSwapchainCreateInfo swapchainCreateInfo{
        .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
        .next = nullptr,
        .createFlags = {},
        .usageFlags = usageFlags,
        .format = format,
        .sampleCount = sampleCount,
        .width = width,
        .height = height,
        .faceCount = 1,
        .arraySize = 1,
        .mipCount = 1,
    };

    const XrResult xrResult = xrCreateSwapchain(m_Session, &swapchainCreateInfo, &swapchain);
    axrLogXrResult(xrResult, "xrCreateSwapchain");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrXrSystem::destroySwapchain(XrSwapchain& swapchain) {
    if (swapchain == XR_NULL_HANDLE) return;

    const XrResult xrResult = xrDestroySwapchain(swapchain);
    axrLogXrResult(xrResult, "xrDestroySwapchain");
    if (XR_SUCCEEDED(xrResult)) {
        swapchain = XR_NULL_HANDLE;
    }
}

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
AxrResult AxrXrSystem::createVulkanInstance(
    const PFN_vkGetInstanceProcAddr pfnGetInstanceProcAddr,
    const VkInstanceCreateInfo& createInfo,
    VkInstance& vkInstance
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    if (m_SystemId == XR_NULL_SYSTEM_ID) {
        axrLogErrorLocation("System ID is null.");
        return AXR_ERROR;
    }

    if (createInfo.pApplicationInfo == nullptr) {
        axrLogErrorLocation("VkInstanceCreateInfo.pApplicationInfo is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    uint32_t vulkanApiVersion;
    const AxrResult axrResult = chooseVulkanApiVersion(
        createInfo.pApplicationInfo->apiVersion,
        vulkanApiVersion
    );
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    const VkApplicationInfo appInfo{
        .sType = createInfo.pApplicationInfo->sType,
        .pNext = createInfo.pApplicationInfo->pNext,
        .pApplicationName = createInfo.pApplicationInfo->pApplicationName,
        .applicationVersion = createInfo.pApplicationInfo->applicationVersion,
        .pEngineName = createInfo.pApplicationInfo->pEngineName,
        .engineVersion = createInfo.pApplicationInfo->engineVersion,
        .apiVersion = vulkanApiVersion,
    };

    const VkInstanceCreateInfo vkInstanceCreateInfo{
        .sType = createInfo.sType,
        .pNext = createInfo.pNext,
        .flags = createInfo.flags,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = createInfo.enabledLayerCount,
        .ppEnabledLayerNames = createInfo.ppEnabledLayerNames,
        .enabledExtensionCount = createInfo.enabledExtensionCount,
        .ppEnabledExtensionNames = createInfo.ppEnabledExtensionNames,
    };

    const XrVulkanInstanceCreateInfoKHR xrVulkanInstanceCreateInfo{
        .type = XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR,
        .next = nullptr,
        .systemId = m_SystemId,
        .createFlags = {},
        .pfnGetInstanceProcAddr = pfnGetInstanceProcAddr,
        .vulkanCreateInfo = &vkInstanceCreateInfo,
        .vulkanAllocator = nullptr,
    };

    VkResult vkResult = VK_ERROR_UNKNOWN;
    const XrResult xrResult = xrCreateVulkanInstanceKHR(
        m_Instance,
        &xrVulkanInstanceCreateInfo,
        &vkInstance,
        &vkResult
    );
    axrLogXrResult(xrResult, "xrCreateVulkanInstanceKHR");
    if (XR_FAILED(xrResult) || VK_FAILED(vkResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::getVulkanPhysicalDevice(const VkInstance vkInstance, VkPhysicalDevice& vkPhysicalDevice) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    if (m_SystemId == XR_NULL_SYSTEM_ID) {
        axrLogErrorLocation("System ID is null.");
        return AXR_ERROR;
    }

    if (vkInstance == VK_NULL_HANDLE) {
        axrLogErrorLocation("vkInstance is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrVulkanGraphicsDeviceGetInfoKHR graphicsDeviceGetInfo{
        .type = XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR,
        .next = nullptr,
        .systemId = m_SystemId,
        .vulkanInstance = vkInstance,
    };

    const XrResult xrResult = xrGetVulkanGraphicsDevice2KHR(m_Instance, &graphicsDeviceGetInfo, &vkPhysicalDevice);
    axrLogXrResult(xrResult, "xrGetVulkanGraphicsDevice2KHR");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::createVulkanDevice(
    const PFN_vkGetInstanceProcAddr pfnGetInstanceProcAddr,
    const VkPhysicalDevice vkPhysicalDevice,
    const VkDeviceCreateInfo& createInfo,
    VkDevice& vkDevice
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    if (m_SystemId == XR_NULL_SYSTEM_ID) {
        axrLogErrorLocation("System ID is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrVulkanDeviceCreateInfoKHR xrVulkanDeviceCreateInfo{
        .type = XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR,
        .next = nullptr,
        .systemId = m_SystemId,
        .createFlags = {},
        .pfnGetInstanceProcAddr = pfnGetInstanceProcAddr,
        .vulkanPhysicalDevice = vkPhysicalDevice,
        .vulkanCreateInfo = &createInfo,
        .vulkanAllocator = nullptr,
    };

    VkResult vkResult = VK_ERROR_UNKNOWN;
    const XrResult xrResult = xrCreateVulkanDeviceKHR(m_Instance, &xrVulkanDeviceCreateInfo, &vkDevice, &vkResult);
    axrLogXrResult(xrResult, "xrCreateVulkanDeviceKHR");
    if (XR_FAILED(xrResult) || VK_FAILED(vkResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrXrSystem::setGraphicsBinding(const XrGraphicsBindingVulkan2KHR& graphicsBinding) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_GraphicsBinding != nullptr) {
        axrLogErrorLocation("Graphics binding already exists.");
        return;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    m_GraphicsBinding = reinterpret_cast<XrBaseInStructure*>(new XrGraphicsBindingVulkan2KHR(graphicsBinding));
}

AxrResult AxrXrSystem::getVulkanSwapchainImages(const XrSwapchain swapchain, std::vector<VkImage>& images) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (swapchain == XR_NULL_HANDLE) {
        axrLogErrorLocation("Swapchain is null.");
        return AXR_ERROR;
    }

    if (!images.empty()) {
        axrLogErrorLocation("Swapchain images already exist.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    uint32_t swapchainImageCount;
    XrResult xrResult = xrEnumerateSwapchainImages(
        swapchain,
        0,
        &swapchainImageCount,
        nullptr
    );
    axrLogXrResult(xrResult, "xrEnumerateSwapchainImages");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    std::vector<XrSwapchainImageVulkan2KHR> swapchainImages(
        swapchainImageCount,
        {.type = XR_TYPE_SWAPCHAIN_IMAGE_VULKAN2_KHR}
    );
    xrResult = xrEnumerateSwapchainImages(
        swapchain,
        swapchainImageCount,
        &swapchainImageCount,
        reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainImages.data())
    );
    axrLogXrResult(xrResult, "xrEnumerateSwapchainImages");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    images.resize(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        images[i] = swapchainImages[i].image;
    }

    return AXR_SUCCESS;
}
#endif

void AxrXrSystem::resetGraphicsBinding() {
    destroyGraphicsBinding();
}

AxrResult AxrXrSystem::beginFrame(XrTime& predictedDisplayTime) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    XrFrameState frameState{.type = XR_TYPE_FRAME_STATE};
    XrResult xrResult = xrWaitFrame(m_Session, nullptr, &frameState);
    axrLogXrResult(xrResult, "xrWaitFrame");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    constexpr XrFrameBeginInfo frameBeginInfo{.type = XR_TYPE_FRAME_BEGIN_INFO};
    xrResult = xrBeginFrame(m_Session, &frameBeginInfo);
    axrLogXrResult(xrResult, "xrBeginFrame");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    if (!isSessionActive() || !frameState.shouldRender) {
        const AxrResult axrResult = endFrame(
            frameState.predictedDisplayTime,
            {}
        );
        if (AXR_FAILED(axrResult)) {
            return axrResult;
        }

        return AXR_DONT_RENDER;
    }

    predictedDisplayTime = frameState.predictedDisplayTime;
    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::endFrame(
    const XrTime displayTime,
    const std::vector<XrCompositionLayerProjectionView>& compositionLayerViews
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    if (m_StageReferenceSpace == XR_NULL_HANDLE) {
        axrLogErrorLocation("View reference space is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    std::vector<const XrCompositionLayerBaseHeader*> compositionLayers;
    XrCompositionLayerProjection projectionLayer;

    if (!compositionLayerViews.empty()) {
        projectionLayer = XrCompositionLayerProjection{
            .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION,
            .next = nullptr,
            .layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT,
            .space = m_StageReferenceSpace,
            .viewCount = static_cast<uint32_t>(compositionLayerViews.size()),
            .views = compositionLayerViews.data(),
        };

        compositionLayers.push_back(reinterpret_cast<const XrCompositionLayerBaseHeader*>(&projectionLayer));
    }

    const XrFrameEndInfo frameEndInfo{
        .type = XR_TYPE_FRAME_END_INFO,
        .next = nullptr,
        .displayTime = displayTime,
        .environmentBlendMode = m_EnvironmentBlendMode,
        .layerCount = static_cast<uint32_t>(compositionLayers.size()),
        .layers = compositionLayers.data(),
    };
    const XrResult xrResult = xrEndFrame(m_Session, &frameEndInfo);
    axrLogXrResult(xrResult, "xrEndFrame");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::locateViews(const XrTime predictedDisplayTime, std::vector<XrView>& xrViews) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    if (m_ViewConfigurationType == XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM) {
        axrLogErrorLocation("View configuration type is undefined.");
        return AXR_ERROR;
    }

    if (m_ViewConfigurations.empty()) {
        axrLogErrorLocation("View configurations are empty.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrViewLocateInfo viewLocateInfo{
        .type = XR_TYPE_VIEW_LOCATE_INFO,
        .next = nullptr,
        .viewConfigurationType = m_ViewConfigurationType,
        .displayTime = predictedDisplayTime,
        .space = m_StageReferenceSpace,
    };

    XrViewState viewState{.type = XR_TYPE_VIEW_STATE};
    xrViews = std::vector(m_ViewConfigurations.size(), XrView{.type = XR_TYPE_VIEW});
    uint32_t viewCount = 0;
    const XrResult xrResult = xrLocateViews(
        m_Session,
        &viewLocateInfo,
        &viewState,
        static_cast<uint32_t>(xrViews.size()),
        &viewCount,
        xrViews.data()
    );
    axrLogXrResult(xrResult, "xrLocateViews");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::acquireSwapchainImage(const XrSwapchain swapchain, uint32_t& imageIndex) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (swapchain == XR_NULL_HANDLE) {
        axrLogErrorLocation("Swapchain is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    constexpr XrSwapchainImageAcquireInfo acquireInfo{
        .type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO,
        .next = nullptr,
    };

    XrResult xrResult = xrAcquireSwapchainImage(swapchain, &acquireInfo, &imageIndex);
    axrLogXrResult(xrResult, "xrAcquireSwapchainImage");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    constexpr XrSwapchainImageWaitInfo waitInfo{
        .type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
        .next = nullptr,
        .timeout = XR_INFINITE_DURATION
    };

    xrResult = xrWaitSwapchainImage(swapchain, &waitInfo);
    axrLogXrResult(xrResult, "xrWaitSwapchainImage");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::releaseSwapchainImage(const XrSwapchain swapchain) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (swapchain == XR_NULL_HANDLE) {
        axrLogErrorLocation("Swapchain is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    constexpr XrSwapchainImageReleaseInfo releaseInfo{
        .type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO,
        .next = nullptr,
    };

    const XrResult xrResult = xrReleaseSwapchainImage(swapchain, &releaseInfo);
    axrLogXrResult(xrResult, "xrReleaseSwapchainImage");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    return AXR_SUCCESS;
}

// ---- Private Functions ----

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
AxrResult AxrXrSystem::chooseVulkanApiVersion(const uint32_t desiredApiVersion, uint32_t& apiVersion) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    if (m_SystemId == XR_NULL_SYSTEM_ID) {
        axrLogErrorLocation("System ID is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    XrGraphicsRequirementsVulkan2KHR graphicsRequirements{
        .type = XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR
    };
    const XrResult xrResult = xrGetVulkanGraphicsRequirements2KHR(m_Instance, m_SystemId, &graphicsRequirements);
    axrLogXrResult(xrResult, "xrGetVulkanGraphicsRequirements2KHR");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    const uint32_t minSupportedApiVersionMajor = XR_VERSION_MAJOR(graphicsRequirements.minApiVersionSupported);
    const uint32_t minSupportedApiVersionMinor = XR_VERSION_MINOR(graphicsRequirements.minApiVersionSupported);
    const uint32_t maxSupportedApiVersionMajor = XR_VERSION_MAJOR(graphicsRequirements.maxApiVersionSupported);
    const uint32_t maxSupportedApiVersionMinor = XR_VERSION_MINOR(graphicsRequirements.maxApiVersionSupported);

    const uint32_t desiredApiVersionMajor = VK_VERSION_MAJOR(desiredApiVersion);
    const uint32_t desiredApiVersionMinor = VK_VERSION_MINOR(desiredApiVersion);

    if (desiredApiVersionMajor < minSupportedApiVersionMajor ||
        desiredApiVersionMinor < minSupportedApiVersionMinor) {
        apiVersion = VK_MAKE_API_VERSION(0, minSupportedApiVersionMajor, minSupportedApiVersionMinor, 0);
        return AXR_SUCCESS;
    }

    if (desiredApiVersionMajor > maxSupportedApiVersionMajor ||
        desiredApiVersionMinor > maxSupportedApiVersionMinor) {
        apiVersion = VK_MAKE_API_VERSION(0, maxSupportedApiVersionMajor, maxSupportedApiVersionMinor, 0);
        return AXR_SUCCESS;
    }

    apiVersion = VK_MAKE_API_VERSION(0, desiredApiVersionMajor, desiredApiVersionMinor, 0);
    return AXR_SUCCESS;
}
#endif

bool AxrXrSystem::isSessionActive() const {
    return m_SessionState == XR_SESSION_STATE_SYNCHRONIZED ||
        m_SessionState == XR_SESSION_STATE_VISIBLE ||
        m_SessionState == XR_SESSION_STATE_FOCUSED;
}

AxrResult AxrXrSystem::createInstance() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance != XR_NULL_HANDLE) {
        axrLogWarningLocation("Instance already exists.");
        return AXR_SUCCESS;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    removeUnsupportedApiLayers();
    const AxrResult axrResult = removeUnsupportedExtensions();
    if (AXR_FAILED(axrResult)) return axrResult;

    const std::vector<const char*> layers = getAllApiLayerNames();
    const std::vector<const char*> extensions = getAllExtensionNames();
    XrBaseOutStructure* instanceStructureChain = createInstanceChain();

    XrApplicationInfo applicationInfo{};
    strncpy_s(applicationInfo.applicationName, m_ApplicationName.c_str(), XR_MAX_APPLICATION_NAME_SIZE);
    applicationInfo.applicationVersion = m_ApplicationVersion;
    strncpy_s(applicationInfo.engineName, AxrEngineName, XR_MAX_ENGINE_NAME_SIZE);
    applicationInfo.engineVersion = AXR_ENGINE_VERSION;
    // If we update to a newer version, make sure we update axrToString(XrResult xrResult) to include any new results
    applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 34);

    const XrInstanceCreateInfo instanceCreateInfo{
        .type = XR_TYPE_INSTANCE_CREATE_INFO,
        .next = instanceStructureChain,
        .createFlags = {},
        .applicationInfo = applicationInfo,
        .enabledApiLayerCount = static_cast<uint32_t>(layers.size()),
        .enabledApiLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .enabledExtensionNames = extensions.data(),
    };

    const XrResult xrResult = xrCreateInstance(&instanceCreateInfo, &m_Instance);
    axrLogXrResult(xrResult, "xrCreateInstance");

    destroyChain(instanceStructureChain);

    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrXrSystem::destroyInstance() {
    if (m_Instance == XR_NULL_HANDLE) return;

    xrDestroyInstance(m_Instance);
    m_Instance = XR_NULL_HANDLE;
}

XrBaseOutStructure* AxrXrSystem::createInstanceChain() const {
    XrBaseOutStructure* chain = nullptr;

    if (m_Extensions.exists(AXR_XR_EXTENSION_TYPE_DEBUG_UTILS)) {
        const XrDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo = createDebugUtilsCreateInfo();

        chain = appendNextChain(
            chain,
            reinterpret_cast<const XrBaseInStructure*>(&debugUtilsCreateInfo),
            sizeof(debugUtilsCreateInfo)
        );
    }

    return chain;
}

void AxrXrSystem::destroyChain(XrBaseOutStructure* chain) {
    XrBaseOutStructure* currentStructure = chain;

    while (currentStructure != nullptr) {
        XrBaseOutStructure* nextStructure = currentStructure->next;
        free(currentStructure);
        currentStructure = nextStructure;
    }
}

XrBaseOutStructure* AxrXrSystem::appendNextChain(
    XrBaseOutStructure* chain,
    const XrBaseInStructure* structure,
    const size_t structureSize
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (structure == nullptr) {
        axrLogErrorLocation("Structure is null.");
        return chain;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    // Copy the extension structure, so we can safely modify it.
    // Any structure that isn't on the end will need it's `next` variable modified to point to
    // the next extension structure.
    const auto structureCopy = static_cast<XrBaseOutStructure*>(malloc(structureSize));
    memcpy_s(structureCopy, structureSize, structure, structureSize);

    if (chain == nullptr) {
        // If it's the first structure in the chain, return it as the head of the chain
        return structureCopy;
    }

    // Get the end of the next chain
    XrBaseOutStructure* chainTail = chain;
    while (chainTail->next != nullptr) {
        chainTail = chainTail->next;
    }

    // Add the current structure to the end of the structure chain
    chainTail->next = structureCopy;

    // Return the structure chain
    return chain;
}

std::vector<const char*> AxrXrSystem::getAllApiLayerNames() const {
    std::vector<const char*> apiLayerNames;

    for (const AxrXrApiLayerConst_T apiLayer : m_ApiLayers) {
        if (apiLayer == nullptr) continue;

        apiLayerNames.push_back(axrGetXrApiLayerName(apiLayer->Type));
    }

    return apiLayerNames;
}

std::vector<const char*> AxrXrSystem::getAllExtensionNames() const {
    std::vector<const char*> extensionNames;

    for (const AxrXrExtensionConst_T extension : m_Extensions) {
        if (extension == nullptr) continue;

        extensionNames.push_back(axrGetXrExtensionName(extension->Type));
    }

    return extensionNames;
}

void AxrXrSystem::addRequiredExtensions() {
    if (m_GraphicsApi == AXR_GRAPHICS_API_VULKAN) {
        auto vulkanExtension = AxrXrExtensionVulkanEnable{
            .IsRequired = true,
        };
        m_Extensions.add(reinterpret_cast<AxrXrExtension_T>(&vulkanExtension));
    }
}

std::vector<std::string> AxrXrSystem::getSupportedApiLayers() const {
    uint32_t supportedApiLayersCount;
    XrResult xrResult = xrEnumerateApiLayerProperties(
        0,
        &supportedApiLayersCount,
        nullptr
    );
    axrLogXrResult(xrResult, "xrEnumerateApiLayerProperties");
    if (XR_FAILED(xrResult)) return {};

    std::vector<XrApiLayerProperties> supportedApiLayers(
        supportedApiLayersCount,
        {.type = XR_TYPE_API_LAYER_PROPERTIES}
    );
    xrResult = xrEnumerateApiLayerProperties(
        supportedApiLayersCount,
        &supportedApiLayersCount,
        supportedApiLayers.data()
    );
    axrLogXrResult(xrResult, "xrEnumerateApiLayerProperties");
    if (XR_FAILED(xrResult)) return {};

    std::vector<std::string> supportedApiLayerNames{};
    supportedApiLayerNames.reserve(supportedApiLayers.size());

    for (const XrApiLayerProperties apiLayer : supportedApiLayers) {
        supportedApiLayerNames.emplace_back(apiLayer.layerName);
    }

    return supportedApiLayerNames;
}

std::vector<std::string> AxrXrSystem::getSupportedExtensions() const {
    uint32_t supportedExtensionsCount;
    XrResult xrResult = xrEnumerateInstanceExtensionProperties(
        nullptr,
        0,
        &supportedExtensionsCount,
        nullptr
    );
    axrLogXrResult(xrResult, "xrEnumerateInstanceExtensionProperties");
    if (XR_FAILED(xrResult)) return {};

    std::vector<XrExtensionProperties> supportedExtensions(
        supportedExtensionsCount,
        {.type = XR_TYPE_EXTENSION_PROPERTIES}
    );
    xrResult = xrEnumerateInstanceExtensionProperties(
        nullptr,
        supportedExtensionsCount,
        &supportedExtensionsCount,
        supportedExtensions.data()
    );
    axrLogXrResult(xrResult, "xrEnumerateInstanceExtensionProperties");
    if (XR_FAILED(xrResult)) return {};

    std::vector<std::string> supportedExtensionNames{};
    supportedExtensionNames.reserve(supportedExtensions.size());

    for (XrExtensionProperties extension : supportedExtensions) {
        supportedExtensionNames.emplace_back(extension.extensionName);
    }

    return supportedExtensionNames;
}

void AxrXrSystem::removeUnsupportedApiLayers() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance != VK_NULL_HANDLE) {
        axrLogWarningLocation("Instance already exists. It's too late to remove api layers.");
        return;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const std::vector<std::string> supportedApiLayers = getSupportedApiLayers();

    for (auto it = m_ApiLayers.begin(); it != m_ApiLayers.end();) {
        if (*it == nullptr) {
            ++it;
            continue;
        }

        if (!axrContainsString(axrGetXrApiLayerName((*it)->Type), supportedApiLayers)) {
            axrLogWarning("Unsupported api layer: {0}", axrGetXrApiLayerName((*it)->Type));

            delete *it;
            it = m_ApiLayers.erase(it);
        } else {
            ++it;
        }
    }
}

AxrResult AxrXrSystem::removeUnsupportedExtensions() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance != VK_NULL_HANDLE) {
        axrLogWarningLocation("Instance already exists. It's too late to remove instance extensions.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    const std::vector<std::string> supportedExtensions = getSupportedExtensions();

    for (auto it = m_Extensions.begin(); it != m_Extensions.end();) {
        if (*it == nullptr) {
            ++it;
            continue;
        }

        if (!axrContainsString(axrGetXrExtensionName((*it)->Type), supportedExtensions)) {
            if ((*it)->IsRequired) {
                axrLogErrorLocation(
                    "Unsupported required extension: {0}.",
                    axrGetXrExtensionName((*it)->Type)
                );
                axrResult = AXR_ERROR;
                continue;
            }

            axrLogWarning("Unsupported extension: {0}.", axrGetXrExtensionName((*it)->Type));

            delete *it;
            it = m_Extensions.erase(it);
        } else {
            ++it;
        }
    }

    return axrResult;
}

XrDebugUtilsMessengerCreateInfoEXT AxrXrSystem::createDebugUtilsCreateInfo() const {
    const auto debugUtilsExtension = reinterpret_cast<AxrXrExtensionDebugUtils*>(
        m_Extensions.get(AXR_XR_EXTENSION_TYPE_DEBUG_UTILS)
    );

    if (debugUtilsExtension == nullptr) {
        return {};
    }

    return XrDebugUtilsMessengerCreateInfoEXT{
        .type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .next = nullptr,
        .messageSeverities = debugUtilsExtension->SeverityFlags,
        .messageTypes = debugUtilsExtension->TypeFlags,
        .userCallback = debugUtilsCallback,
    };
}

AxrResult AxrXrSystem::createDebugUtils() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_Extensions.exists(AXR_XR_EXTENSION_TYPE_DEBUG_UTILS)) {
        return AXR_SUCCESS;
    }

    if (m_DebugUtilsMessenger != XR_NULL_HANDLE) {
        axrLogErrorLocation("Debug Utils already exist.");
        return AXR_ERROR;
    }

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo = createDebugUtilsCreateInfo();

    // Create debug utils messenger
    const XrResult xrResult = xrCreateDebugUtilsMessengerEXT(
        m_Instance,
        &debugUtilsCreateInfo,
        &m_DebugUtilsMessenger
    );

    axrLogXrResult(xrResult, "xrCreateDebugUtilsMessengerEXT");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrXrSystem::destroyDebugUtils() {
    if (m_DebugUtilsMessenger == XR_NULL_HANDLE) return;

    const XrResult xrResult = xrDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugUtilsMessenger);
    axrLogXrResult(xrResult, "xrDestroyDebugUtilsMessengerEXT");

    if (XR_SUCCEEDED(xrResult)) {
        m_DebugUtilsMessenger = XR_NULL_HANDLE;
    }
}

AxrResult AxrXrSystem::setSystemId() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    if (m_SystemId != XR_NULL_SYSTEM_ID) {
        axrLogErrorLocation("System ID already exists.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    constexpr XrSystemGetInfo systemGetInfo{
        .type = XR_TYPE_SYSTEM_GET_INFO,
        .next = nullptr,
        .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY,
    };

    const XrResult xrResult = xrGetSystem(m_Instance, &systemGetInfo, &m_SystemId);
    axrLogXrResult(xrResult, "xrGetSystem");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrXrSystem::resetSystemId() {
    m_SystemId = XR_NULL_SYSTEM_ID;
}

void AxrXrSystem::logSystemDetails() const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return;
    }

    if (m_SystemId == XR_NULL_SYSTEM_ID) {
        axrLogErrorLocation("System ID is null.");
        return;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    XrInstanceProperties instanceProperties{
        .type = XR_TYPE_INSTANCE_PROPERTIES,
    };

    XrResult xrResult = xrGetInstanceProperties(m_Instance, &instanceProperties);
    axrLogXrResult(xrResult, "xrGetInstanceProperties");
    if (XR_FAILED(xrResult)) {
        return;
    }

    XrSystemProperties systemProperties{
        .type = XR_TYPE_SYSTEM_PROPERTIES,
    };

    xrResult = xrGetSystemProperties(m_Instance, m_SystemId, &systemProperties);
    axrLogXrResult(xrResult, "xrGetSystemProperties");
    if (XR_FAILED(xrResult)) {
        return;
    }

    axrLogInfo(
        "OpenXR Runtime: {0} - {1}.{2}.{3} | System name: {4} | Resolution: {5}:{6}",
        instanceProperties.runtimeName,
        XR_VERSION_MAJOR(instanceProperties.runtimeVersion),
        XR_VERSION_MINOR(instanceProperties.runtimeVersion),
        XR_VERSION_PATCH(instanceProperties.runtimeVersion),
        systemProperties.systemName,
        systemProperties.graphicsProperties.maxSwapchainImageWidth,
        systemProperties.graphicsProperties.maxSwapchainImageHeight
    );
}

AxrResult AxrXrSystem::setViewConfiguration() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (!m_ViewConfigurations.empty()) {
        axrLogErrorLocation("Views already exists.");
        return AXR_ERROR;
    }

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    if (m_SystemId == XR_NULL_SYSTEM_ID) {
        axrLogErrorLocation("System ID is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = getViewConfigurationType(m_ViewConfigurationType);
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    uint32_t viewConfigurationViewCount = 0;
    XrResult xrResult = xrEnumerateViewConfigurationViews(
        m_Instance,
        m_SystemId,
        m_ViewConfigurationType,
        0,
        &viewConfigurationViewCount,
        nullptr
    );
    axrLogXrResult(xrResult, "xrEnumerateViewConfigurationViews");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    std::vector<XrViewConfigurationView> viewConfigurationViews(
        viewConfigurationViewCount,
        {
            .type = XR_TYPE_VIEW_CONFIGURATION_VIEW
        }
    );

    xrResult = xrEnumerateViewConfigurationViews(
        m_Instance,
        m_SystemId,
        m_ViewConfigurationType,
        viewConfigurationViewCount,
        &viewConfigurationViewCount,
        viewConfigurationViews.data()
    );
    axrLogXrResult(xrResult, "xrEnumerateViewConfigurationViews");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    m_ViewConfigurations = viewConfigurationViews;

    return AXR_SUCCESS;
}

void AxrXrSystem::resetViewConfiguration() {
    m_ViewConfigurations.clear();
    m_ViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
}

AxrResult AxrXrSystem::getViewConfigurationType(XrViewConfigurationType& viewConfigurationType) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    if (m_SystemId == XR_NULL_SYSTEM_ID) {
        axrLogErrorLocation("SystemId is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    uint32_t viewConfigurationCount = 0;
    XrResult xrResult = xrEnumerateViewConfigurations(
        m_Instance,
        m_SystemId,
        0,
        &viewConfigurationCount,
        nullptr
    );
    axrLogXrResult(xrResult, "xrEnumerateViewConfigurations");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    std::vector<XrViewConfigurationType> viewConfigurations(viewConfigurationCount);
    xrResult = xrEnumerateViewConfigurations(
        m_Instance,
        m_SystemId,
        viewConfigurationCount,
        &viewConfigurationCount,
        viewConfigurations.data()
    );
    axrLogXrResult(xrResult, "xrEnumerateViewConfigurations");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    auto foundViewConfiguration = std::find_first_of(
        viewConfigurations.begin(),
        viewConfigurations.end(),
        m_SupportedViewConfigurationTypes.begin(),
        m_SupportedViewConfigurationTypes.end()
    );

    if (foundViewConfiguration != viewConfigurations.end()) {
        viewConfigurationType = *foundViewConfiguration;
        return AXR_SUCCESS;
    }

    axrLogErrorLocation("Failed to find a supported view configuration type.");
    return AXR_ERROR;
}

AxrResult AxrXrSystem::setEnvironmentBlendMode() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_EnvironmentBlendMode != XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM) {
        axrLogErrorLocation("Environment blend mode already exists.");
        return AXR_ERROR;
    }

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    if (m_SystemId == XR_NULL_SYSTEM_ID) {
        axrLogErrorLocation("SystemId is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    uint32_t environmentBlendModesCount;
    XrResult xrResult = xrEnumerateEnvironmentBlendModes(
        m_Instance,
        m_SystemId,
        m_ViewConfigurationType,
        0,
        &environmentBlendModesCount,
        nullptr
    );
    axrLogXrResult(xrResult, "xrEnumerateEnvironmentBlendModes");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    std::vector<XrEnvironmentBlendMode> environmentBlendModes(environmentBlendModesCount);
    xrResult = xrEnumerateEnvironmentBlendModes(
        m_Instance,
        m_SystemId,
        m_ViewConfigurationType,
        environmentBlendModesCount,
        &environmentBlendModesCount,
        environmentBlendModes.data()
    );
    axrLogXrResult(xrResult, "xrEnumerateEnvironmentBlendModes");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    const auto foundEnvironmentBlendMode = std::find_first_of(
        environmentBlendModes.begin(),
        environmentBlendModes.end(),
        m_SupportedEnvironmentBlendModes.begin(),
        m_SupportedEnvironmentBlendModes.end()
    );

    if (foundEnvironmentBlendMode != environmentBlendModes.end()) {
        m_EnvironmentBlendMode = *foundEnvironmentBlendMode;
        return AXR_SUCCESS;
    }

    axrLogErrorLocation("Failed to find a supported environment blend mode.");
    return AXR_ERROR;
}

void AxrXrSystem::resetEnvironmentBlendMode() {
    m_EnvironmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;
}

void AxrXrSystem::destroyGraphicsBinding() {
    if (m_GraphicsBinding == nullptr) return;

    delete m_GraphicsBinding;
    m_GraphicsBinding = nullptr;
}

void AxrXrSystem::destroySessionData() {
    detachActionSets();

    OnXrSessionStateChangedCallbackActions(false);
    OnXrSessionStateChangedCallbackGraphics(false);
    destroySpace(m_StageReferenceSpace);
    destroySession();
    m_IsSessionRunning = false;
}

AxrResult AxrXrSystem::createSession() {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Session != XR_NULL_HANDLE) {
        axrLogErrorLocation("Session already exists.");
        return AXR_ERROR;
    }

    if (m_Instance == XR_NULL_HANDLE) {
        axrLogErrorLocation("Instance is null.");
        return AXR_ERROR;
    }

    if (m_SystemId == XR_NULL_SYSTEM_ID) {
        axrLogErrorLocation("System ID is null.");
        return AXR_ERROR;
    }

    if (m_GraphicsBinding == nullptr) {
        axrLogErrorLocation("Graphics binding is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const XrSessionCreateInfo sessionCreateInfo{
        .type = XR_TYPE_SESSION_CREATE_INFO,
        .next = m_GraphicsBinding,
        .createFlags = {},
        .systemId = m_SystemId,
    };

    const XrResult xrResult = xrCreateSession(m_Instance, &sessionCreateInfo, &m_Session);
    axrLogXrResult(xrResult, "xrCreateSession");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

void AxrXrSystem::destroySession() {
    if (m_Session == XR_NULL_HANDLE) return;

    const XrResult xrResult = xrDestroySession(m_Session);
    axrLogXrResult(xrResult, "xrDestroySession");

    if (XR_SUCCEEDED(xrResult)) {
        m_Session = XR_NULL_HANDLE;
    }
}

AxrResult AxrXrSystem::getSupportedReferenceSpaces(std::vector<XrReferenceSpaceType>& referenceSpaces) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    uint32_t availableReferenceSpacesCount;
    XrResult xrResult = xrEnumerateReferenceSpaces(
        m_Session,
        0,
        &availableReferenceSpacesCount,
        nullptr
    );
    axrLogXrResult(xrResult, "xrEnumerateReferenceSpaces");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    std::vector<XrReferenceSpaceType> availableReferenceSpaces(availableReferenceSpacesCount);
    xrResult = xrEnumerateReferenceSpaces(
        m_Session,
        availableReferenceSpacesCount,
        &availableReferenceSpacesCount,
        availableReferenceSpaces.data()
    );
    axrLogXrResult(xrResult, "xrEnumerateReferenceSpaces");
    if (XR_FAILED(xrResult)) {
        return AXR_ERROR;
    }

    referenceSpaces = availableReferenceSpaces;
    return AXR_SUCCESS;
}

AxrResult AxrXrSystem::createReferenceSpace(
    XrReferenceSpaceType referenceSpaceType,
    XrSpace& referenceSpace
) const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (referenceSpace != XR_NULL_HANDLE) {
        axrLogErrorLocation("Reference space already exists.");
        return AXR_ERROR;
    }

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    std::vector<XrReferenceSpaceType> supportedReferenceSpaces;
    const AxrResult axrResult = getSupportedReferenceSpaces(supportedReferenceSpaces);
    if (AXR_FAILED(axrResult)) return axrResult;

    if (std::ranges::none_of(
        supportedReferenceSpaces,
        [referenceSpaceType](const XrReferenceSpaceType supportedReferenceSpace) -> bool {
            return supportedReferenceSpace == referenceSpaceType;
        }
    )) {
        axrLogWarningLocation(
            "Reference space type: {0} is not supported. Defaulting to using XR_REFERENCE_SPACE_TYPE_LOCAL.",
            static_cast<int32_t>(referenceSpaceType)
        );
        referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    }

    const XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{
        .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
        .next = nullptr,
        .referenceSpaceType = referenceSpaceType,
        .poseInReferenceSpace = XrPosef{
            .orientation = XrQuaternionf{
                .x = 0.0f,
                .y = 0.0f,
                .z = 0.0f,
                .w = 1.0f,
            },
            .position = XrVector3f{
                .x = 0.0f,
                .y = 0.0f,
                .z = 0.0f,
            }
        },
    };

    const XrResult xrResult = xrCreateReferenceSpace(m_Session, &referenceSpaceCreateInfo, &referenceSpace);
    axrLogXrResult(xrResult, "xrCreateReferenceSpace");
    if (XR_FAILED(xrResult)) return AXR_ERROR;

    return AXR_SUCCESS;
}

void AxrXrSystem::xrEvent_EventsLost(const XrEventDataEventsLost& eventData) {
    axrLogWarningLocation("OpenXR - Events Lost: {0}", eventData.lostEventCount);
}

void AxrXrSystem::xrEvent_InstanceLossPending(const XrEventDataInstanceLossPending& eventData) {
    axrLogWarningLocation("OpenXR - Instance Loss Pending at: {0}", eventData.lossTime);
    destroySessionData();
    resetSetup();
}

void AxrXrSystem::xrEvent_InteractionProfileChanged(const XrEventDataInteractionProfileChanged& eventData) {
    axrLogInfo("OpenXR - Interaction Profile changed.");

    if (eventData.session != m_Session) {
        return;
    }
}

void AxrXrSystem::xrEvent_ReferenceSpaceChangePending(const XrEventDataReferenceSpaceChangePending& eventData) {
    axrLogInfo("OpenXR - Reference Space Change pending.");

    if (eventData.session != m_Session) {
        return;
    }
}

void AxrXrSystem::xrEvent_SessionStateChanged(const XrEventDataSessionStateChanged& eventData) {
    if (eventData.session != m_Session) {
        return;
    }

    m_SessionState = eventData.state;

    // NOLINTNEXTLINE(clang-diagnostic-switch-enum)
    switch (m_SessionState) {
        case XR_SESSION_STATE_READY: {
            XrSessionBeginInfo sessionBeginInfo{.type = XR_TYPE_SESSION_BEGIN_INFO};
            sessionBeginInfo.primaryViewConfigurationType = m_ViewConfigurationType;
            const XrResult xrResult = xrBeginSession(m_Session, &sessionBeginInfo);
            axrLogXrResult(xrResult, "xrBeginSession");
            if (XR_SUCCEEDED(xrResult)) {
                OnXrSessionStateChangedCallbackGraphics(true);
                OnXrSessionStateChangedCallbackActions(true);
            }
            break;
        }
        case XR_SESSION_STATE_STOPPING: {
            const XrResult xrResult = xrEndSession(m_Session);
            axrLogXrResult(xrResult, "xrEndSession");
            destroySessionData();
            break;
        }
        case XR_SESSION_STATE_LOSS_PENDING:
        case XR_SESSION_STATE_EXITING: {
            destroySessionData();
            break;
        }
        default: {
            break;
        }
    }
}

void AxrXrSystem::syncAttachedActions() const {
    // ----------------------------------------- //
    // Validate
    // ----------------------------------------- //

    if (m_AttachedActionSets.empty()) {
        return;
    }

    if (m_Session == XR_NULL_HANDLE) {
        axrLogErrorLocation("Session is null.");
        return;
    }

    // The xrSyncActions function returns XR_SESSION_NOT_FOCUSED if application isn't focused.
    // So it's not worth calling it in this case
    if (!isSessionActive()) {
        return;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    std::vector<XrActiveActionSet> activeActionSets;
    activeActionSets.reserve(m_AttachedActionSets.size());

    for (const AxrActionSet* actionSet : m_AttachedActionSets | std::ranges::views::values) {
        if (actionSet == nullptr) continue;

        activeActionSets.push_back(
            XrActiveActionSet{
                .actionSet = actionSet->getXrActionSet(),
                .subactionPath = XR_NULL_PATH,
            }
        );
    }

    const XrActionsSyncInfo actionsSyncInfo = {
        .type = XR_TYPE_ACTIONS_SYNC_INFO,
        .next = nullptr,
        .countActiveActionSets = static_cast<uint32_t>(activeActionSets.size()),
        .activeActionSets = activeActionSets.data(),
    };

    const XrResult xrResult = xrSyncActions(m_Session, &actionsSyncInfo);
    axrLogXrResult(xrResult, "xrSyncActions");
    if (XR_FAILED(xrResult)) return;

    for (AxrActionSet* actionSet : m_AttachedActionSets | std::ranges::views::values) {
        if (actionSet == nullptr) continue;

        actionSet->updateXrActionValues();
    }
}

XrBool32 AxrXrSystem::debugUtilsCallback(
    const XrDebugUtilsMessageSeverityFlagsEXT messageSeverities,
    const XrDebugUtilsMessageTypeFlagsEXT messageTypes,
    const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* userData
) {
    AxrLogLevelEnum logLevel = AXR_LOG_LEVEL_ERROR;
    const char* messageSeverityString;
    const char* messageTypeString;

    switch (messageTypes) {
        case XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
            messageTypeString = "General";
            break;
        }
        case XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
            messageTypeString = "Validation";
            break;
        }
        case XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
            messageTypeString = "Performance";
            break;
        }
        case XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT: {
            messageTypeString = "Conformance";
            break;
        }
        default: {
            messageTypeString = "Unknown Type";
            break;
        }
    }

    switch (messageSeverities) {
        case XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
            messageSeverityString = "Verbose";
            logLevel = AXR_LOG_LEVEL_INFO;
            break;
        }
        case XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
            messageSeverityString = "Info";
            logLevel = AXR_LOG_LEVEL_INFO;
            break;
        }
        case XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
            messageSeverityString = "Warning";
            logLevel = AXR_LOG_LEVEL_WARNING;
            break;
        }
        case XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
            messageSeverityString = "Error";
            logLevel = AXR_LOG_LEVEL_ERROR;
            break;
        }
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            messageSeverityString = "Unknown Severity";
            logLevel = AXR_LOG_LEVEL_ERROR;
            break;
        }
    }

    axrLog(
        logLevel,
        "[OpenXR | {0} | {1}] : {2}",
        messageTypeString,
        messageSeverityString,
        pCallbackData->message
    );

    return VK_FALSE;
}
