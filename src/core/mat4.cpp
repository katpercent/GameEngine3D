#include "../../include/core/mat4.hpp"
#include "../../include/core/vec4.hpp"

vec4 mat4::operator*(const vec4& vec) const {
    // Perform matrix-vector multiplication (result = matrix * vector)
    float newX = vec.x * m[0][0] + vec.y * m[0][1] + vec.z * m[0][2] + vec.w * m[0][3];
    float newY = vec.x * m[1][0] + vec.y * m[1][1] + vec.z * m[1][2] + vec.w * m[1][3];
    float newZ = vec.x * m[2][0] + vec.y * m[2][1] + vec.z * m[2][2] + vec.w * m[2][3];
    float newW = vec.x * m[3][0] + vec.y * m[3][1] + vec.z * m[3][2] + vec.w * m[3][3];
    return vec4(newX, newY, newZ, newW);
}

mat4 mat4::operator*(const mat4& matrix) const {
    mat4 mat;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            mat.m[r][c] = m[r][0] * matrix.m[0][c] + m[r][1] * matrix.m[1][c] + m[r][2] * matrix.m[2][c] + m[r][3] * matrix.m[3][c];
    return matrix;
}

void mat4::MatrixMakeIdentity() {
    this->m[0][0] = 1.0;
    this->m[1][1] = 1.0;
    this->m[2][2] = 1.0;
    this->m[3][3] = 1.0;
};

