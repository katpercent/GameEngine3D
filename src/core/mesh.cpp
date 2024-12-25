#include "../../include/core/Mesh.hpp"
#include <sstream>
#include <fstream>
#include <iostream> // Include for std::cout

// Getter for id
void Mesh::Scale(float scaleFactor) {
    for (auto& tri : tris) {
        for (auto& vertex : tri.p) {
            vertex.x *= scaleFactor;
            vertex.y *= scaleFactor;
            vertex.z *= scaleFactor;
        } 
    }
}

// Setter for id
void Mesh::ChangePos(float x, float y, float z) {
    coo.x = x;
    coo.y = y;
    coo.z = z;
}

// Setter for name
bool Mesh::LoadFromObjectFile(const std::string& sFilename) {
    std::ifstream f(sFilename);
    if (!f.is_open()) {
        std::cerr << "Failed to open file: " << sFilename << std::endl;
        return false;
    }

    // Local cache of vertices
    std::vector<vec4> verts;

    while (!f.eof()) {
        char line[128];
        f.getline(line, 128);

        std::stringstream s;
        s << line;

        char junk;

        if (line[0] == 'v' && line[1] == ' ') {
           vec4 v;
            s >> junk >> v.x >> v.y >> v.z;
            verts.push_back(v);
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            std::vector<int> f;
            std::string segment;
            while (s >> segment) {
                if (segment != "f") {
                    std::stringstream segmentStream(segment);
                    std::string indexStr;
                    getline(segmentStream, indexStr, '/');
                    f.push_back(stoi(indexStr) - 1);
                }
            }

            if (f.size() == 3) {
                tris.push_back({ verts[f[0]], verts[f[1]], verts[f[2]] });
            }
            else if (f.size() > 3) {
                for (size_t i = 1; i < f.size() - 1; ++i) {
                    tris.push_back({ verts[f[0]], verts[f[i]], verts[f[i + 1]] });
                }
            }
        }
    }
    return true;
}
