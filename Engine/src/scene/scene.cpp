// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "scene.hpp"
#include "axr/logger.h"

// ----------------------------------------- //
// External Functions
// ----------------------------------------- //

const char* axrSceneGetName(const AxrSceneConst_T scene) {
    if (scene == nullptr) {
        axrLogErrorLocation("`scene` is null");
        return "";
    }

    return scene->getName().c_str();
}

AxrAssetCollection_T axrSceneGetAssetCollection(const AxrScene_T scene) {
    if (scene == nullptr) {
        axrLogErrorLocation("`scene` is null");
        return nullptr;
    }

    return scene->getAssetCollection();
}

entt::registry* axrSceneGetEcsRegistry(const AxrScene_T scene) {
    if (scene == nullptr) {
        axrLogErrorLocation("`scene` is null");
        return nullptr;
    }

    return scene->getEcsRegistry();
}

entt::entity axrSceneCreateEntity(const AxrScene_T scene) {
    if (scene == nullptr) {
        axrLogErrorLocation("`scene` is null");
        return {};
    }

    return scene->createEntity();
}

void axrSceneSetMainCamera(const AxrScene_T scene, const AxrEntityConst_T entity) {
    if (scene == nullptr) {
        axrLogErrorLocation("`scene` is null");
        return;
    }

    return scene->setMainCamera(entity);
}

void axrSceneSetUIImageResourcesPreloadCount(const AxrScene_T scene, const uint32_t imageCount) {
    if (scene == nullptr) {
        axrLogErrorLocation("`scene` is null");
        return;
    }

    return scene->setUIImageResourcesPreloadCount(imageCount);
}

AxrResult axrSceneSetBuildUICanvasCallback(
    const AxrScene_T scene,
    void* userData,
    const AxrBuildUICanvasCallback_T buildCanvasCallback
) {
    if (scene == nullptr) {
        axrLogErrorLocation("`scene` is null");
        return AXR_ERROR;
    }

    return scene->setBuildUICanvasCallback(userData, buildCanvasCallback);
}

// ----------------------------------------- //
// Internal Functions
// ----------------------------------------- //

// ---- Special Functions ----

AxrScene::AxrScene() = default;

AxrScene::AxrScene(const std::string& name, const AxrGraphicsApiEnum graphicsApi):
    m_Name(name),
    m_AssetCollection(graphicsApi),
    m_MainCamera(m_Registry, entt::null) {
}

AxrScene::AxrScene(AxrScene&& src) noexcept {
    m_Name = std::move(src.m_Name);
    m_AssetCollection = std::move(src.m_AssetCollection);
    m_Registry = std::move(src.m_Registry);

    m_MainCamera = src.m_MainCamera;
    m_UIImageResourcesPreloadCount = src.m_UIImageResourcesPreloadCount;
    m_BuildUICanvasCallback = src.m_BuildUICanvasCallback;

    src.m_MainCamera = AxrEntity_T{m_Registry, entt::null};
    src.m_UIImageResourcesPreloadCount = 0;
    src.m_BuildUICanvasCallback = {};
}

AxrScene::~AxrScene() {
    cleanup();
}

AxrScene& AxrScene::operator=(AxrScene&& src) noexcept {
    if (this != &src) {
        cleanup();

        m_Name = std::move(src.m_Name);
        m_AssetCollection = std::move(src.m_AssetCollection);
        m_Registry = std::move(src.m_Registry);

        m_MainCamera = src.m_MainCamera;
        m_UIImageResourcesPreloadCount = src.m_UIImageResourcesPreloadCount;
        m_BuildUICanvasCallback = src.m_BuildUICanvasCallback;

        src.m_MainCamera = AxrEntity_T{m_Registry, entt::null};
        src.m_UIImageResourcesPreloadCount = 0;
        src.m_BuildUICanvasCallback = {};
    }

    return *this;
}

const std::string& AxrScene::getName() const {
    return m_Name;
}

// ---- Public Functions ----

AxrAssetCollection_T AxrScene::getAssetCollection() {
    return &m_AssetCollection;
}

entt::registry* AxrScene::getEcsRegistry() {
    return &m_Registry;
}

entt::entity AxrScene::createEntity() {
    return m_Registry.create();
}

void AxrScene::setMainCamera(const AxrEntityConst_T entity) {
    auto [cameraComponent, transformComponent] = entity.try_get<AxrCameraComponent, AxrTransformComponent>();
    if (cameraComponent == nullptr) {
        axrLogErrorLocation("Entity doesn't have a camera component.");
        return;
    }

    if (transformComponent == nullptr) {
        axrLogErrorLocation("Entity doesn't have a transform component.");
        return;
    }

    m_MainCamera = entity;
}

void AxrScene::setUIImageResourcesPreloadCount(const uint32_t imageCount) {
    m_UIImageResourcesPreloadCount = imageCount;
}

AxrResult AxrScene::setBuildUICanvasCallback(void* userData, const AxrBuildUICanvasCallback_T buildCanvasCallback) {
    if (buildCanvasCallback == nullptr) {
        return AXR_ERROR;
    }

    m_BuildUICanvasCallback = CallbackData{
        .UserData = userData,
        .Function = buildCanvasCallback,
    };

    return AXR_SUCCESS;
}

bool AxrScene::isMainCameraValid() const {
    if (m_MainCamera == entt::null) {
        return false;
    }

    auto [cameraComponent, transformComponent] = m_MainCamera.try_get<AxrCameraComponent, AxrTransformComponent>();
    if (cameraComponent == nullptr || transformComponent == nullptr) {
        return false;
    }

    return true;
}

AxrEntityConst_T AxrScene::getMainCamera() const {
    return m_MainCamera;
}

uint32_t AxrScene::getUIImageResourcesPreloadCount() const {
    return m_UIImageResourcesPreloadCount;
}

const AxrScene::CallbackData& AxrScene::getUICanvasCallback() const {
    return m_BuildUICanvasCallback;
}

// ---- Private Functions ----

void AxrScene::cleanup() {
    m_Name.clear();
    m_Registry.clear();
    m_AssetCollection.cleanup();
    m_MainCamera = {m_Registry, entt::null};
    m_UIImageResourcesPreloadCount = 0;
    m_BuildUICanvasCallback = {};
}
