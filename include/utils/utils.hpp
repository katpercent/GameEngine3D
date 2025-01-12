#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <SDL2/SDL.h>
#include "../core/vec4.hpp"
#include "../core/Triangle.hpp"
#include "../core/Camera.hpp"
#include "../core/Lighting.hpp"

std::string getCurrentDirectory();
int maxValue(int, int);
int minValue(int, int);
double degreesToRadians(double);
double timeToRadians(double&);
vec4 Vector_IntersectPlane(vec4&, vec4&, vec4&, vec4&, float&);
void DrawTriangleDepthBuffer(SDL_Renderer*, triangle&, float*, int, int, int&);
int Triangle_ClipAgainstPlane(vec4, vec4, triangle&, triangle&, triangle&);
void DrawTriangleDepthBufferPerformance(
    SDL_Renderer* renderer,
    triangle& triProjected,
    triangle& triLightSpace,
    float* depthBuffer,
    float* shadowMap,
    int& ScreenSizeX,
    int& ScreenSizeY,
    int& numPixels,
    Camera& camera,
    Lighting& light,
    unsigned int DEPTH_BUFFER_RES_X,
    unsigned int DEPTH_BUFFER_RES_Y,
    int& numShadow);
void ShadowMap(
    triangle& tri,
    float* shadowMap,
    int& ScreenSizeX,
    int& ScreenSizeY,
    unsigned int DEPTH_BUFFER_RES_X,
    unsigned int DEPTH_BUFFER_RES_Y,
    int& numPixels,
    int& numShadow);
#endif
