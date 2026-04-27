#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/terminal.hpp>

enum class appMode{VIEW, EDIT};

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

    if (z_adjusted != 0.0f){
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

    appMode currentMode = appMode::VIEW;
    
    int selectedVertex = -1;
    
    float camRotX = 0.0f;
    float camRotY = 0.0f;

    float camZoom = 20.0f;

    ftxui::Component renderer = ftxui::Renderer([&]{

        ftxui::Dimensions term_size = ftxui::Terminal::Size();
        
        int canvas_width = term_size.dimx * 2;
        int canvas_height = term_size.dimy * 4;
        
        ftxui::Canvas my_canvas = ftxui::Canvas(canvas_width, canvas_height);

        // Find the center of the window
        int centerX = canvas_width / 2;
        int centerY = canvas_height / 2;

        // XYZ Axes - now extending in both directions (very long)
        float axisLength = 100.0f; // large value so axes appear to go across the whole screen

        Vector3 orig(0,0,0);
        Vector3 xPos(axisLength, 0, 0);
        Vector3 xNeg(-axisLength, 0, 0);
        Vector3 yPos(0, axisLength, 0);
        Vector3 yNeg(0, -axisLength, 0);
        Vector3 zPos(0, 0, axisLength);
        Vector3 zNeg(0, 0, -axisLength);

        // Project axes WITHOUT any rotation - they remain fixed
        Vector2 pOrig = ProjectToScreen(orig, camZoom, 4.0f);
        Vector2 pX1  = ProjectToScreen(xPos, camZoom, 4.0f);
        Vector2 pX2  = ProjectToScreen(xNeg, camZoom, 4.0f);
        Vector2 pY1  = ProjectToScreen(yPos, camZoom, 4.0f);
        Vector2 pY2  = ProjectToScreen(yNeg, camZoom, 4.0f);
        Vector2 pZ1  = ProjectToScreen(zPos, camZoom, 4.0f);
        Vector2 pZ2  = ProjectToScreen(zNeg, camZoom, 4.0f);

        // Draw axes with colors: X=Red, Y=Green, Z=Blue
        my_canvas.DrawPointLine(pOrig.x + centerX, pOrig.y + centerY, pX1.x + centerX, pX1.y + centerY, ftxui::Color::Red);
        my_canvas.DrawPointLine(pOrig.x + centerX, pOrig.y + centerY, pX2.x + centerX, pX2.y + centerY, ftxui::Color::Red);
        my_canvas.DrawPointLine(pOrig.x + centerX, pOrig.y + centerY, pY1.x + centerX, pY1.y + centerY, ftxui::Color::Green);
        my_canvas.DrawPointLine(pOrig.x + centerX, pOrig.y + centerY, pY2.x + centerX, pY2.y + centerY, ftxui::Color::Green);
        my_canvas.DrawPointLine(pOrig.x + centerX, pOrig.y + centerY, pZ1.x + centerX, pZ1.y + centerY, ftxui::Color::Blue);
        my_canvas.DrawPointLine(pOrig.x + centerX, pOrig.y + centerY, pZ2.x + centerX, pZ2.y + centerY, ftxui::Color::Blue);

        // Draw Mesh Edges
        for(size_t i = 0; i < defaultCube.edges.size(); ++i){
            
            std::pair<int, int> edge = defaultCube.edges[i];

            Vector3 v1 = defaultCube.vertices[edge.first];
            Vector3 v2 = defaultCube.vertices[edge.second];

            v1 = RotateX(v1, camRotX);
            v1 = RotateY(v1, camRotY);

            v2 = RotateX(v2, camRotX);
            v2 = RotateY(v2, camRotY);

            Vector2 p1 = ProjectToScreen(v1, camZoom, 4.0f);
            Vector2 p2 = ProjectToScreen(v2, camZoom, 4.0f);

            // Mesh edges remain white for contrast against background and axes
            my_canvas.DrawPointLine(p1.x + centerX, p1.y + centerY, p2.x + centerX, p2.y + centerY, ftxui::Color::White);
        }

        // --- Draw Vertices ---
        for(int i = 0; i < (int)defaultCube.vertices.size(); ++i){
            Vector3 v = defaultCube.vertices[i];
            v = RotateX(v, camRotX);
            v = RotateY(v, camRotY);
            Vector2 p = ProjectToScreen(v, camZoom, 4.0f);

            ftxui::Color vertexColor = ftxui::Color::Orange1; // Default: Orange

            if(currentMode == appMode::EDIT){
                if(i == selectedVertex){
                    vertexColor = ftxui::Color::Red; // Selected in Edit mode: Red

                    // Draw selection circle in Yellow
                    my_canvas.DrawPointCircle(p.x + centerX, p.y + centerY, 4, ftxui::Color::Yellow);
                }
            }
            
            // Draw the vertex point
            my_canvas.DrawPoint(p.x + centerX, p.y + centerY, true, vertexColor);
        }

        std::string mode_text = (currentMode == appMode::VIEW) ? "MODE: VIEW (WASD=Rot, +/-=Zoom, TAB=Edit)" : "MODE: EDIT (WASD=Select, Arrows=Move, TAB=View)";
        return ftxui::vbox({
            ftxui::text(mode_text) | ftxui::bold,
            ftxui::canvas(std::move(my_canvas)) | ftxui::flex
        });
    });

    ftxui::Component event_listener = ftxui::CatchEvent(renderer, [&](ftxui::Event event){
        if(event == ftxui::Event::Tab){
            if(currentMode == appMode::VIEW){
                currentMode = appMode::EDIT;
                if(selectedVertex == -1){
                    float minDist = 1e10;
                    for(int i = 0; i < (int)defaultCube.vertices.size(); ++i){
                        Vector3 v = defaultCube.vertices[i];
                        v = RotateX(v, camRotX);
                        v = RotateY(v, camRotY);
                        Vector2 p = ProjectToScreen(v, camZoom, 4.0f);
                        float distSq = p.x * p.x + p.y * p.y;
                        if(distSq < minDist){
                            minDist = distSq;
                            selectedVertex = i;
                        }else if(std::abs(distSq - minDist) < 0.001f){
                            if(p.y < defaultCube.vertices[selectedVertex].y) selectedVertex = i;
                        }
                    }
                }
            }else{
                currentMode = appMode::VIEW;
            }
            return true;
        }

        if(event == ftxui::Event::Character('r')){
            camRotX = 0.0f; camRotY = 0.0f; camZoom = 20.0f;
            return true;
        }

        if(currentMode == appMode::VIEW){
            if(event == ftxui::Event::Character('w')) {camRotX -= 0.1f; return true;}
            if(event == ftxui::Event::Character('s')) {camRotX += 0.1f; return true;}
            if(event == ftxui::Event::Character('a')) {camRotY -= 0.1f; return true;}
            if(event == ftxui::Event::Character('d')) {camRotY += 0.1f; return true;}
            if(event == ftxui::Event::Character('q')) {camRotX -= 0.1f; camRotY -= 0.1f; return true;}
            if(event == ftxui::Event::Character('e')) {camRotX -= 0.1f; camRotY += 0.1f; return true;}
            if(event == ftxui::Event::Character('z')) {camRotX += 0.1f; camRotY -= 0.1f; return true;}
            if(event == ftxui::Event::Character('c')) {camRotX += 0.1f; camRotY += 0.1f; return true;}
            if(event == ftxui::Event::Character('+') || event == ftxui::Event::Character('=')){camZoom += 2.0f; return true;}
            if(event == ftxui::Event::Character('-') || event == ftxui::Event::Character('_')){
                camZoom -= 2.0f; if(camZoom < 1.0f) camZoom = 1.0f; return true;
            }
        }else{
            if(event == ftxui::Event::Character('d')) {selectedVertex = (selectedVertex + 1) % defaultCube.vertices.size(); return true;}
            if(event == ftxui::Event::Character('a')) {selectedVertex = (selectedVertex - 1 + defaultCube.vertices.size()) % defaultCube.vertices.size(); return true;}
            if(event == ftxui::Event::ArrowUp)    {defaultCube.vertices[selectedVertex].y -= 0.1f; return true;}
            if(event == ftxui::Event::ArrowDown)  {defaultCube.vertices[selectedVertex].y += 0.1f; return true;}
            if(event == ftxui::Event::ArrowLeft)  {defaultCube.vertices[selectedVertex].x -= 0.1f; return true;}
            if(event == ftxui::Event::ArrowRight) {defaultCube.vertices[selectedVertex].x += 0.1f; return true;}
        }
        return false;
    });

    screen.Loop(event_listener);

    return 0;
}