#ifndef MESH_HPP
#define MESH_HPP

#include "Triangle.hpp"
#include "vec4.hpp"
#include <vector>
#include <string>


class Mesh {
public:
	std::string path;
    std::vector<triangle> tris;
    vec4 position;
    vec4 rotation;

    bool LoadFromObjectFile(const std::string& sFilename);
	void Scale(float scaleFactor);
	void ChangePos(float x, float y, float z);
};

#endif
