#pragma once
#include <vector>

enum class appMode {VIEW, EDIT};
enum class LastMoved {NONE, X, Y, Z};

struct Vector2{
    float x, y;
    Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

struct Vector3{
    float x, y, z;
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};

struct Mesh{
    std::vector<Vector3> vertices;
    std::vector<std::pair<int, int>> edges;
};