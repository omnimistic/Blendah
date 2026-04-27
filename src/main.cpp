#include <iostream>
#include <vector>

struct Vector2{
    float x, y;

    Vector2(float x = 0.0f, float y = 0.0f){
        this->x = x;
        this->y = y;
    }
};

struct Vector3 {
    float x, y, z;

    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
};

struct Mesh {

    std::vector<Vector3> vertices;

    std::vector<std::pair<int, int>> edges;
};


Mesh createCube() {
    Mesh cube;

    // 8 corners of the cube
    cube.vertices = {
        {-1, -1, -1},
        { 1, -1, -1},
        { 1,  1, -1},
        {-1,  1, -1},
        {-1, -1,  1},
        { 1, -1,  1},
        { 1,  1,  1},
        {-1,  1,  1}
    };

    // The 12 edges connecting the vertices
    cube.edges = {
        // Front face
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        
        // Back face
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        
        // Connecting lines between the faces
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    return cube;
}


int main() {
    
    return 0;
}
