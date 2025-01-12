#include "../../include/core/vec3.hpp"

// Default constructor
vec3::vec3() : x(0.0), y(0.0), w(1.0) {}

// Parameterized constructor
vec3::vec3(float a) : x(a), y(a), w(1.0f) {} 
vec3::vec3(float x, float y, float w) : x(x), y(y), w(w) {}

// Copy constructor
vec3::vec3(const vec3& other) : x(other.x), y(other.y), w(other.w) {}
