#include "../include/math.h"
#include <cmath>

Vector2 ProjectToScreen(Vector3 point, float scale, float zOff){

    float z = point.z + zOff;
    if (z == 0.0f) return Vector2(0, 0);
    return Vector2((point.x / z) * scale, (point.y / z) * scale);
}

Vector3 RotateX(Vector3 p, float a){
    return Vector3(p.x, p.y * std::cos(a) - p.z * std::sin(a), p.y * std::sin(a) + p.z * std::cos(a));
}

Vector3 RotateY(Vector3 p, float a){
    return Vector3(p.x * std::cos(a) + p.z * std::sin(a), p.y, -p.x * std::sin(a) + p.z * std::cos(a));
}