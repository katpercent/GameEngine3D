#ifndef MESHES_HPP
#define MESHES_HPP

#include "Mesh.hpp"
#include "vec4.hpp"

class World {
public:
	std::vector<Mesh> meshes;
	std::string PathObject3D;
    std::vector<std::string> mesh;
    std::vector<vec4> MeshTranslation;
    std::vector<vec4> MeshRotationSize;
    vec4 WorldRotation;

    World(std::vector<std::string>, std::vector<vec4>, std::vector<vec4>, vec4);
};

#endif
