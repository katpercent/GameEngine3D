#ifndef MAT4_HPP
#define MAT4_HPP

#include "vec4.hpp"

class mat4 {
public:
	float m[4][4] = { 0.0f };

	vec4 operator*(const vec4& vec) const;
	mat4 operator*(const mat4& matrix) const;
	void MatrixMakeIdentity();
	void Translation(vec4&);
	void RotationX(float&);
	void RotationY(float&);
	void RotationZ(float&);
	friend std::ostream& operator<<(std::ostream&, mat4&);
};

#endif
