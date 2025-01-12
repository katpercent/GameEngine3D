#ifndef LIGHTING_HPP
#define LIGHTING_HPP

#include "vec4.hpp"
#include "mat4.hpp"

enum LightType {
    DIRECTIONAL,
    POINT,
    SPOT
};

class Lighting {
public:
    // Attributes
    vec4 position;       // Position of the light
    vec4 direction;      // Direction of the light
    vec4 up;
    vec4 right;
    LightType type;      // Type of light (directional, point, spot)
    float nearPlane;     // Near clipping plane for projection
    float farPlane;      // Far clipping plane for projection
    float fovHorizontal;           // Field of view (for spotlights)
    float fovVertical;           // Field of view (for spotlights)
    float aspectRatio;   // Aspect ratio for projection
    vec4 intensity;

    // Constructor
    Lighting(const vec4& pos, const vec4& target, const vec4& upDir, vec4 col, LightType t, float fovX, float fovY, float nearP, float farP, float aspectR)
        :position(pos), direction(target.normalize()), up(upDir), intensity(col), type(t), nearPlane(nearP), farPlane(farP), fovHorizontal(fovX), fovVertical(fovY), aspectRatio(aspectR) {
        right = target.crossProduct(upDir).normalize();  // Calculate right vector
        up = right.crossProduct(target).normalize();  // Recalculate up to ensure orthogonality
    }

    // Compute view matrix for the light
    mat4 computeViewMatrix() const;

    // Compute projection matrix for the light
    mat4 computeProjectionMatrix() const;

    mat4 computeSpaceMatrix() const;

    // Print light information
    friend std::ostream& operator<<(std::ostream& os, const Lighting& light);
};

#endif
