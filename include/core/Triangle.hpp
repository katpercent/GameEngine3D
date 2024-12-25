#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include "vec4.hpp"

class triangle {
public:
	vec4 p[3];
    vec4 col[3];
};

#endif
