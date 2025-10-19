#include "sponzaScene.hpp"

SponzaScene::SponzaScene(axr::Application& app):
    m_Application(app),
    m_Scene(nullptr) {
}

SponzaScene::~SponzaScene() {
}

axr::Result SponzaScene::setup() {
    if (AXR_FAILED(m_Application.createScene(m_SceneName.c_str()))) return axr::Result::Error;
    m_Scene = m_Application.findScene(m_SceneName.c_str());
    axr::AssetCollection assetCollection = m_Scene.getAssetCollection();

    const char* defaultMaterialName = "SponzaDefaultMaterial";
    if (AXR_FAILED(
        assetCollection.createMaterial(
            defaultMaterialName,
            axr::EngineAssetMaterial_DefaultMaterial(
                axr::MaterialBackfaceCullModeEnum::Back,
                axr::MaterialAlphaRenderModeEnum::Opaque,
                "",
                axr::engineAssetGetName(axr::EngineAssetEnum::ImageMissingTexture),
                axr::engineAssetGetName(axr::EngineAssetEnum::ImageSamplerNearestRepeat)
            )
        )
    ))
        return axr::Result::Error;

    const std::string modelName = "SponzaModel";
    const axr::ModelConfig modelConfig(
        modelName.c_str(),
        "sample-models/2.0/Sponza/glTF/Sponza.gltf"
    );

    if (AXR_FAILED(assetCollection.createModel(modelConfig))) return axr::Result::Error;

    axr::ModelFileInfo modelInfo;
    if (AXR_FAILED(axr::getModelFileData(modelConfig.FilePath, modelInfo))) return axr::Result::Error;

    std::vector<std::string> imageSamplerNames(modelInfo.ImageSamplerCount, "");
    for (int i = 0; i < modelInfo.ImageSamplerCount; ++i) {
        if (!axrStringIsEmpty(modelInfo.ImageSamplers[i].Name)) {
            imageSamplerNames[i] = modelInfo.ImageSamplers[i].Name;
        } else {
            imageSamplerNames[i] = modelName + "_ImageSampler(" + std::to_string(i + 1) + ")";
        }

        axr::ImageSamplerConfig config(
            imageSamplerNames[i].c_str(),
            modelInfo.ImageSamplers[i].MinFilter,
            modelInfo.ImageSamplers[i].MagFilter,
            modelInfo.ImageSamplers[i].MipmapFilter,
            modelInfo.ImageSamplers[i].WrapU,
            modelInfo.ImageSamplers[i].WrapV
        );
        if (AXR_FAILED(assetCollection.createImageSampler(config))) return axr::Result::Error;
    }

    std::vector<std::string> imageNames(modelInfo.ImageCount, "");
    for (uint32_t i = 0; i < modelInfo.ImageCount; ++i) {
        if (!axrStringIsEmpty(modelInfo.Images[i].Name)) {
            imageNames[i] = modelInfo.Images[i].Name;
        } else {
            imageNames[i] = modelName + "_Image(" + std::to_string(i + 1) + ")";
        }

        axr::ImageConfig config(
            imageNames[i].c_str(),
            modelInfo.Images[i].FilePath,
            axr::ImageFormatEnum::Srgb
        );
        if (AXR_FAILED(assetCollection.createImage(config))) return axr::Result::Error;
    }

    std::vector<std::string> materialNames(modelInfo.MaterialCount, "");
    std::vector<std::string> alphaCutoffUniformBufferNames;
    for (uint32_t i = 0; i < modelInfo.MaterialCount; ++i) {
        if (!axrStringIsEmpty(modelInfo.Materials[i].Name)) {
            materialNames[i] = modelInfo.Materials[i].Name;
        } else {
            materialNames[i] = modelName + "_Material(" + std::to_string(i + 1) + ")";
        }

        const char* imageName = nullptr;
        const char* imageSamplerName = nullptr;

        if (modelInfo.Materials[i].ColorImageIndex >= 0) {
            imageName = imageNames[modelInfo.Materials[i].ColorImageIndex].c_str();
        }

        if (modelInfo.Materials[i].ColorImageSamplerIndex >= 0) {
            imageSamplerName = imageSamplerNames[modelInfo.Materials[i].ColorImageSamplerIndex].c_str();
        } else {
            imageSamplerName = axr::engineAssetGetName(axr::EngineAssetEnum::ImageSamplerLinearRepeat);
        }

        axr::MaterialAlphaRenderModeEnum alphaRenderMode;
        switch (modelInfo.Materials[i].AlphaMode) {
            case axr::ModelFileMaterialInfoAlphaModeEnum::Mask:
            case axr::ModelFileMaterialInfoAlphaModeEnum::Opaque: {
                alphaRenderMode = axr::MaterialAlphaRenderModeEnum::Opaque;
                break;
            }
            case axr::ModelFileMaterialInfoAlphaModeEnum::Blend: {
                alphaRenderMode = axr::MaterialAlphaRenderModeEnum::AlphaBlend;
                break;
            }
        }

        if (modelInfo.Materials[i].AlphaCutoff > 0.0f) {
            alphaCutoffUniformBufferNames.emplace_back(
                modelName + "_AlphaCutoff(" + std::to_string(alphaCutoffUniformBufferNames.size() + 1) + ")"
            );
            if (AXR_FAILED(
                    assetCollection.createUniformBuffer(
                        axr::UniformBufferConfig(
                            alphaCutoffUniformBufferNames.back().c_str(),
                            sizeof(float),
                            &modelInfo.Materials[i].AlphaCutoff
                        )
                    )
                )
            )
                return axr::Result::Error;
        }

        if (AXR_FAILED(
            assetCollection.createMaterial(
                materialNames[i].c_str(),
                axr::EngineAssetMaterial_DefaultMaterial(
                    modelInfo.Materials[i].BackfaceCullMode,
                    alphaRenderMode,
                    modelInfo.Materials[i].AlphaCutoff > 0.0f ?
                    alphaCutoffUniformBufferNames.back().c_str() :
                    "",
                    imageName,
                    imageSamplerName
                )
            )
        )) {
            return axr::Result::Error;
        }
    }

    m_Entity = m_Scene.createEntity();

    m_Entity.emplace<AxrTransformComponent>(
        AxrTransformComponent{
            .Position = glm::vec3(0.0f, 0.0f, 0.0f),
            .Scale = glm::vec3(1.0f, 1.0f, 1.0f),
            .Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        }
    );

    std::vector<AxrModelComponent::Mesh> meshes(modelInfo.MeshCount);
    for (uint32_t meshIndex = 0; meshIndex < modelInfo.MeshCount; ++meshIndex) {
        std::vector<AxrModelComponent::Submesh> submeshes(modelInfo.Meshes[meshIndex].SubmeshCount);

        for (int submeshIndex = 0; submeshIndex < modelInfo.Meshes[meshIndex].SubmeshCount; ++submeshIndex) {
            const char* materialName = nullptr;

            if (modelInfo.Meshes[meshIndex].Submeshes[submeshIndex].MaterialIndex >= 0) {
                materialName = materialNames[modelInfo.Meshes[meshIndex].Submeshes[submeshIndex].MaterialIndex].c_str();
            } else {
                materialName = defaultMaterialName;
            }

            AxrModelComponent::Submesh submesh{
                .MaterialName = {},
            };
            strcpy_s(submesh.MaterialName, materialName);

            submeshes[submeshIndex] = submesh;
        }

        meshes[meshIndex] = AxrModelComponent::Mesh{
            .Submeshes = submeshes
        };
    }

    AxrModelComponent modelComponent{
        .ModelName = {},
        .Meshes = meshes,
        .PushConstantBufferName = {},
    };
    strcpy_s(modelComponent.ModelName, modelName.c_str());
    strcpy_s(
        modelComponent.PushConstantBufferName,
        axr::engineAssetGetName(axr::EngineAssetEnum::PushConstantBufferMvpMatrix)
    );

    m_Entity.emplace<AxrModelComponent>(modelComponent);

    m_CameraEntity = m_Scene.createEntity();

    m_CameraEntity.emplace<AxrTransformComponent>(
        AxrTransformComponent{
            .Position = glm::vec3(0.0f, 1.0f, -0.5f),
            .Scale = glm::vec3(1.0f, 1.0f, 1.0f),
            .Orientation = glm::quat(glm::vec3(0.0f, glm::radians(45.0f), 0.0f)),
        }
    );

    m_CameraEntity.emplace<AxrCameraComponent>(
        AxrCameraComponent{
            .Fov = 90.0f,
            .ZNear = 0.01f,
            .ZFar = 1000.0f,
        }
    );

    m_Scene.setMainCamera(m_CameraEntity);

    // ---- XR Head ----

    const char* testCubeImageName = "UvTesterImage";
    if (AXR_FAILED(
        m_Scene.getAssetCollection().createImage(testCubeImageName, axr::EngineAssetEnum::ImageUvTester)
    )) {
        return axr::Result::Error;
    }

    const char* testCubeMaterialName = "HandMaterial";
    if (AXR_FAILED(
        assetCollection.createMaterial(
            testCubeMaterialName,
            axr::EngineAssetMaterial_DefaultMaterial(
                axr::MaterialBackfaceCullModeEnum::Back,
                axr::MaterialAlphaRenderModeEnum::Opaque,
                "",
                testCubeImageName,
                axr::engineAssetGetName(axr::EngineAssetEnum::ImageSamplerNearestRepeat)
            )
        )
    )) {
        return axr::Result::Error;
    }

    const char* testCubeModelName = "TestCube";
    if (AXR_FAILED(assetCollection.createModel(testCubeModelName, axr::EngineAssetEnum::ModelCube))) {
        return axr::Result::Error;
    }

    m_XrHeadEntity = m_Scene.createEntity();
    m_XrHeadEntity.emplace<AxrTransformComponent>(
        AxrTransformComponent{
            .Position = glm::vec3(0.0f, 0.0f, 0.0f),
            .Scale = glm::vec3(0.3f, 0.3f, 0.3f),
            .Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        }
    );
    m_XrHeadEntity.emplace<AxrMirrorPoseInputActionComponent>(
        AxrMirrorPoseInputActionComponent{
            .ActionSetName = "test",
            .PoseInputActionName = "head",
            .OffsetPosition = glm::vec3(0.0f, 0.0f, 0.0f),
            .OffsetOrientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        }
    );

    AxrModelComponent::Submesh testCubeSubmesh{
        .MaterialName = {},
    };
    strcpy_s(testCubeSubmesh.MaterialName, testCubeMaterialName);

    AxrModelComponent headModelComponent{
        .ModelName = {},
        .Meshes = std::vector{
            AxrModelComponent::Mesh{
                .Submeshes = std::vector{
                    testCubeSubmesh,
                },
            },
        },
        .PushConstantBufferName = {},
    };
    strcpy_s(headModelComponent.ModelName, testCubeModelName);
    strcpy_s(
        headModelComponent.PushConstantBufferName,
        axr::engineAssetGetName(axr::EngineAssetEnum::PushConstantBufferMvpMatrix)
    );

    m_XrHeadEntity.emplace<AxrModelComponent>(headModelComponent);

    // ---- XR Hand ----

    m_XrHandEntity = m_Scene.createEntity();
    m_XrHandEntity.emplace<AxrTransformComponent>(
        AxrTransformComponent{
            .Position = glm::vec3(0.0f, 0.0f, 0.0f),
            .Scale = glm::vec3(0.1f, 0.1f, 0.1f),
            .Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        }
    );
    m_XrHandEntity.emplace<AxrMirrorPoseInputActionComponent>(
        AxrMirrorPoseInputActionComponent{
            .ActionSetName = "test",
            .PoseInputActionName = "righthand",
            .OffsetPosition = glm::vec3(0.0f, 0.2f, 0.0f),
            .OffsetOrientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        }
    );

    AxrModelComponent handModelComponent{
        .ModelName = {},
        .Meshes = std::vector{
            AxrModelComponent::Mesh{
                .Submeshes = std::vector{
                    testCubeSubmesh,
                }
            },
        },
        .PushConstantBufferName = {},
    };
    strcpy_s(handModelComponent.ModelName, testCubeModelName);
    strcpy_s(
        handModelComponent.PushConstantBufferName,
        axr::engineAssetGetName(axr::EngineAssetEnum::PushConstantBufferMvpMatrix)
    );

    m_XrHandEntity.emplace<AxrModelComponent>(handModelComponent);

    // ---- UI ----

    m_UIImageData = axr::UIImageData(
        testCubeImageName,
        axr::engineAssetGetName(axr::EngineAssetEnum::ImageSamplerLinearRepeat)
    );

    m_Scene.setUIImageResourcesPreloadCount(16);
    m_Scene.setBuildUICanvasCallback(
        this,
        [](
        void* userData,
        const axr::PlatformType platformType,
        Clay_Context* context
    ) -> axr::UICanvasConfig {
            const auto scene = static_cast<SponzaScene*>(userData);
            return scene->uiCallback(platformType, context);
        }
    );

    if (AXR_FAILED(
        m_Application.getGlobalAssetCollection().getFontID(
            axr::engineAssetGetName(axr::EngineAssetEnum::FontJetbrainsMono_Regular),
            &m_FontID
        )
    )) {
        return axr::Result::Error;
    }

    m_FpsString = "FPS: 0";
    m_DeltaTimeString = "DeltaTime: 0";
    m_UIActionSet = m_Application.getActionSystem().getActionSet("ui");

    return axr::Result::Success;
}

axr::Result SponzaScene::loadScene() const {
    return m_Application.loadScene(m_SceneName.c_str());
}

axr::Result SponzaScene::setAsActiveScene() const {
    return m_Application.setActiveScene(m_SceneName.c_str());
}

void SponzaScene::update() {
    const float deltaTime = m_Application.getDeltaTime();
    m_AccumulatedDeltaTime += deltaTime;
    m_AccumulatedDeltaTimeCount++;

    if (m_AccumulatedDeltaTime >= 0.5f) {
        const float averageDeltaTime = m_AccumulatedDeltaTime / static_cast<float>(m_AccumulatedDeltaTimeCount);
        const auto fps = static_cast<uint32_t>(std::round(1.0f / averageDeltaTime));

        m_FpsString = "FPS: " + std::to_string(fps);
        m_DeltaTimeString = "DeltaTime: " + std::to_string(averageDeltaTime);

        m_AccumulatedDeltaTime = 0.0f;
        m_AccumulatedDeltaTimeCount = 0;
    }
}

axr::UICanvasConfig SponzaScene::uiCallback(const axr::PlatformType platformType, Clay_Context* context) {
    // Only handle UI for the window
    if (platformType != axr::PlatformType::Window) return {};
    Clay_SetCurrentContext(context);

    const axr::Vec2 cursorPos = m_UIActionSet.getVec2InputAction("cursor_position").getValue();
    const bool click = m_UIActionSet.getBoolInputAction("click").getValue();
    const float scroll = m_UIActionSet.getFloatInputAction("scroll").getValue();

    Clay_SetPointerState(Clay_Vector2{.x = cursorPos.x, .y = cursorPos.y}, click);
    Clay_UpdateScrollContainers(
        true,
        Clay_Vector2{.x = 0.0f, .y = scroll},
        m_Application.getDeltaTime()
    );

    Clay_SetMeasureTextFunction(axrApplicationHandleClayMeasureText, m_Application.toRaw());

    Clay_BeginLayout();

    // TODO: Maybe make axr versions of all these functions. that way we won't need to set the context and the measure text function here
    { // ---- Outer Container ----
        Clay__OpenElement();
        Clay__ConfigureOpenElement(
            Clay_ElementDeclaration{
                .id = CLAY_ID("OuterContainer"),
                .layout = Clay_LayoutConfig{
                    .sizing = Clay_Sizing{
                        .width = CLAY_SIZING_GROW(0),
                        .height = CLAY_SIZING_GROW(0)
                    },
                    .padding = CLAY_PADDING_ALL(16),
                    .childGap = 16,
                },
            }
        );
        { // ---- Sidebar ----
            Clay__OpenElement();
            Clay__ConfigureOpenElement(
                Clay_ElementDeclaration{
                    .id = CLAY_ID("SideBar"),
                    .layout = Clay_LayoutConfig{
                        .sizing = Clay_Sizing{
                            .width = CLAY_SIZING_FIXED(300),
                            .height = CLAY_SIZING_GROW(0)
                        },
                        .padding = CLAY_PADDING_ALL(16),
                        .childGap = 16,
                        .childAlignment = Clay_ChildAlignment{
                            .x = CLAY_ALIGN_X_LEFT,
                            .y = CLAY_ALIGN_Y_TOP,
                        },
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    },
                    .backgroundColor = Clay_Color{
                        0.0f,
                        0.0f,
                        0.0f,
                        0.8f
                    },
                    .cornerRadius = Clay_CornerRadius{
                        .topLeft = 25,
                        .topRight = 25,
                        .bottomLeft = 25,
                        .bottomRight = 25,
                    },
                    .clip = Clay_ClipElementConfig{
                        .vertical = true,
                        .childOffset = Clay_GetScrollOffset(),
                    },
                    .border = Clay_BorderElementConfig{
                        .color = Clay_Color{
                            .r = 0.01,
                            .g = 0.01,
                            .b = 0.01,
                            .a = 1.0f
                        },
                        .width = Clay_BorderWidth{
                            .left = 2,
                            .right = 2,
                            .top = 2,
                            .bottom = 2,
                            .betweenChildren = 0,
                        }
                    },
                }
            );
            { // ---- Fps Outer ----
                std::string outer_id = std::string("FpsOuter");
                Clay__OpenElement();
                Clay__ConfigureOpenElement(
                    Clay_ElementDeclaration{
                        .id = CLAY_SID(Clay_String(false, outer_id.size(), outer_id.c_str())),
                        .layout = {
                            .sizing = {.width = CLAY_SIZING_GROW(0)},
                            .padding = Clay_Padding{
                                5, 5, 5, 5
                            },
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        },
                        .backgroundColor = Clay_Color{
                            0.01,
                            0.01,
                            0.01,
                            1.0
                        },
                        .cornerRadius = Clay_CornerRadius{
                            .topLeft = 10,
                            .topRight = 10,
                            .bottomLeft = 10,
                            .bottomRight = 10,
                        },
                    }
                );
                { // ---- FPS ----
                    Clay_TextElementConfig* textElementConfig = Clay__StoreTextElementConfig(
                        Clay_TextElementConfig{
                            .textColor = {
                                1.0f,
                                1.0f,
                                1.0f,
                                1.0f
                            },
                            .fontId = m_FontID,
                            .fontSize = 20,
                        }
                    );
                    Clay__OpenTextElement(
                        Clay_String(false, m_FpsString.length(), m_FpsString.c_str()),
                        textElementConfig
                    );
                    Clay__OpenTextElement(
                        Clay_String(false, m_DeltaTimeString.length(), m_DeltaTimeString.c_str()),
                        textElementConfig
                    );
                }
                Clay__CloseElement();
            }
            for (int i = 0; i < 20; ++i) {
                { // ---- Profile Picture Outer ----
                    std::string outer_id = std::string("ProfilePictureOuter") + std::to_string(i);
                    Clay__OpenElement();
                    Clay__ConfigureOpenElement(
                        Clay_ElementDeclaration{
                            .id = CLAY_SID(Clay_String(false, outer_id.size(), outer_id.c_str())),
                            .layout = {
                                .sizing = {.width = CLAY_SIZING_GROW(0)},
                                .padding = Clay_Padding{
                                    5, 5, 5, 5
                                },
                                .childGap = 16,
                            },
                            .backgroundColor = Clay_Color{
                                0.1,
                                0.0,
                                0.2,
                                1.0
                            },
                            .cornerRadius = Clay_CornerRadius{
                                .topLeft = 10,
                                .topRight = 10,
                                .bottomLeft = 10,
                                .bottomRight = 10,
                            },
                        }
                    );
                    { // ---- Profile Picture ----
                        std::string inner_id = std::string("ProfilePicture") + std::to_string(i);

                        Clay__OpenElement();
                        Clay__ConfigureOpenElement(
                            Clay_ElementDeclaration{
                                .id = CLAY_SID(Clay_String(false, inner_id.size(), inner_id.c_str())),
                                .layout = {
                                    .sizing = {
                                        .width = CLAY_SIZING_FIXED(60),
                                        .height = CLAY_SIZING_FIXED(60)
                                    }
                                },
                                .backgroundColor = Clay_Color{
                                    1.0f,
                                    1.0f,
                                    1.0f,
                                    1.0f
                                },
                                .cornerRadius = Clay_CornerRadius{
                                    .topLeft = 10,
                                    .topRight = 10,
                                    .bottomLeft = 10,
                                    .bottomRight = 10,
                                },
                                .image = Clay_ImageElementConfig{
                                    .imageData = &m_UIImageData,
                                },
                            }
                        );
                        Clay__CloseElement();
                    }
                    { // ---- Profile Text ----
                        Clay_TextElementConfig* textElementConfig = Clay__StoreTextElementConfig(
                            Clay_TextElementConfig{
                                .textColor = {
                                    1.0f,
                                    1.0f,
                                    1.0f,
                                    1.0f
                                },
                                .fontId = m_FontID,
                                .fontSize = 14,
                            }
                        );
                        Clay__OpenTextElement(
                            CLAY_STRING("Sphinx of black quartz, judge my vow!"),
                            textElementConfig
                        );
                    }
                    Clay__CloseElement();
                }
            }
            Clay__CloseElement();
        }
        Clay__CloseElement();
    }

    const Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    return axr::UICanvasConfig(true, renderCommands);
}
