#include "alphaTestScene.hpp"

AlphaTestScene::AlphaTestScene(axr::Application& app):
    m_Application(app),
    m_Scene(nullptr) {
}

AlphaTestScene::~AlphaTestScene() {
}

axr::Result AlphaTestScene::setup() {
    if (AXR_FAILED(m_Application.createScene(m_SceneName.c_str()))) return axr::Result::Error;
    m_Scene = m_Application.findScene(m_SceneName.c_str());
    axr::AssetCollection assetCollection = m_Scene.getAssetCollection();

    const char* defaultMaterialName = "AlphaTestDefaultMaterial";
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

    const std::string modelName = "AlphaTestModel";
    const axr::ModelConfig modelConfig(
        modelName.c_str(),
        "sample-models/2.0/AlphaBlendModeTest/glTF/AlphaBlendModeTest.gltf"
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
            modelInfo.Images[i].FilePath
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
            .Position = glm::vec3(0.0f, 0.0f, -5.5f),
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
        axr::engineAssetGetName(axr::EngineAssetEnum::PushConstantBufferModelMatrix)
    );

    m_Entity.emplace<AxrModelComponent>(modelComponent);

    m_CameraEntity = m_Scene.createEntity();

    m_CameraEntity.emplace<AxrTransformComponent>(
        AxrTransformComponent{
            .Position = glm::vec3(0.0f, 1.0f, 0.0f),
            .Scale = glm::vec3(1.0f, 1.0f, 1.0f),
            .Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        }
    );

    m_CameraEntity.emplace<AxrCameraComponent>(
        AxrCameraComponent{
            .Fov = 90.0f,
            .ZNear = 0.1f,
            .ZFar = 1000.0f,
        }
    );

    m_Scene.setMainCamera(m_CameraEntity);

    // ---- Layered Transparency ----

    std::string glassModelName = "GlassPlaneModel";
    if (AXR_FAILED(assetCollection.createModel(glassModelName.c_str(), axr::EngineAssetEnum::ModelSquare)))
        return axr::Result::Error;

    std::string cyanGlassImageName = "CyanGlassImage";
    if (AXR_FAILED(assetCollection.createImage(axr::ImageConfig(cyanGlassImageName.c_str(), "cyan-glass.png"))))
        return axr::Result::Error;

    std::string magentaGlassImageName = "MagentaGlassImage";
    if (AXR_FAILED(assetCollection.createImage(axr::ImageConfig(magentaGlassImageName.c_str(), "magenta-glass.png"))))
        return axr::Result::Error;

    std::string yellowGlassImageName = "YellowGlassImage";
    if (AXR_FAILED(assetCollection.createImage(axr::ImageConfig(yellowGlassImageName.c_str(), "yellow-glass.png"))))
        return axr::Result::Error;

    std::string cyanGlassMaterialName = "CyanGlassMaterial";
    if (AXR_FAILED(
        assetCollection.createMaterial(cyanGlassMaterialName.c_str(),
            axr::EngineAssetMaterial_DefaultMaterial(
                axr::MaterialBackfaceCullModeEnum::None,
                axr::MaterialAlphaRenderModeEnum::AlphaBlend,
                "",
                cyanGlassImageName.c_str(),
                axr::engineAssetGetName(axr::EngineAssetEnum::ImageSamplerNearestRepeat)
            ))
    ))
        return axr::Result::Error;

    std::string magentaGlassMaterialName = "MagentaGlassMaterial";
    if (AXR_FAILED(
        assetCollection.createMaterial(magentaGlassMaterialName.c_str(),
            axr::EngineAssetMaterial_DefaultMaterial(
                axr::MaterialBackfaceCullModeEnum::None,
                axr::MaterialAlphaRenderModeEnum::AlphaBlend,
                "",
                magentaGlassImageName.c_str(),
                axr::engineAssetGetName(axr::EngineAssetEnum::ImageSamplerNearestRepeat)
            ))
    ))
        return axr::Result::Error;

    std::string yellowGlassMaterialName = "YellowGlassMaterial";
    if (AXR_FAILED(
        assetCollection.createMaterial(yellowGlassMaterialName.c_str(),
            axr::EngineAssetMaterial_DefaultMaterial(
                axr::MaterialBackfaceCullModeEnum::None,
                axr::MaterialAlphaRenderModeEnum::AlphaBlend,
                "",
                yellowGlassImageName.c_str(),
                axr::engineAssetGetName(axr::EngineAssetEnum::ImageSamplerNearestRepeat)
            ))
    ))
        return axr::Result::Error;

    for (int i = 0; i < 4; ++i) {
        axr::Entity_T cyanGlassEntity = m_Scene.createEntity();
        axr::Entity_T magentaGlassEntity = m_Scene.createEntity();
        axr::Entity_T yellowGlassEntity = m_Scene.createEntity();

        glm::vec3 pos;
        if (i == 0) {
            pos = glm::vec3(0.0f, 2.3f, -5.5f);
        } else if (i == 1) {
            pos = glm::vec3(5.5f, 2.3f, 0.0f);
        } else if (i == 2) {
            pos = glm::vec3(0.0f, 2.3f, 5.5f);
        } else if (i == 3) {
            pos = glm::vec3(-5.5f, 2.3f, 0.0f);
        }

        cyanGlassEntity.emplace<AxrTransformComponent>(
            AxrTransformComponent{
                .Position = pos,
                .Scale = glm::vec3(1.0f, 1.0f, 1.0f),
                .Orientation = glm::rotate(
                    glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                    glm::radians(90.0f * i),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                ),
            }
        );

        magentaGlassEntity.emplace<AxrTransformComponent>(
            AxrTransformComponent{
                .Position = pos + glm::vec3(-0.2f, 0.1f, -0.1f),
                .Scale = glm::vec3(1.0f, 1.0f, 1.0f),
                .Orientation = glm::rotate(
                    glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                    glm::radians(90.0f * i),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                ),
            }
        );

        yellowGlassEntity.emplace<AxrTransformComponent>(
            AxrTransformComponent{
                .Position = pos + glm::vec3(-0.4f, 0.2f, -0.2f),
                .Scale = glm::vec3(1.0f, 1.0f, 1.0f),
                .Orientation = glm::rotate(
                    glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                    glm::radians(90.0f * i),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                ),
            }
        );

        AxrModelComponent::Submesh cyanGlassSubmesh{
            .MaterialName = {},
        };
        strcpy_s(cyanGlassSubmesh.MaterialName, cyanGlassMaterialName.c_str());

        AxrModelComponent::Mesh cyanGlassMesh{
            .Submeshes = std::vector{
                cyanGlassSubmesh,
            }
        };

        AxrModelComponent cyanGlassModelComponent{
            .ModelName = {},
            .Meshes = std::vector{
                cyanGlassMesh,
            },
            .PushConstantBufferName = {},
        };
        strcpy_s(cyanGlassModelComponent.ModelName, glassModelName.c_str());
        strcpy_s(
            cyanGlassModelComponent.PushConstantBufferName,
            axr::engineAssetGetName(axr::EngineAssetEnum::PushConstantBufferModelMatrix)
        );

        cyanGlassEntity.emplace<AxrModelComponent>(cyanGlassModelComponent);

        AxrModelComponent::Submesh magentaGlassSubmesh{
            .MaterialName = {},
        };
        strcpy_s(magentaGlassSubmesh.MaterialName, magentaGlassMaterialName.c_str());

        AxrModelComponent::Mesh magentaGlassMesh{
            .Submeshes = std::vector{
                magentaGlassSubmesh,
            }
        };

        AxrModelComponent magentaGlassModelComponent{
            .ModelName = {},
            .Meshes = std::vector{
                magentaGlassMesh,
            },
            .PushConstantBufferName = {},
        };
        strcpy_s(magentaGlassModelComponent.ModelName, glassModelName.c_str());
        strcpy_s(
            magentaGlassModelComponent.PushConstantBufferName,
            axr::engineAssetGetName(axr::EngineAssetEnum::PushConstantBufferModelMatrix)
        );

        magentaGlassEntity.emplace<AxrModelComponent>(magentaGlassModelComponent);

        AxrModelComponent::Submesh yellowGlassSubmesh{
            .MaterialName = {},
        };
        strcpy_s(yellowGlassSubmesh.MaterialName, yellowGlassMaterialName.c_str());

        AxrModelComponent::Mesh yellowGlassMesh{
            .Submeshes = std::vector{
                yellowGlassSubmesh,
            }
        };

        AxrModelComponent yellowGlassModelComponent{
            .ModelName = {},
            .Meshes = std::vector{
                yellowGlassMesh,
            },
            .PushConstantBufferName = {},
        };
        strcpy_s(yellowGlassModelComponent.ModelName, glassModelName.c_str());
        strcpy_s(
            yellowGlassModelComponent.PushConstantBufferName,
            axr::engineAssetGetName(axr::EngineAssetEnum::PushConstantBufferModelMatrix)
        );

        yellowGlassEntity.emplace<AxrModelComponent>(yellowGlassModelComponent);
    }

    return axr::Result::Success;
}

axr::Result AlphaTestScene::loadScene() const {
    return m_Application.loadScene(m_SceneName.c_str());
}

axr::Result AlphaTestScene::setAsActiveScene() const {
    return m_Application.setActiveScene(m_SceneName.c_str());
}

void AlphaTestScene::update() {
    const float deltaTime = m_Application.getDeltaTime();
    const axr::ActionSet movement = m_Application.getActionSystem().getActionSet("movement");
    const axr::BoolInputAction forward = movement.getBoolInputAction("forward");
    const axr::BoolInputAction left = movement.getBoolInputAction("left");
    const axr::BoolInputAction back = movement.getBoolInputAction("back");
    const axr::BoolInputAction right = movement.getBoolInputAction("right");
    const axr::Vec2InputAction mouse = movement.getVec2InputAction("mouse");

    m_CameraEntity.patch<AxrTransformComponent>(
        [deltaTime, forward, left, back, right, mouse](AxrTransformComponent& transform) {
            if (mouse.valueChanged()) {
                transform.Orientation = glm::rotate(
                    transform.Orientation,
                    5.0f * deltaTime,
                    glm::vec3(
                        0.0f,
                        mouse.getValue().x > 0.0f ? -1.0f : 1.0f,
                        0.0f
                    )
                );
            }

            if (forward.getValue()) {
                transform.Position += glm::vec3(0.0f, 0.0f, deltaTime * -2.0f);
            }
            if (left.getValue()) {
                transform.Position += glm::vec3(deltaTime * -2.0f, 0.0f, 0.0f);
            }
            if (back.getValue()) {
                transform.Position += glm::vec3(0.0f, 0.0f, deltaTime * 2.0f);
            }
            if (right.getValue()) {
                transform.Position += glm::vec3(deltaTime * 2.0f, 0.0f, 0.0f);
            }
        }
    );
}
