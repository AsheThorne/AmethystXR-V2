#pragma once

// ----------------------------------------- //
// AXR Macros
// ----------------------------------------- //
#ifdef AXR_BUILD_DLL
#define AXR_API __declspec(dllexport)
#else
#define AXR_API __declspec(dllimport)
#endif

#define AXR_SUCCEEDED(result) ((result) >= 0)
#define AXR_FAILED(result) ((result) < 0)

#define AXR_MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

#define AXR_VERSION_MAJOR(version) (((uint32_t)(version) >> 22U) & 0x7FU)
#define AXR_VERSION_MINOR(version) (((uint32_t)(version) >> 12U) & 0x3FFU)
#define AXR_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFFU)

// ----------------------------------------- //
// AXR Engine Data
// ----------------------------------------- //
#define AXR_ENGINE_VERSION AXR_MAKE_VERSION(1, 0, 0)
#define AXR_HAPTIC_ACTION_NO_DURATION XR_NO_DURATION
#define AXR_HAPTIC_ACTION_MIN_DURATION XR_MIN_HAPTIC_DURATION
#define AXR_HAPTIC_ACTION_INFINITE_DURATION XR_INFINITE_DURATION
#define AXR_HAPTIC_ACTION_UNSPECIFIED_FREQUENCY XR_FREQUENCY_UNSPECIFIED

// ---- Max String Sizes ----

#define AXR_MAX_APPLICATION_NAME_SIZE 128
#define AXR_MAX_ENGINE_NAME_SIZE 128
#define AXR_MAX_SCENE_NAME_SIZE 128
#define AXR_MAX_ASSET_NAME_SIZE 128
#define AXR_MAX_FILE_PATH_SIZE 256
#define AXR_MAX_ACTION_NAME_SIZE 64
#define AXR_MAX_ACTION_LOCALIZED_NAME_SIZE 128
#define AXR_MAX_ACTION_SET_NAME_SIZE 64
#define AXR_MAX_ACTION_SET_LOCALIZED_NAME_SIZE 128
#define AXR_MAX_FONT_ATLAS_CHARACTER_SIZE 1028
