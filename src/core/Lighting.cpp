#include "../../include/core/Lighting.hpp"
#include <cmath>
#include <iostream>

// Compute view matrix for the light
/*mat4 Lighting::computeViewMatrix() const {
    vec4 up(0.0f, 1.0f, 0.0f);

    // For directional and spotlight, use direction; for point light, use position
    vec4 target = type == POINT ? position : position + direction;

    vec4 zAxis = (position - target).normalize();         // Forward vector
    vec4 xAxis = up.crossProduct(zAxis).normalize();      // Right vector
    vec4 yAxis = zAxis.crossProduct(xAxis).normalize();   // Up vector

    mat4 view;
    view.MatrixMakeIdentity();

    // Set rotation part
    view.m[0][0] = xAxis.x; view.m[0][1] = xAxis.y; view.m[0][2] = xAxis.z;
    view.m[1][0] = yAxis.x; view.m[1][1] = yAxis.y; view.m[1][2] = yAxis.z;
    view.m[2][0] = zAxis.x; view.m[2][1] = zAxis.y; view.m[2][2] = zAxis.z;

    // Set translation part
    view.m[0][3] = -xAxis.dot(position);
    view.m[1][3] = -yAxis.dot(position);
    view.m[2][3] = -zAxis.dot(position);

    return view;
}*/

// Returns the view matrix
mat4 Lighting::computeViewMatrix() const {
    // Point at the camera's position, looking in the forward direction
    vec4 target = position + direction;
    // Calculate new forward direction
    vec4 forward = (target - position).normalize();

    // Calculate new up direction
    vec4 a = forward * up.dot(forward);
    vec4 newUp = (up - a).normalize();

    // New right direction
    vec4 right = newUp.crossProduct(forward);

    // Construct rotation matrix
    mat4 cameraRot;
    cameraRot.m[0][0] = right.x;   cameraRot.m[0][1] = right.y;   cameraRot.m[0][2] = right.z;   cameraRot.m[0][3] = -(position.dot(right));;
    cameraRot.m[1][0] = newUp.x;   cameraRot.m[1][1] = newUp.y;   cameraRot.m[1][2] = newUp.z;   cameraRot.m[1][3] = -(position.dot(newUp));;
    cameraRot.m[2][0] = forward.x; cameraRot.m[2][1] = forward.y; cameraRot.m[2][2] = forward.z; cameraRot.m[2][3] = -(position.dot(forward));;
    cameraRot.m[3][0] = 0.0f;      cameraRot.m[3][1] = 0.0f;      cameraRot.m[3][2] = 0.0f;      cameraRot.m[3][3] = 1.0f;
    return cameraRot ;
}

// Compute projection matrix for the light
mat4 Lighting::computeProjectionMatrix() const {
    if (type == DIRECTIONAL) {
        // Orthographic projection for directional light
        float size = 10.0f; // Adjust the size of the orthographic box as needed
        float left = -size, right = size;
        float bottom = -size, top = size;

        mat4 projection;
        projection.MatrixMakeIdentity();

        projection.m[0][0] = 2.0f / (right - left);
        projection.m[1][1] = 2.0f / (top - bottom);
        projection.m[2][2] = -2.0f / (farPlane - nearPlane);
        projection.m[0][3] = -(right + left) / (right - left);
        projection.m[1][3] = -(top + bottom) / (top - bottom);
        projection.m[2][3] = -(farPlane + nearPlane) / (farPlane - nearPlane);

        return projection;
    } else {
        // Perspective projection for point and spotlights
        mat4 projection;
        
        float FovHorizontal = 1.0f / (aspectRatio * tan(fovHorizontal * 0.5f * (M_PI / 180.0f)));
        float FovVertical = 1.0f / tan(fovVertical * 0.5f * (M_PI / 180.0f)); 

        projection.m[0][0] = FovHorizontal;
        projection.m[1][1] = FovVertical;
        projection.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        projection.m[2][3] = -(2 * farPlane * nearPlane) / (farPlane - nearPlane);
        projection.m[3][2] = -1.0f;

        return projection;
    }
}

mat4 Lighting::computeSpaceMatrix() const {
    return computeProjectionMatrix() * computeViewMatrix();
}

// Print light information
std::ostream& operator<<(std::ostream& os, const Lighting& light) {
    os << "Lighting Info:\n";
    os << "Position: (" << light.position.x << ", " << light.position.y << ", " << light.position.z << ")\n";
    os << "Direction: (" << light.direction.x << ", " << light.direction.y << ", " << light.direction.z << ")\n";
    os << "Color: (" << light.intensity.x << ", " << light.intensity.y << ", " << light.intensity.z << ")\n";
    os << "Type: " << (light.type == DIRECTIONAL ? "Directional" : (light.type == POINT ? "Point" : "Spot")) << "\n";
    os << "Near Plane: " << light.nearPlane << ", Far Plane: " << light.farPlane << "\n";
    os << "fov Horizontal: " << light.fovHorizontal << ", fov Vertical: " << light.fovVertical << " Aspect Ratio: " << light.aspectRatio << "\n";
    return os;
}
