# ---------------------------------------------
# Create AXR_Engine Project
# ---------------------------------------------
project(AXR_Engine VERSION 1.0.0)

set(AXR_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(AXR_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(AXR_ASSETS_DIR ${CMAKE_CURRENT_LIST_DIR}/assets)
set(AXR_SHADERS_DIR ${AXR_ASSETS_DIR}/shaders)

# ---------------------------------------------
# Generate defines.h from defines.h.in
# ---------------------------------------------
configure_file(
    ${AXR_INCLUDE_DIR}/axr/common/defines.h.in
    ${AXR_INCLUDE_DIR}/axr/common/defines.h
)

# ---------------------------------------------
# Set Header Files
# ---------------------------------------------

set(AXR_PUBLIC_HEADER_FILES
    # ---- Root ----
    ${AXR_INCLUDE_DIR}/axr.h
    # ---- AXR ----
    ${AXR_INCLUDE_DIR}/axr/common.h
    ${AXR_INCLUDE_DIR}/axr/application.h
    ${AXR_INCLUDE_DIR}/axr/logger.h
    ${AXR_INCLUDE_DIR}/axr/setup.h
    ${AXR_INCLUDE_DIR}/axr/windowSystem.h
    ${AXR_INCLUDE_DIR}/axr/graphicsSystem.h
    ${AXR_INCLUDE_DIR}/axr/vulkanApi.h
    ${AXR_INCLUDE_DIR}/axr/assets.h
    ${AXR_INCLUDE_DIR}/axr/scene.h
    ${AXR_INCLUDE_DIR}/axr/xrSystem.h
    ${AXR_INCLUDE_DIR}/axr/actionSystem.h
    # ---- Common ----
    ${AXR_INCLUDE_DIR}/axr/common/defines.h.in
    ${AXR_INCLUDE_DIR}/axr/common/defines.h
    ${AXR_INCLUDE_DIR}/axr/common/enums.h
    ${AXR_INCLUDE_DIR}/axr/common/eventHandler.h
    ${AXR_INCLUDE_DIR}/axr/common/callback.h
    ${AXR_INCLUDE_DIR}/axr/common/utils.h
    ${AXR_INCLUDE_DIR}/axr/common/types.h
)

set(AXR_PUBLIC_HEADER_HPP_FILES
    # ---- Root ----
    ${AXR_INCLUDE_DIR}/axr.hpp
    # ---- AXR HPP ----
    ${AXR_INCLUDE_DIR}/axr-hpp/common.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/application.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/logger.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/setup.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/windowSystem.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/graphicsSystem.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/vulkanApi.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/flags.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/assets.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/scene.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/xrSystem.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/actionSystem.hpp
    # ---- Common ----
    ${AXR_INCLUDE_DIR}/axr-hpp/common/defines.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/common/enums.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/common/callback.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/common/eventHandler.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/common/utils.hpp
    ${AXR_INCLUDE_DIR}/axr-hpp/common/types.hpp
)

set(AXR_HEADER_FILES
    # ---- Root ----
    ${AXR_SRC_DIR}/common.hpp
    ${AXR_SRC_DIR}/utils.hpp
    ${AXR_SRC_DIR}/extensionCollection.hpp
    # ---- AXR Application ----
    ${AXR_SRC_DIR}/application/application.hpp
    # ---- Window System ----
    ${AXR_SRC_DIR}/windowSystem/windowSystem.hpp
    # ---- Window System - Win32 ----
    ${AXR_SRC_DIR}/windowSystem/win32/win32WindowSystem.hpp
    # ---- Graphics System ----
    ${AXR_SRC_DIR}/graphicsSystem/graphicsSystem.hpp
    # ---- Graphics System - Vulkan ----
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanGraphicsSystem.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanUtils.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanSharedFunctions.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanQueueFamilies.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanWindowGraphics.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanXrGraphics.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanSurfaceDetails.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanBuffer.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanRenderCommands.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanImage.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanRenderStructs.hpp
    # ---- Graphics System - Vulkan - Scene Data ----
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanSceneData.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanModelData.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanMaterialLayoutData.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanMaterialData.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanUniformBufferData.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanLoadedScenesCollection.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanImageData.hpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanImageSamplerData.hpp
    # ---- Assets ----
    ${AXR_SRC_DIR}/assets/assetCollection.hpp
    ${AXR_SRC_DIR}/assets/shader.hpp
    ${AXR_SRC_DIR}/assets/material.hpp
    ${AXR_SRC_DIR}/assets/model.hpp
    ${AXR_SRC_DIR}/assets/mesh.hpp
    ${AXR_SRC_DIR}/assets/submesh.hpp
    ${AXR_SRC_DIR}/assets/engineAssets.hpp
    ${AXR_SRC_DIR}/assets/shaderProperties.hpp
    ${AXR_SRC_DIR}/assets/shaderValues.hpp
    ${AXR_SRC_DIR}/assets/assetsUtils.hpp
    ${AXR_SRC_DIR}/assets/uniformBuffer.hpp
    ${AXR_SRC_DIR}/assets/pushConstantBuffer.hpp
    ${AXR_SRC_DIR}/assets/image.hpp
    ${AXR_SRC_DIR}/assets/imageSampler.hpp
    # ---- Scene ----
    ${AXR_SRC_DIR}/scene/scene.hpp
    ${AXR_SRC_DIR}/scene/sceneUtils.hpp
    # ---- Xr System ----
    ${AXR_SRC_DIR}/xrSystem/xrSystem.hpp
    ${AXR_SRC_DIR}/xrSystem/xrUtils.hpp
    ${AXR_SRC_DIR}/xrSystem/xrExtensionFunctions.hpp
    # ---- Action System ----
    ${AXR_SRC_DIR}/actionSystem/actionSystem.hpp
    ${AXR_SRC_DIR}/actionSystem/boolInputAction.hpp
    ${AXR_SRC_DIR}/actionSystem/floatInputAction.hpp
    ${AXR_SRC_DIR}/actionSystem/vec2InputAction.hpp
    ${AXR_SRC_DIR}/actionSystem/poseInputAction.hpp
    ${AXR_SRC_DIR}/actionSystem/hapticOutputAction.hpp
    ${AXR_SRC_DIR}/actionSystem/actionSet.hpp
    ${AXR_SRC_DIR}/actionSystem/actionUtils.hpp
)

# ---------------------------------------------
# Set CPP Files
# ---------------------------------------------

set(AXR_CPP_FILES
    # ---- Root ----
    ${AXR_SRC_DIR}/setup.cpp
    ${AXR_SRC_DIR}/utils.cpp
    ${AXR_SRC_DIR}/libraryImplementations.cpp
    # ---- AXR Application ----
    ${AXR_SRC_DIR}/application/application.cpp
    # ---- Window System ----
    ${AXR_SRC_DIR}/windowSystem/windowSystem.cpp
    # ---- Window System - Win32 ----
    ${AXR_SRC_DIR}/windowSystem/win32/win32WindowSystem.cpp
    # ---- Graphics System ----
    ${AXR_SRC_DIR}/graphicsSystem/graphicsSystem.cpp
    ${AXR_SRC_DIR}/graphicsSystem/graphicsCommonStructs.hpp
    # ---- Graphics System - Vulkan ----
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanGraphicsSystem.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanUtils.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanSharedFunctions.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanQueueFamilies.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanWindowGraphics.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanXrGraphics.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanSurfaceDetails.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanBuffer.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/vulkanImage.cpp
    # ---- Graphics System - Vulkan - Scene Data ----
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanSceneData.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanModelData.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanMaterialLayoutData.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanMaterialData.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanUniformBufferData.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanLoadedScenesCollection.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanImageData.cpp
    ${AXR_SRC_DIR}/graphicsSystem/vulkan/sceneData/vulkanImageSamplerData.cpp
    # ---- Assets ----
    ${AXR_SRC_DIR}/assets/assetCollection.cpp
    ${AXR_SRC_DIR}/assets/shader.cpp
    ${AXR_SRC_DIR}/assets/material.cpp
    ${AXR_SRC_DIR}/assets/model.cpp
    ${AXR_SRC_DIR}/assets/mesh.cpp
    ${AXR_SRC_DIR}/assets/submesh.cpp
    ${AXR_SRC_DIR}/assets/engineAssets.cpp
    ${AXR_SRC_DIR}/assets/shaderProperties.cpp
    ${AXR_SRC_DIR}/assets/shaderValues.cpp
    ${AXR_SRC_DIR}/assets/assetsUtils.cpp
    ${AXR_SRC_DIR}/assets/uniformBuffer.cpp
    ${AXR_SRC_DIR}/assets/pushConstantBuffer.cpp
    ${AXR_SRC_DIR}/assets/image.cpp
    ${AXR_SRC_DIR}/assets/imageSampler.cpp
    # ---- Scene ----
    ${AXR_SRC_DIR}/scene/scene.cpp
    ${AXR_SRC_DIR}/scene/sceneUtils.cpp
    # ---- Xr System ----
    ${AXR_SRC_DIR}/xrSystem/xrSystem.cpp
    ${AXR_SRC_DIR}/xrSystem/xrUtils.cpp
    ${AXR_SRC_DIR}/xrSystem/xrExtensionFunctions.cpp
    # ---- Action System ----
    ${AXR_SRC_DIR}/actionSystem/actionSystem.cpp
    ${AXR_SRC_DIR}/actionSystem/boolInputAction.cpp
    ${AXR_SRC_DIR}/actionSystem/floatInputAction.cpp
    ${AXR_SRC_DIR}/actionSystem/vec2InputAction.cpp
    ${AXR_SRC_DIR}/actionSystem/poseInputAction.cpp
    ${AXR_SRC_DIR}/actionSystem/hapticOutputAction.cpp
    ${AXR_SRC_DIR}/actionSystem/actionSet.cpp
    ${AXR_SRC_DIR}/actionSystem/actionUtils.cpp
)

# ---------------------------------------------
# Set Asset Files
# ---------------------------------------------

set(AXR_ASSET_FILES
    # ---- Shaders ----
    ${AXR_SHADERS_DIR}/shader.vert
    ${AXR_SHADERS_DIR}/shader.frag
    # ---- Shaders UI ----
    ${AXR_SHADERS_DIR}/ui/element.vert
    ${AXR_SHADERS_DIR}/ui/rectangle.frag
    ${AXR_SHADERS_DIR}/ui/border.frag
    ${AXR_SHADERS_DIR}/ui/image.frag
)

# ---------------------------------------------
# Create AXR_Engine library
# ---------------------------------------------

# ---- Combine cpp, header and asset files into one list called AXR_FILES ----
list(APPEND AXR_SOURCE_FILES ${AXR_CPP_FILES} ${AXR_HEADER_FILES} ${AXR_PUBLIC_HEADER_FILES} ${AXR_PUBLIC_HEADER_HPP_FILES})
list(APPEND AXR_FILES ${AXR_SOURCE_FILES} ${AXR_ASSET_FILES})

add_library(AXR_Engine SHARED ${AXR_FILES})

set_target_properties(AXR_Engine PROPERTIES LINKER_LANGUAGE CXX)

# ---------------------------------------------
# Link libraries to AXR_Engine
# ---------------------------------------------
target_link_libraries(AXR_Engine
    PRIVATE axr_compiler_flags
    Vulkan::Vulkan
    optimized OpenXR::openxr_loader
    debug OpenXR::openxr_loader_debug
)

# ---------------------------------------------
# AXR_Engine Include Directories
# ---------------------------------------------
target_include_directories(AXR_Engine
    PUBLIC ${AXR_INCLUDE_DIR}
    ${SPDLOG_DIRECTORY}/include
    ${EnTT_DIRECTORY}/single_include
    ${STB_DIRECTORY}/include
    ${TINY_GLTF_DIRECTORY}/include
    ${JSON_DIRECTORY}/include
    ${CLAY_DIRECTORY}/include
)

if (VULKAN_HPP_DIRECTORY)
    target_include_directories(AXR_Engine
        PUBLIC ${VULKAN_HPP_DIRECTORY}
    )
endif ()

# ---------------------------------------------
# Set AXR_Engine compile definitions
# ---------------------------------------------
add_compile_definitions(UNICODE)
add_compile_definitions(AXR_BUILD_DLL)

if (WIN32)
    add_compile_definitions(AXR_USE_PLATFORM_WIN32)
    add_compile_definitions(NOMINMAX)
endif ()

if (Vulkan_FOUND)
    add_compile_definitions(AXR_SUPPORTED_GRAPHICS_VULKAN)
    # This definition is for OpenXR
    add_compile_definitions(XR_USE_GRAPHICS_API_VULKAN)
    add_compile_definitions(VULKAN_HPP_NO_EXCEPTIONS)
    add_compile_definitions(VULKAN_HPP_NO_DEFAULT_DISPATCHER)
    if (WIN32)
        add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
    endif ()
endif ()

# ---------------------------------------------
# Generate vulkan shaders
# ---------------------------------------------

if (Vulkan_FOUND)
    add_custom_command(TARGET AXR_Engine POST_BUILD COMMAND
        ${GLSLC} ${AXR_SHADERS_DIR}/shader.frag -DALPHA_MODE_MASK -o ${AXR_SHADERS_DIR}/shader_mask.frag.spv
    )
endif ()

generate_vulkan_shaders(${AXR_SHADERS_DIR} AXR_Engine)

# ---------------------------------------------
# Copy Axr engine assets to output folder
# ---------------------------------------------

set(AXR_OUTPUT_ASSETS_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/axr-assets)

# Delete old axr assets folder
add_custom_command(TARGET AXR_Engine POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E remove_directory ${AXR_OUTPUT_ASSETS_DIR}
)

# Copy axr assets folder to output directory
add_custom_command(TARGET AXR_Engine POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${AXR_ASSETS_DIR} ${AXR_OUTPUT_ASSETS_DIR}
)