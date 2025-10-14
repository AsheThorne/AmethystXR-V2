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

const char* axrToString(const Clay_ErrorType errorType) {
    switch (errorType) {
        case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_FUNCTION_NOT_PROVIDED: {
            return "CLAY_ERROR_TYPE_TEXT_MEASUREMENT_FUNCTION_NOT_PROVIDED";
        }
        case CLAY_ERROR_TYPE_ARENA_CAPACITY_EXCEEDED: {
            return "CLAY_ERROR_TYPE_ARENA_CAPACITY_EXCEEDED";
        }
        case CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED: {
            return "CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED";
        }
        case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED: {
            return "CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED";
        }
        case CLAY_ERROR_TYPE_DUPLICATE_ID: {
            return "CLAY_ERROR_TYPE_DUPLICATE_ID";
        }
        case CLAY_ERROR_TYPE_FLOATING_CONTAINER_PARENT_NOT_FOUND: {
            return "CLAY_ERROR_TYPE_FLOATING_CONTAINER_PARENT_NOT_FOUND";
        }
        case CLAY_ERROR_TYPE_PERCENTAGE_OVER_1: {
            return "CLAY_ERROR_TYPE_PERCENTAGE_OVER_1";
        }
        case CLAY_ERROR_TYPE_INTERNAL_ERROR: {
            return "CLAY_ERROR_TYPE_INTERNAL_ERROR";
        }
        default: {
            return "UNKNOWN_ERROR";
        }
    }
}
