#include "../../include/core/vec2.hpp"

// Default constructor
vec2::vec2() : x(0.0), y(0.0) {}

// Parameterized constructor
vec2::vec2(float a) : x(a), y(a) {} 
vec2::vec2(float x, float y) : x(x), y(y) {}

// Copy constructor
vec2::vec2(const vec2& other) : x(other.x), y(other.y) {}
