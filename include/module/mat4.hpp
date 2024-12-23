#ifndef MAT4_HPP
#define MAT4_HPP

#pragma once
#include "vec4.hpp"

class mat4 {
public:
	float m[4][4] = { 0 };
	vec4 operator*(const vec4& vec) const;
	mat4 operator*(const mat4& matrix) const;
	void MatrixMakeIdentity();

};

#endif
