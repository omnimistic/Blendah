#include <iostream>
#include <vector>
#include <cmath>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/terminal.hpp>


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


Vector3 RotateX(Vector3 point, float angle){

    float new_y = point.y * std::cos(angle) - point.z * std::sin(angle);
    float new_z = point.y * std::sin(angle) + point.z * std::cos(angle);

    return Vector3(point.x, new_y, new_z);
}


Vector3 RotateY(Vector3 point, float angle){

    float new_x = point.x * std::cos(angle) + point.z * std::sin(angle);
    float new_z = -point.x * std::sin(angle) + point.z * std::cos(angle);
    
    return Vector3(new_x, point.y, new_z);
}

int main(){
    
    Mesh defaultCube = createCube();

    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

    float camRotX = 0.0f;
    float camRotY = 0.0f;

    ftxui::Component renderer = ftxui::Renderer([&]{

        ftxui::Dimensions term_size = ftxui::Terminal::Size();
        
        int canvas_width = term_size.dimx * 2;
        int canvas_height = term_size.dimy * 4;
        
        ftxui::Canvas my_canvas = ftxui::Canvas(canvas_width, canvas_height);

        // Find the center of the window
        int centerX = canvas_width / 2;
        int centerY = canvas_height / 2;

        for(size_t i = 0; i < defaultCube.edges.size(); ++i){
            
            std::pair<int, int> edge = defaultCube.edges[i];

            Vector3 v1 = defaultCube.vertices[edge.first];
            Vector3 v2 = defaultCube.vertices[edge.second];

            v1 = RotateX(v1, camRotX);
            v1 = RotateY(v1, camRotY);

            v2 = RotateX(v2, camRotX);
            v2 = RotateY(v2, camRotY);

            Vector2 p1 = ProjectToScreen(v1, 20.0f, 4.0f);
            Vector2 p2 = ProjectToScreen(v2, 20.0f, 4.0f);

            my_canvas.DrawPointLine(p1.x + centerX, p1.y + centerY, p2.x + centerX, p2.y + centerY);
        }

        return ftxui::canvas(std::move(my_canvas));
    });

    // Event listener for the keyboard controls
    ftxui::Component event_listener = ftxui::CatchEvent(renderer, [&](ftxui::Event event){

        if (event == ftxui::Event::Character('w')) {camRotX -= 0.1f; return true;}
        if (event == ftxui::Event::Character('s')) {camRotX += 0.1f; return true;}
        if (event == ftxui::Event::Character('a')) {camRotY -= 0.1f; return true;}
        if (event == ftxui::Event::Character('d')) {camRotY += 0.1f; return true;}
        return false; 
    });

    screen.Loop(event_listener);

    return 0;
}
