
#include "../../include/core/World.hpp"
#include <cstddef>

World::World(std::vector<std::string> meshes, std::vector<vec4> MeshTranslat, std::vector<vec4> MeshRotSize, vec4 WorldRot) {
    this->mesh = meshes;
    this->MeshTranslation = MeshTranslat;
    this->MeshRotationSize = MeshRotSize;
    this->WorldRotation = WorldRot;

    for (size_t m = 0; m < (this->mesh.size()); m++) {
        class Mesh NewMesh;
        
        NewMesh.path = PathObject3D + this->mesh[m];
        NewMesh.LoadFromObjectFile(this->mesh[m]);
        NewMesh.position = this->MeshTranslation[m];
        NewMesh.rotation = this->MeshRotationSize[m];
        NewMesh.Scale(this->MeshTranslation[m].w);
        this->meshes.push_back(NewMesh);
    }
}
