// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "sceneUtils.hpp"

// ----------------------------------------- //
// External Functions
// ----------------------------------------- //

AxrTransformComponent axrTransformComponentRelativeTo(
    const AxrTransformComponent* child,
    const AxrTransformComponent* parent
) {
    if (child == nullptr) {
        return AxrTransformComponent{};
    }

    if (parent == nullptr) {
        return AxrTransformComponent{
            .Position = child->Position,
            .Scale = child->Scale,
            .Orientation = child->Orientation,
        };
    }

    return AxrTransformComponent{
        .Position = parent->Position + (parent->Orientation * child->Position),
        .Scale = parent->Scale * child->Scale,
        .Orientation = parent->Orientation * child->Orientation,
    };
}

// ----------------------------------------- //
// Internal Functions
// ----------------------------------------- //

glm::mat4 axrTransformGetMatrix(const AxrTransformComponent& transformComponent) {
    const glm::mat4 translate = glm::translate(glm::mat4(1.0f), transformComponent.Position);
    const glm::mat4 rotation = glm::toMat4(transformComponent.Orientation);
    const glm::mat4 scale = glm::scale(glm::mat4(1.0f), transformComponent.Scale);
    return translate * rotation * scale;
}
