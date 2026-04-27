#include <iostream>
#include <vector>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/elements.hpp>


struct Vector2{
    
    float x, y;

    Vector2(float x = 0.0f, float y = 0.0f){
        this->x = x;
        this->y = y;
    }
};

struct Vector3{

    float x, y, z;

    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
};

struct Mesh{

    std::vector<Vector3> vertices;

    std::vector<std::pair<int, int>> edges;
};


Mesh createCube(){

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

Vector2 ProjectToScreen(Vector3 point, float scale_multiplier, float zOffset){

    float z_adjusted = point.z + zOffset;

    float x_proj = 0.0f; 
    float y_proj = 0.0f;

    if (z_adjusted != 0.0f) {
        x_proj = point.x / z_adjusted;
        y_proj = point.y / z_adjusted;
    }

    x_proj *= scale_multiplier;
    y_proj *= scale_multiplier;

    return Vector2(x_proj, y_proj);
}


int main(){

    ftxui::Canvas my_canvas = ftxui::Canvas(100, 100);

    Mesh defaultCube = createCube();

    for(size_t i = 0; i < defaultCube.edges.size(); ++i){

        std::pair<int, int> edge = defaultCube.edges[i];

        Vector3 v1 = defaultCube.vertices[edge.first];
        Vector3 v2 = defaultCube.vertices[edge.second];

        Vector2 p1 = ProjectToScreen(v1, 20.0f, 4.0f);
        Vector2 p2 = ProjectToScreen(v2, 20.0f, 4.0f);

        my_canvas.DrawPointLine(p1.x + 50, p1.y + 50, p2.x + 50, p2.y + 50);
    }

    ftxui::Element document = ftxui::canvas(std::move(my_canvas));
    ftxui::Screen screen = ftxui::Screen::Create(ftxui::Dimension::Fixed(100), ftxui::Dimension::Fixed(100));
    
    ftxui::Render(screen, document);
    screen.Print();

    return 0;
}
