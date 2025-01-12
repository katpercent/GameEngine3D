#include "../../include/core/vec4.hpp"
#include <cmath>

// Default constructor
vec4::vec4() : x(0.0), y(0.0), z(0.0), w(1.0) {}

// Parameterized constructor
vec4::vec4(float a) : x(a), y(a), z(a), w(1.0f) {} 
vec4::vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

// Copy constructor
vec4::vec4(const vec4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

// Overloaded operators
vec4 vec4::operator+(const vec4& other) const {
    return vec4(x + other.x, y + other.y, z + other.z, w);
}

vec4 vec4::operator-(const vec4& other) const {
    return vec4(x - other.x, y - other.y, z - other.z, w);
}

vec4 vec4::operator-() const {
    return vec4(-x, -y, -z, w); // Inverse x, y, z, mais garde w inchangé
}

vec4 vec4::operator*(float scalar) const {
    return vec4(x * scalar, y * scalar, z * scalar, w);
}

vec4 vec4::operator/(float scalar) const {
    if (std::abs(scalar) < std::numeric_limits<float>::epsilon()) {
        //throw std::invalid_argument("Division by zero or near-zero scalar!");
        return *this;
    }
    return vec4(x / scalar, y / scalar, z / scalar, w);
}

vec4 vec4::lerp(const vec4& other, float t) const {
    return *this * (1 - t) + other * t;
}

// Dot product
float vec4::dot(const vec4& other) const {
    return x * other.x + y * other.y + z * other.z;
}

vec4 vec4::operator*(const vec4& other) const {
    float newX = x * other.x + x * other.y + x * other.z;
    float newY = y * other.x + y * other.y + z * other.z;
    float newZ = z * other.x + z * other.y + z * other.z;
    return vec4(newX, newY, newZ, 1.0);
}

// Magnitude (length) of the vector
float vec4::magnitude() const {
    return std::sqrt(x * x + y * y + z * z);
}

// Normalize the vector
vec4 vec4::normalize() const {
    float mag = magnitude();
    if (mag == 0) {
        //throw std::invalid_argument("Cannot normalize a zero-length vector!");
        return *this;
        
    }
    return *this / mag;
}

vec4 vec4::reflect(const vec4& normal) const {
    vec4 reflectVector;
    float dotProduct = this->dot(normal);
    if (std::abs(dotProduct) < 0.001f) {
        reflectVector = -*this;
    } else {
        reflectVector = *this - 2.f * dotProduct * normal;
    }
    return reflectVector;
}

// Cross product (3D) - using the first three components of both vectors
vec4 vec4::crossProduct(const vec4& other) const {
    float crossX = y * other.z - z * other.y;
    float crossY = z * other.x - x * other.z;
    float crossZ = x * other.y - y * other.x;
    
    // The w component is not affected by the cross product
    return vec4(crossX, crossY, crossZ, w);
}

// Swizzling methods (optional)
vec4 vec4::xyzw() const {
    return *this;
}

vec4 vec4::xyz() const {
    return vec4(x, y, z, 0);
}

vec4 vec4::xy() const {
    return vec4(x, y, 0, 0);
}

// Overloaded output operator for easy printing
std::ostream& operator<<(std::ostream& os, const vec4& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
    return os;
}
