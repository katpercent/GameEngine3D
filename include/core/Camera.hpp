#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <cmath>
#include "vec4.hpp"
#include "mat4.hpp"

class Camera {
public:
    // Camera properties
    vec4 position;       // Position of the camera
    vec4 forward;        // Forward vector (direction camera is looking at)
    vec4 up;             // Up vector (orientation)
    vec4 right;          // Right vector (perpendicular to forward and up)

    float aspectRatio;
    float fovHorizontal; // Field of View horizontal in degrees
    float fovVertical;   // Field of View horizontal in degrees
    float nearPlane;     // Near clipping plane
    float farPlane;      // Far clipping plane
    float currentPitch = 0.0f; // Tracks the pitch angle

    // Constructor
    Camera(const vec4& pos, const vec4& target, const vec4& upDir, 
           float fovX, float fovY, float nearP, float farP, float aspectR)
        : position(pos), forward(target), up(upDir), fovHorizontal(fovX), fovVertical(fovY), nearPlane(nearP), farPlane(farP), aspectRatio(aspectR) {
        right = forward.crossProduct(upDir).normalize();  // Calculate right vector
    }

    mat4 computeViewMatrix() const {
        // Point at the camera's position, looking in the forward direction
        vec4 target = position + forward;
        // Calculate new forward direction
        vec4 forward = (target - position).normalize();

        // Calculate new up direction
        vec4 a = forward * up.dot(forward);
        vec4 newUp = (up - a).normalize();

        // New right direction
        vec4 right = newUp.crossProduct(forward);

        // Construct rotation matrix
        mat4 cameraRot;
        cameraRot.m[0][0] = right.x;   cameraRot.m[0][1] = right.y;   cameraRot.m[0][2] = right.z;   cameraRot.m[0][3] = -(position.dot(right));
        cameraRot.m[1][0] = newUp.x;   cameraRot.m[1][1] = newUp.y;   cameraRot.m[1][2] = newUp.z;   cameraRot.m[1][3] = -(position.dot(newUp));
        cameraRot.m[2][0] = forward.x; cameraRot.m[2][1] = forward.y; cameraRot.m[2][2] = forward.z; cameraRot.m[2][3] = -(position.dot(forward));
        cameraRot.m[3][0] = 0.0f;      cameraRot.m[3][1] = 0.0f;      cameraRot.m[3][2] = 0.0f;      cameraRot.m[3][3] = 1.0f;
        return cameraRot ;
    }

    // Returns the projection matrix
    mat4 computeProjectionMatrix() const {
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

    mat4 computeSpaceMatrix() const {
        return computeProjectionMatrix() * computeViewMatrix();
    }

    // Moves the camera in the forward direction
    void moveForward(float distance) {
        position = position + (forward * distance);
    }

    // Moves the camera in the right direction
    void moveRight(float distance) {
        position = position + (right * distance);
    }

    // Moves the camera in the up direction
    void moveUp(float distance) {
        position = position + (up * distance);
    }

    // Rotates the camera around the Y-axis (yaw)
    void rotateYaw(float angleDegrees) {
        float angleRad = angleDegrees * (M_PI / 180.0f);
        mat4 rotation;
        rotation.RotationY(angleRad);
        forward = (rotation * forward).normalize();
        right = forward.crossProduct(up).normalize();
    }

    // Rotates the camera around the X-axis (pitch)
    void rotatePitch(float angleDegrees) {
        float newPitch = currentPitch + angleDegrees;

        // Clamp pitch between -45 and 45 degrees
        if (newPitch > 45.0f) {
            angleDegrees -= (newPitch - 45.0f);
            newPitch = 45.0f;
        } else if (newPitch < -45.0f) {
            angleDegrees -= (newPitch + 45.0f);
            newPitch = -45.0f;
        }

        currentPitch = newPitch;
        float angleRad = angleDegrees * (M_PI / 180.0f);
        mat4 rotation; 
        rotation.RotationX(angleRad);
        forward = (rotation * forward).normalize();
        up = right.crossProduct(forward).normalize();
    }

    // Rotates the camera around the Z-axis (roll)
    void rotateRoll(float angleDegrees) {
        float angleRad = angleDegrees * (M_PI / 180.0f);
        mat4 rotation;
        rotation.RotationZ(angleRad);
        up = (rotation * up).normalize();
        right = forward.crossProduct(up).normalize();
    }
};

#endif // CAMERA_H
