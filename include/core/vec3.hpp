#ifndef VEC3_HPP
#define VEC3_HPP

class vec3 {
public:
    // Components
    float x, y, w;

    // Default constructor
    vec3();

    // Constructor for x, y, z, w
    vec3(float);
    vec3(float x, float y, float w = 1.0f);

    // Copy constructor
    vec3(const vec3& other);

    // Copy assignment operator
    vec3& operator=(const vec3& other) {
        if (this == &other) return *this;
        x = other.x;
        y = other.y;
        w = other.w;
        return *this;
    }
};

#endif
