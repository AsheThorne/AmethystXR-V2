// ----------------------------------------- //
// Win32 Headers
// ----------------------------------------- //
#include <windows.h>

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include <axr.hpp>
#include "sponzaScene.hpp"
#include "alphaTestScene.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
    const std::string applicationName = "Sandbox";

    const auto engineSetupConfig = axr::SetupConfig(axr::LogLevelEnum::Info);
    axr::setup(engineSetupConfig);
    axr::loggerSetup(applicationName);

    const axr::VulkanWindowConfig vulkanWindowConfig(axr::VulkanPresentationModeEnum::Mailbox);

    axr::VulkanApiConfig vulkanApiConfig(&vulkanWindowConfig);

#ifndef NDEBUG
    vulkanApiConfig.addApiLayerCoreValidation();
    vulkanApiConfig.addExtensionDebugUtils(
        false,
        // axr::VulkanDebugUtilsSeverityFlagBits::VerboseBit |
        // axr::VulkanDebugUtilsSeverityFlagBits::InfoBit |
        axr::VulkanDebugUtilsSeverityFlagBits::WarningBit |
        axr::VulkanDebugUtilsSeverityFlagBits::ErrorBit,
        axr::VulkanDebugUtilsTypeFlagBits::GeneralBit |
        axr::VulkanDebugUtilsTypeFlagBits::ValidationBit |
        axr::VulkanDebugUtilsTypeFlagBits::PerformanceBit |
        axr::VulkanDebugUtilsTypeFlagBits::DeviceAddressBindingBit
    );
#endif

    const axr::GraphicsWindowConfig windowGraphicsConfig(axr::MsaaSampleCountEnum::SampleCount64);
    const axr::GraphicsXrSessionConfig xrSessionGraphicsConfig(axr::MsaaSampleCountEnum::SampleCount1);

    const axr::GraphicsSystemConfig graphicsSystemConfig(
        &vulkanApiConfig,
        &windowGraphicsConfig,
        &xrSessionGraphicsConfig,
        axr::SamplerAnisotropyQualityEnum::High
    );

    axr::ActionSystemConfig actionSystemConfig(
        std::vector{
            axr::ActionSetConfig(
                "movement",
                "Movement",
                std::vector<axr::BoolInputActionConfig>{
                    axr::BoolInputActionConfig(
                        "forward",
                        "Forward",
                        axr::ActionXrVisibilityEnum::Auto,
                        std::vector{
                            axr::BoolInputActionEnum::KeyboardW,
                        }
                    ),
                    axr::BoolInputActionConfig(
                        "left",
                        "Left",
                        axr::ActionXrVisibilityEnum::Auto,
                        std::vector{
                            axr::BoolInputActionEnum::KeyboardA,
                        }
                    ),
                    axr::BoolInputActionConfig(
                        "back",
                        "Back",
                        axr::ActionXrVisibilityEnum::Auto,
                        std::vector{
                            axr::BoolInputActionEnum::KeyboardS,
                        }
                    ),
                    axr::BoolInputActionConfig(
                        "right",
                        "Right",
                        axr::ActionXrVisibilityEnum::Auto,
                        std::vector{
                            axr::BoolInputActionEnum::KeyboardD,
                        }
                    ),
                },
                std::vector<axr::FloatInputActionConfig>{},
                std::vector<axr::Vec2InputActionConfig>{
                    axr::Vec2InputActionConfig(
                        "mouse",
                        "Mouse",
                        axr::ActionXrVisibilityEnum::Auto,
                        std::vector{
                            axr::Vec2InputActionEnum::MouseMoved,
                        }
                    ),
                },
                std::vector<axr::PoseInputActionConfig>{},
                std::vector<axr::HapticOutputActionConfig>{}
            ),
            axr::ActionSetConfig(
                "test",
                "Test",
                std::vector<axr::BoolInputActionConfig>{
                    axr::BoolInputActionConfig(
                        "click",
                        "Click",
                        axr::ActionXrVisibilityEnum::Always,
                        std::vector{
                            axr::BoolInputActionEnum::Keyboard1,
                            axr::BoolInputActionEnum::XrController_Left_A_Click,
                            axr::BoolInputActionEnum::XrController_Right_A_Click,
                        }
                    ),
                },
                std::vector<axr::FloatInputActionConfig>{},
                std::vector<axr::Vec2InputActionConfig>{},
                std::vector<axr::PoseInputActionConfig>{
                    axr::PoseInputActionConfig(
                        "head",
                        "Head",
                        axr::ActionXrVisibilityEnum::Auto,
                        axr::PoseInputActionEnum::XrHMD
                    ),
                    axr::PoseInputActionConfig(
                        "righthand",
                        "Right Hand",
                        axr::ActionXrVisibilityEnum::Auto,
                        axr::PoseInputActionEnum::XrController_Right_Grip
                    ),
                },
                std::vector<axr::HapticOutputActionConfig>{
                    axr::HapticOutputActionConfig(
                        "controller",
                        "Controller",
                        axr::ActionXrVisibilityEnum::Auto,
                        std::vector{
                            axr::HapticOutputActionEnum::XrController_Left,
                            axr::HapticOutputActionEnum::XrController_Right,
                        }
                    ),
                }
            ),
            axr::ActionSetConfig(
                "ui",
                "UI",
                std::vector<axr::BoolInputActionConfig>{
                    axr::BoolInputActionConfig(
                        axr::BoolInputActionConfig(
                            "click",
                            "Click",
                            axr::ActionXrVisibilityEnum::Auto,
                            std::vector{
                                axr::BoolInputActionEnum::MouseClickL,
                            }
                        )
                    ),
                },
                std::vector<axr::FloatInputActionConfig>{
                    axr::FloatInputActionConfig(
                        "scroll",
                        "Scroll",
                        axr::ActionXrVisibilityEnum::Auto,
                        std::vector{
                            axr::FloatInputActionEnum::MouseWheel,
                        }
                    ),
                },
                std::vector<axr::Vec2InputActionConfig>{
                    axr::Vec2InputActionConfig(
                        "cursor_position",
                        "Cursor Position",
                        axr::ActionXrVisibilityEnum::Auto,
                        std::vector{
                            axr::Vec2InputActionEnum::MousePosition,
                        }
                    ),
                },
                std::vector<axr::PoseInputActionConfig>{},
                std::vector<axr::HapticOutputActionConfig>{}
            ),
        },
        std::vector<axr::XrInteractionProfileEnum>{
            axr::XrInteractionProfileEnum::ValveIndexController,
        }
    );

    const axr::WindowSystemConfig windowSystemConfig(
        800,
        600
    );

    axr::XrSystemConfig xrSystemConfig(axr::XrReferenceSpaceEnum::Stage);

#ifndef NDEBUG
    xrSystemConfig.addApiLayerCoreValidation();
    xrSystemConfig.addExtensionDebugUtils(
        false,
        // axr::XrDebugUtilsSeverityFlagBits::VerboseBit |
        // axr::XrDebugUtilsSeverityFlagBits::InfoBit |
        axr::XrDebugUtilsSeverityFlagBits::WarningBit |
        axr::XrDebugUtilsSeverityFlagBits::ErrorBit,
        axr::XrDebugUtilsTypeFlagBits::GeneralBit |
        axr::XrDebugUtilsTypeFlagBits::ValidationBit |
        axr::XrDebugUtilsTypeFlagBits::PerformanceBit |
        axr::XrDebugUtilsTypeFlagBits::ConformanceBit
    );
#endif

    const auto appConfig = axr::ApplicationConfig(
        applicationName.c_str(),
        AXR_MAKE_VERSION(1, 0, 0),
        graphicsSystemConfig,
        actionSystemConfig,
        &windowSystemConfig,
        nullptr
    );

    auto app = axr::Application(appConfig);
    if (AXR_FAILED(app.setup())) return -1;

    if (AXR_FAILED(app.getGlobalAssetCollection().createFont(axr::EngineAssetEnum::FontJetbrainsMono_Regular)))
        return -1;

    SponzaScene scene(app);
    if (AXR_FAILED(scene.setup())) return -1;

    if (AXR_FAILED(scene.loadScene())) return -1;
    if (AXR_FAILED(scene.setAsActiveScene())) return -1;

    axr::WindowSystem windowSystem = app.getWindowSystem();
    if (windowSystem.isValid()) {
        if (AXR_FAILED(windowSystem.openWindow())) return -1;
    }

    axr::XrSystem xrSystem = app.getXrSystem();
    if (xrSystem.isValid()) {
        xrSystem.setClippingPlane(0.01f, 1000.0f);
        if (AXR_FAILED(xrSystem.startXrSession())) return -1;
    }

    axr::GraphicsSystem graphicsSystem = app.getGraphicsSystem();
    graphicsSystem.setClearColor(axr::Color(0.2f, 0.05f, 0.2f, 1.0f));
    graphicsSystem.setWindowRenderSource(axr::WindowRenderSourceEnum::SceneMainCamera);

    axr::ActionSystem actionSystem = app.getActionSystem();
    axr::ActionSet actionSet = actionSystem.getActionSet("test");
    axr::BoolInputAction clickAction = actionSet.getBoolInputAction("click");

    while (app.isRunning()) {
        app.processEvents();

        if (!windowSystem.isWindowOpen()) {
            xrSystem.stopXrSession();
            app.processEvents();
            if (!app.isRunning()) {
                break;
            }
        }

        if (clickAction.valueChanged() && clickAction.getValue()) {
            switch (graphicsSystem.getWindowRenderSource()) {
                case axr::WindowRenderSourceEnum::SceneMainCamera: {
                    graphicsSystem.setWindowRenderSource(axr::WindowRenderSourceEnum::XrDeviceLeftEye);
                    break;
                }
                case axr::WindowRenderSourceEnum::XrDeviceLeftEye: {
                    graphicsSystem.setWindowRenderSource(axr::WindowRenderSourceEnum::XrDeviceRightEye);
                    break;
                }
                case axr::WindowRenderSourceEnum::XrDeviceRightEye: {
                    graphicsSystem.setWindowRenderSource(axr::WindowRenderSourceEnum::XrDeviceBothEyes);
                    break;
                }
                case axr::WindowRenderSourceEnum::XrDeviceBothEyes: {
                    graphicsSystem.setWindowRenderSource(axr::WindowRenderSourceEnum::SceneMainCamera);
                    break;
                }
            }
        }

        scene.update();

        graphicsSystem.drawFrame();
    }

    return 0;
}
