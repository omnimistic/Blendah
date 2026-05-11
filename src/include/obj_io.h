#pragma once
#include "types.h"
#include "mesh.h"
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>

// Helper to extract just the vertex index from "v/vt/vn" formats
inline int parseVertexIndex(const std::string& token){

    size_t slashPos = token.find('/');
    if (slashPos != std::string::npos){
        return std::stoi(token.substr(0, slashPos));
    }
    return std::stoi(token);
}

inline Mesh LoadOBJ(const std::string& filepath){

    Mesh mesh;
    std::ifstream file(filepath);
    if (!file.is_open()) return mesh; // Return empty mesh if file not found

    std::string line;
    // Using a set to automatically ignore duplicate edges (e.g. two faces sharing the same edge)
    std::set<std::pair<int, int>> unique_edges;

    while (std::getline(file, line)){

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v"){
            float x, y, z;
            iss >> x >> y >> z;
            // Fliping the Y axis so it renders right-side up in the terminal since the y axis is inverted in blendah
            mesh.vertices.push_back(Vector3(x, -y, z));
        } 
        else if (type == "f" || type == "l"){
            std::vector<int> indices;
            std::string token;
            
            while (iss >> token){
                int idx = parseVertexIndex(token);
                if (idx > 0){
                    indices.push_back(idx - 1); // Convert 1-based OBJ index to 0-based C++ index
                }
            }

            if (indices.size() >= 2){
                for (size_t i = 0; i < indices.size(); ++i){

                    int v1 = indices[i];
                    int v2;

                    if (type == "f"){
                        // Faces form a closed loop, so the last vertex connects back to the first
                        v2 = indices[(i + 1) % indices.size()];
                    } else {
                        // Lines 'l' are just strips, they dont loop back
                        if (i == indices.size() - 1) break;
                        v2 = indices[i + 1];
                    }

                    // Sorting the pair so (1, 2) and (2, 1) are treated as the same edge
                    int min_v = std::min(v1, v2);
                    int max_v = std::max(v1, v2);
                    unique_edges.insert({min_v, max_v});
                }
            }
        }
    }

    // Finally moving the unique edges into our mesh
    for (const auto& edge : unique_edges){
        mesh.edges.push_back(edge);
    }

    return mesh;
}

inline void ExportOBJ(const Mesh& mesh, const std::string& filepath){
    
    std::ofstream file(filepath);
    
    if (!file.is_open()) return;

    file << "# Exported by Blendah OS\n";

    for (const auto& v : mesh.vertices){
        // Un-flip the y axis so standard 3D software reads it correctly
        file << "v " << v.x << " " << -v.y << " " << v.z << "\n";
    }

    for (const auto& e : mesh.edges){
        // Export as 'l' since it only supports wireframe at the moment
        // Convert 0-based back to 1-based
        file << "l " << (e.first + 1) << " " << (e.second + 1) << "\n";
    }
}