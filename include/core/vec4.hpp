#ifndef VEC4_HPP
#define VEC4_HPP

#include <iostream>

class vec4 {
public:
    // Components
    float x, y, z, w;

    // Default constructor
    vec4();

    // Constructor for x, y, z, w
    vec4(float x, float y, float z, float w = 1.0f);

    // Copy constructor
    vec4(const vec4& other);

    // Copy assignment operator
    vec4& operator=(const vec4& other) {
        if (this == &other) return *this;
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

    // Overloaded operators
    vec4 operator+(const vec4& other) const;
    vec4 operator-(const vec4& other) const;
    vec4 operator-() const;
    vec4 operator*(float scalar) const;
    vec4 operator/(float scalar) const;
    vec4 operator*(const vec4& other) const;
    bool operator==(const vec4& other) const;
    bool operator!=(const vec4& other) const;
    
    // linear interpolation between two vectors
    vec4 lerp(const vec4& other, float t) const;

    // Dot product
    float dot(const vec4& other) const;

    // Magnitude (length) of the vector
    float magnitude() const;

    // Normalize the vector
    vec4 normalize() const;

    // Cross product (3D) - defined using the first three components (x, y, z)
    vec4 crossProduct(const vec4& other) const;

    // Swizzling methods
    vec4 xyzw() const;
    vec4 xyz() const;
    vec4 xy() const;

    // Overloaded output operator for easy printing
    friend std::ostream& operator<<(std::ostream& os, const vec4& vec);
};

#endif
