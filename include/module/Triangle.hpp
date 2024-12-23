#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#pragma once
#include <SDL2/SDL.h>
#include "vec4.hpp"

#pragma once

class triangle {
public:
	vec4 p[3];
    SDL_Color col[3];
};

#endif
