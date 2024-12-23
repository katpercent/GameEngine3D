#ifndef MESHES_HPP
#define MESHES_HPP
#pragma once
#include "Mesh.hpp"
#include "vec4.hpp"

class Meshes {
public:
	std::vector<Mesh> meshes;
	std::string PathObject3D;
    std::vector<std::string> mesh;
    std::vector<vec4> MeshCoo;
    std::vector<vec4> MeshRot;

    Meshes(std::vector<std::string> mesh, std::vector<vec4> MeshCoo, std::vector<vec4> MeshRot);
};

#endif
