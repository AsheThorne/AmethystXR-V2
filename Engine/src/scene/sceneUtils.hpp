#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/scene.h"

/// Get the matrix for the given transform component
/// @param transformComponent Transform component
/// @returns The transform component matrix
[[nodiscard]] glm::mat4 axrTransformGetMatrix(const AxrTransformComponent& transformComponent);
