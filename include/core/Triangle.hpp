#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "vec4.hpp"
#include "vec2.hpp"

class triangle {
public:
	vec4 p[3];
    vec4 col[3];
    vec2 tex[3];
    vec4 normal;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

#endif
