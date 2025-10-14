#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/scene.h"

/// Get the matrix for the given transform component
/// @param transformComponent Transform component
/// @returns The transform component matrix
[[nodiscard]] glm::mat4 axrTransformGetMatrix(const AxrTransformComponent& transformComponent);

/// Convert the given clay error type to a string
/// @param errorType Clay error type
/// @returns String equivalent
// TODO: Move this out of here. it's not part of the scene
[[nodiscard]] const char* axrToString(Clay_ErrorType errorType);
