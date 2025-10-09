// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/application.h"
#include "application.hpp"
#include "axr/logger.h"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <chrono>

// ----------------------------------------- //
// External Functions
// ----------------------------------------- //

AxrApplication_T axrCreateApplication(const AxrApplicationConfig* config) {
    if (config == nullptr) {
        axrLogErrorLocation("`config` is null.");
        return nullptr;
    }

    return new AxrApplication(*config);
}

void axrDestroyApplication(AxrApplication_T* app) {
    if (app == nullptr || *app == nullptr) return;

    delete *app;
    *app = nullptr;
}

AxrResult axrApplicationSetup(const AxrApplication_T app) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return AXR_ERROR;
    }

    return app->setup();
}

bool axrApplicationIsRunning(const AxrApplicationConst_T app) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return false;
    }

    return app->isRunning();
}

void axrApplicationProcessEvents(const AxrApplication_T app) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return;
    }

    app->processEvents();
}

AxrWindowSystem_T axrApplicationGetWindowSystem(const AxrApplication_T app) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return nullptr;
    }

    return app->getWindowSystem();
}

AxrXrSystem_T axrApplicationGetXrSystem(const AxrApplication_T app) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return nullptr;
    }

    return app->getXrSystem();
}

AxrGraphicsSystem_T axrApplicationGetGraphicsSystem(const AxrApplication_T app) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return nullptr;
    }

    return app->getGraphicsSystem();
}

AxrActionSystem_T axrApplicationGetActionSystem(const AxrApplication_T app) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return nullptr;
    }

    return app->getActionSystem();
}

AxrAssetCollection_T axrApplicationGetGlobalAssetCollection(const AxrApplication_T app) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return nullptr;
    }

    return app->getGlobalAssetCollection();
}

float axrApplicationGetDeltaTime(const AxrApplication_T app) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return 0;
    }

    return app->getDeltaTime();
}

AxrResult axrApplicationCreateScene(const AxrApplication_T app, char sceneName[AXR_MAX_SCENE_NAME_SIZE]) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return AXR_ERROR;
    }

    return app->createScene(sceneName == nullptr ? "" : sceneName);
}

AxrScene_T axrApplicationFindScene(const AxrApplication_T app, const char* sceneName) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return nullptr;
    }

    return app->findScene(sceneName);
}

AxrResult axrApplicationLoadScene(const AxrApplication_T app, const char* sceneName) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return AXR_ERROR;
    }

    return app->loadScene(sceneName);
}

AxrResult axrApplicationSetActiveScene(const AxrApplication_T app, const char* sceneName) {
    if (app == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return AXR_ERROR;
    }

    return app->setActiveScene(sceneName);
}

Clay_Dimensions axrApplicationHandleClayMeasureText(
    Clay_StringSlice text,
    Clay_TextElementConfig* config,
    void* app
) {
    auto appCast = static_cast<AxrApplication_T>(app);
    if (appCast == nullptr) {
        axrLogErrorLocation("`app` is null.");
        return {};
    }

    return appCast->measureClayText(text, config);
}

// ----------------------------------------- //
// Internal Functions
// ----------------------------------------- //

// ---- Special Functions ----

AxrApplication::AxrApplication(const AxrApplicationConfig& config):
    m_ApplicationName(config.ApplicationName),
    m_ApplicationVersion(config.ApplicationVersion),
    m_GraphicsSystem(
        AxrGraphicsSystem::Config{
            .ApplicationName = config.ApplicationName,
            .ApplicationVersion = config.ApplicationVersion,
            .WindowSystem = config.WindowSystemConfig == nullptr ? nullptr : &m_WindowSystem,
            .XrSystem = config.XrSystemConfig == nullptr ? nullptr : &m_XrSystem,
            .GlobalAssetCollection = &m_GlobalAssetCollection,
            .GraphicsConfig = config.GraphicsSystemConfig,
        }
    ),
    m_WindowSystem(
        [&]() -> AxrWindowSystem {
            if (config.WindowSystemConfig == nullptr) {
                return AxrWindowSystem(nullptr);
            }

            return AxrWindowSystem(
                AxrWindowSystem::Config{
                    .ApplicationName = config.ApplicationName,
                    .ActionSystem = &m_ActionSystem,
                    .Width = config.WindowSystemConfig->Width,
                    .Height = config.WindowSystemConfig->Height,
                }
            );
        }()
    ),
    m_XrSystem(
        [config]() -> AxrXrSystem {
            if (config.XrSystemConfig == nullptr) {
                return AxrXrSystem(nullptr);
            }

            return AxrXrSystem(
                AxrXrSystem::Config{
                    .ApplicationName = config.ApplicationName,
                    .ApplicationVersion = config.ApplicationVersion,
                    .GraphicsApi = config.GraphicsSystemConfig.GraphicsApi,
                    .StageReferenceSpace = config.XrSystemConfig->StageReferenceSpace,
                    .ApiLayerCount = config.XrSystemConfig->ApiLayerCount,
                    .ApiLayers = config.XrSystemConfig->ApiLayers,
                    .ExtensionCount = config.XrSystemConfig->ExtensionCount,
                    .Extensions = config.XrSystemConfig->Extensions
                }
            );
        }()
    ),
    m_ActionSystem(
        AxrActionSystem::Config{
            .XrSystem = config.XrSystemConfig == nullptr ? nullptr : &m_XrSystem,
            .ActionSetCount = config.ActionSystemConfig.ActionSetCount,
            .ActionSets = config.ActionSystemConfig.ActionSets,
            .XrInteractionProfileCount = config.ActionSystemConfig.XrInteractionProfileCount,
            .XrInteractionProfiles = config.ActionSystemConfig.XrInteractionProfiles
        }
    ),
    m_GlobalAssetCollection(true, config.GraphicsSystemConfig.GraphicsApi),
    m_ClayContext(nullptr),
    m_ClayArena(),
    m_DeltaTime(0) {
}

AxrApplication::~AxrApplication() {
    m_GraphicsSystem.resetSetup();
    m_ActionSystem.resetSetup();
    resetSetupClay();
    m_WindowSystem.resetSetup();
    m_XrSystem.resetSetup();
    m_GlobalAssetCollection.cleanup();
}

// ---- Public Functions ----

AxrResult AxrApplication::setup() {
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = setupGlobalAssetCollection();
    if (AXR_FAILED(axrResult)) return axrResult;

    if (m_WindowSystem.isValid()) {
        axrResult = m_WindowSystem.setup();
        if (AXR_FAILED(axrResult)) return axrResult;
    }

    if (m_XrSystem.isValid()) {
        axrResult = m_XrSystem.setup();
        if (AXR_FAILED(axrResult)) return axrResult;
    }

    axrResult = setupClay();
    if (AXR_FAILED(axrResult)) return axrResult;

    axrResult = m_ActionSystem.setup();
    if (AXR_FAILED(axrResult)) return axrResult;

    axrResult = m_GraphicsSystem.setup();
    if (AXR_FAILED(axrResult)) return axrResult;

    return AXR_SUCCESS;
}

bool AxrApplication::isRunning() const {
    bool windowIsOpen = false;
    bool xrSessionIsRunning = false;

    if (m_WindowSystem.isValid()) {
        windowIsOpen = m_WindowSystem.isWindowOpen();
    }

    if (m_XrSystem.isValid()) {
        xrSessionIsRunning = m_XrSystem.isXrSessionRunning();
    }

    return windowIsOpen || xrSessionIsRunning;
}

void AxrApplication::processEvents() {
    m_ActionSystem.newFrameStarted();

    if (m_WindowSystem.isValid()) {
        m_WindowSystem.processEvents();
    }
    if (m_XrSystem.isValid()) {
        m_XrSystem.processEvents();
    }

    m_ActionSystem.processEvents();

    static std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
    const std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> deltaTime = currentFrameTime - lastFrameTime;
    m_DeltaTime = deltaTime.count();
    lastFrameTime = currentFrameTime;
}

AxrWindowSystem_T AxrApplication::getWindowSystem() {
    return &m_WindowSystem;
}

AxrXrSystem_T AxrApplication::getXrSystem() {
    return &m_XrSystem;
}

AxrGraphicsSystem_T AxrApplication::getGraphicsSystem() {
    return &m_GraphicsSystem;
}

AxrActionSystem_T AxrApplication::getActionSystem() {
    return &m_ActionSystem;
}

AxrAssetCollection_T AxrApplication::getGlobalAssetCollection() {
    return &m_GlobalAssetCollection;
}

float AxrApplication::getDeltaTime() const {
    return m_DeltaTime;
}

AxrResult AxrApplication::createScene(const std::string& sceneName) {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_Scenes.contains(sceneName)) {
        axrLogErrorLocation("Scene already exists with the name: {0}.", sceneName.c_str());
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    const auto insertResult = m_Scenes.insert(
        std::pair(sceneName, AxrScene(sceneName, m_GraphicsSystem.getGraphicsApi()))
    );
    if (!insertResult.second) {
        // If the insertion failed
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

AxrScene_T AxrApplication::findScene(const std::string& sceneName) {
    const auto foundScene = m_Scenes.find(sceneName);
    if (foundScene == m_Scenes.end()) {
        axrLogErrorLocation("Failed to find scene with the name: {0}.", sceneName.c_str());
        return nullptr;
    }

    return &foundScene->second;
}

AxrResult AxrApplication::loadScene(const std::string& sceneName) {
    const AxrScene_T foundScene = findScene(sceneName);
    if (foundScene == nullptr) {
        axrLogErrorLocation("Failed to find scene with the name: {0}.", sceneName.c_str());
        return AXR_ERROR;
    }

    return m_GraphicsSystem.loadScene(foundScene);
}

AxrResult AxrApplication::setActiveScene(const std::string& sceneName) {
    return m_GraphicsSystem.setActiveScene(sceneName);
}

Clay_Dimensions AxrApplication::measureClayText(Clay_StringSlice text, Clay_TextElementConfig* config) const {
    // TODO: Do this properly
    return Clay_Dimensions{
        .width = static_cast<float>(text.length * config->fontSize),
        .height = static_cast<float>(config->fontSize)
    };
}

// ---- Private Functions ----

AxrResult AxrApplication::setupGlobalAssetCollection() {
    AxrResult axrResult = AXR_SUCCESS;

    axrResult = m_GlobalAssetCollection.createImageSampler(AXR_ENGINE_ASSET_IMAGE_SAMPLER_NEAREST_REPEAT);
    if (AXR_FAILED(axrResult)) return axrResult;

    axrResult = m_GlobalAssetCollection.createImageSampler(AXR_ENGINE_ASSET_IMAGE_SAMPLER_LINEAR_REPEAT);
    if (AXR_FAILED(axrResult)) return axrResult;

    axrResult = m_GlobalAssetCollection.createImage(AXR_ENGINE_ASSET_IMAGE_MISSING_TEXTURE);
    if (AXR_FAILED(axrResult)) return axrResult;

    // ---- UI Assets ----

    axrResult = m_GlobalAssetCollection.createShader(AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT);
    if (AXR_FAILED(axrResult)) return axrResult;

    axrResult = m_GlobalAssetCollection.createShader(AXR_ENGINE_ASSET_SHADER_UI_RECTANGLE_FRAG);
    if (AXR_FAILED(axrResult)) return axrResult;

    axrResult = m_GlobalAssetCollection.createShader(AXR_ENGINE_ASSET_SHADER_UI_BORDER_FRAG);
    if (AXR_FAILED(axrResult)) return axrResult;

    axrResult = m_GlobalAssetCollection.createShader(AXR_ENGINE_ASSET_SHADER_UI_IMAGE_FRAG);
    if (AXR_FAILED(axrResult)) return axrResult;

    axrResult = m_GlobalAssetCollection.createModel(AXR_ENGINE_ASSET_MODEL_UI_RECTANGLE);
    if (AXR_FAILED(axrResult)) return axrResult;

    return AXR_SUCCESS;
}

AxrResult AxrApplication::setupClay() {
    const uint64_t totalMemorySize = Clay_MinMemorySize();
    m_ClayArena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));

    m_ClayContext = Clay_Initialize(
        m_ClayArena,
        Clay_Dimensions{
            // These values will get set during rendering, depending on the platform that's being rendered
            .width = 0, .height = 0
        },
        Clay_ErrorHandler{
            // ReSharper disable once CppPassValueParameterByConstReference
            .errorHandlerFunction = [](const Clay_ErrorData errorData) -> void {
                const auto application = static_cast<AxrApplication*>(errorData.userData);
                application->handleClayErrors(errorData);
            },
            .userData = this,
        }
    );

    Clay_SetMeasureTextFunction(axrApplicationHandleClayMeasureText, this);

    // We chose 256 since most vulkan gpus have a uniform buffer range of 65536 or more. And the worst possible offset
    // alignment is 256. So as long as sizeof(AxrEngineAssetUniformBuffer_UIElement) is less than 256, then we can
    // have a max of 65536 / 256 = 256 elements.
    // NOTE: If we need more than we should use a dynamic storage buffer instead of a dynamic uniform buffer.
    //  Or we just accept having a max of 128 elements instead (65536 / 512 = 128).
    Clay_SetMaxElementCount(256);
    static_assert(
        sizeof(AxrEngineAssetUniformBuffer_UIElement) <= 256,
        "UI Element size is larger than 256 bytes. Consider changing to a dynamic storage buffer instead."
    );

    return AXR_SUCCESS;
}

void AxrApplication::resetSetupClay() {
    if (m_ClayContext == nullptr || m_ClayArena.memory == nullptr) return;

    bool resetCurrentContext = false;
    if (Clay_GetCurrentContext() == m_ClayContext) {
        resetCurrentContext = true;
    }

    free(m_ClayArena.memory);
    m_ClayArena = {};
    m_ClayContext = nullptr;

    if (resetCurrentContext) {
        Clay_SetCurrentContext(nullptr);
    }
}

void AxrApplication::handleClayErrors(const Clay_ErrorData& errorData) const {
    const char* messageTypeString = axrToString(errorData.errorType);

    axrLogError(
        "[Clay | XR Graphics | {0}] : {1}",
        messageTypeString,
        errorData.errorText.chars
    );
}
