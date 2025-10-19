#pragma once
#include "axr-hpp/application.hpp"

class SponzaScene {
public:
    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Constructor
    SponzaScene(axr::Application& app);
    /// Copy Constructor
    /// @param src Source SponzaScene to copy from
    SponzaScene(const SponzaScene& src) = delete;
    /// Move Constructor
    /// @param src Source SponzaScene to move from
    SponzaScene(SponzaScene&& src) noexcept = delete;

    // ---- Destructor ----

    /// Destructor
    ~SponzaScene();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source SponzaScene to copy from
    SponzaScene& operator=(const SponzaScene& src) = delete;
    /// Move Assignment Operator
    /// @param src Source SponzaScene to move from
    SponzaScene& operator=(SponzaScene&& src) noexcept = delete;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    axr::Result setup();
    axr::Result loadScene() const;
    axr::Result setAsActiveScene() const;

    void update();

    axr::UICanvasConfig uiCallback(axr::PlatformType platformType, Clay_Context* context);

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //
    axr::Application& m_Application;

    std::string m_SceneName = "SponzaScene";
    axr::Scene m_Scene;
    axr::Entity_T m_Entity;
    axr::Entity_T m_CameraEntity;
    axr::Entity_T m_XrHeadEntity;
    axr::Entity_T m_XrHandEntity;
    axr::ActionSet m_UIActionSet = axr::ActionSet(nullptr);

    axr::UIImageData m_UIImageData;
    float m_AccumulatedDeltaTime = 0.0f;
    uint32_t m_AccumulatedDeltaTimeCount = 0;
    std::string m_FpsString;
    std::string m_DeltaTimeString;
    uint16_t m_FontID = 0;
};
