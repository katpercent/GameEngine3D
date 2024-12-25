#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include "../core/vec4.hpp"
#include "../core/Triangle.hpp"
#include "../core/mat4.hpp"

std::string getCurrentDirectory();
int maxValue(int, int);
int minValue(int, int);
double degreesToRadians(double);
double timeToRadians(double&);
mat4 MatrixPointAt(vec4&, vec4&, vec4&);
mat4 MatrixTranslation(vec4&);
mat4 MatrixRotationX(float&);
mat4 MatrixRotationY(float&);
mat4 MatrixRotationZ(float&);
vec4 Vector_IntersectPlane(vec4&, vec4&, vec4&, vec4&, float&);
int Triangle_ClipAgainstPlane(vec4, vec4, triangle&, triangle&, triangle&);

#endif
