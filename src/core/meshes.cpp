
#include "../../include/module/Meshes.hpp"

Meshes::Meshes(std::vector<std::string> mesh, std::vector<vec4> MeshCoo, std::vector<vec4> MeshRot) {
    this->mesh = mesh;
    this->MeshCoo = MeshCoo;
    this->MeshRot = MeshRot;

    for (int m = 0; m < (this->mesh.size()); m++)
    {

        class Mesh NewMesh;
        this->meshes.push_back(NewMesh);
        this->meshes[m].path = PathObject3D + mesh[m];
        this->meshes[m].LoadFromObjectFile(mesh[m]);
        this->meshes[m].coo = MeshCoo[m];
        this->meshes[m].rot = MeshRot[m];
        this->meshes[m].Scale(MeshCoo[m].w);
    }
}
