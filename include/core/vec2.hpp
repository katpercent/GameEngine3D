#ifndef VEC2_HPP
#define VEC2_HPP

class vec2 {
public:
    // Components
    float x, y;

    // Default constructor
    vec2();

    // Constructor for x, y, z, w
    vec2(float);
    vec2(float x, float y);

    // Copy constructor
    vec2(const vec2& other);

    // Copy assignment operator
    vec2& operator=(const vec2& other) {
        if (this == &other) return *this;
        x = other.x;
        y = other.y;
        return *this;
    }
};
#endif
