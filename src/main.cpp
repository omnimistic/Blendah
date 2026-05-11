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
        float nearPlane = -3.9f;

        auto getRotated = [&](Vector3 v){
            v = RotateY(v, camRotY);
            v = RotateX(v, camRotX);
            return v;
        };

        // Draw the axes
        float axisLen = 100.0f;

        std::vector<std::pair<Vector3, ftxui::Color>> axisPts = {
            {{axisLen, 0, 0}, ftxui::Color::Red}, {{-axisLen, 0, 0}, ftxui::Color::Red},
            {{0, axisLen, 0}, ftxui::Color::Green}, {{0, -axisLen, 0}, ftxui::Color::Green},
            {{0, 0, axisLen}, ftxui::Color::Blue}, {{0, 0, -axisLen}, ftxui::Color::Blue}
        };

        Vector3 origin_rot = getRotated(Vector3(0,0,0));
        Vector2 pOrig = ProjectToScreen(origin_rot, camZoom, 4.0f);

        for(auto& axis : axisPts){
            Vector3 v = getRotated(axis.first);
            if(v.z < nearPlane){
                float t = nearPlane / v.z;
                v.x *= t; v.y *= t; v.z = nearPlane;
            }
            Vector2 pAxis = ProjectToScreen(v, camZoom, 4.0f);
            my_canvas.DrawPointLine(pOrig.x + centerX, pOrig.y + centerY, pAxis.x + centerX, pAxis.y + centerY, axis.second);
        }

        // Draw Mesh Edges
        for(size_t i = 0; i < defaultCube.edges.size(); ++i){
            
            std::pair<int, int> edge = defaultCube.edges[i];
            Vector3 v1 = getRotated(defaultCube.vertices[edge.first]);
            Vector3 v2 = getRotated(defaultCube.vertices[edge.second]);

            if(v1.z > nearPlane && v2.z > nearPlane){

                Vector2 p1 = ProjectToScreen(v1, camZoom, 4.0f);
                Vector2 p2 = ProjectToScreen(v2, camZoom, 4.0f);

                my_canvas.DrawPointLine(p1.x + centerX, p1.y + centerY, p2.x + centerX, p2.y + centerY, ftxui::Color::White);
            }
        }

        // Draw Vertices
        for(int i = 0; i < (int)defaultCube.vertices.size(); ++i){

            Vector3 v = getRotated(defaultCube.vertices[i]);

            if(v.z < nearPlane) continue;
            Vector2 p = ProjectToScreen(v, camZoom, 4.0f);
            ftxui::Color vCol = ftxui::Color::Orange1;

            if(currentMode == appMode::EDIT && i == selectedVertex){
                vCol = ftxui::Color::Red;
                my_canvas.DrawPointCircle(p.x + centerX, p.y + centerY, 4, ftxui::Color::Yellow);
            }

            my_canvas.DrawPoint(p.x + centerX, p.y + centerY, true, vCol);
        }

        std::string mode_text = (currentMode == appMode::VIEW) ? "MODE: VIEW (WASD=Rot, +/-=Zoom, TAB=Edit)" : "MODE: EDIT (WASD=Spatial Select, Arrows=Rel Move, TAB=View)";
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

                        Vector3 v = RotateY(defaultCube.vertices[i], camRotY);
                        v = RotateX(v, camRotX);

                        Vector2 p = ProjectToScreen(v, camZoom, 4.0f);

                        float d = p.x * p.x + p.y * p.y;
                        if(d < minDist){ minDist = d; selectedVertex = i; }
                    }
                }
            }else{currentMode = appMode::VIEW;}
            return true;
        }

        // Reset camera
        if(event == ftxui::Event::Character('r')){
            camRotX = 0.0f;
            camRotY = 0.0f;
            camZoom = 20.0f;
            return true;
        }

        // View mode controls
        if(currentMode == appMode::VIEW){
            if(event == ftxui::Event::Character('w')) {camRotX += 0.1f; return true;}
            if(event == ftxui::Event::Character('s')) {camRotX -= 0.1f; return true;}
            if(event == ftxui::Event::Character('a')) {camRotY -= 0.1f; return true;}
            if(event == ftxui::Event::Character('d')) {camRotY += 0.1f; return true;}

            if(event == ftxui::Event::Character('+') || event == ftxui::Event::Character('=')){
                camZoom += 2.0f;
                return true;
            }
            if(event == ftxui::Event::Character('-') || event == ftxui::Event::Character('_')){
                camZoom -= 2.0f;
                if(camZoom < 1.0f) camZoom = 1.0f;
                return true;
            }

        }else{
            // Spatial Selection Logic relative to current view
            auto jumpSelection = [&](float dirX, float dirY) {
                if (selectedVertex == -1) return;
                
                Vector3 v_curr = RotateY(defaultCube.vertices[selectedVertex], camRotY);
                v_curr = RotateX(v_curr, camRotX);
                Vector2 p_curr = ProjectToScreen(v_curr, camZoom, 4.0f);

                int bestIdx = -1;
                float minScore = 1e10;

                for (int i = 0; i < (int)defaultCube.vertices.size(); ++i) {
                    if (i == selectedVertex) continue;

                    Vector3 v_next = RotateY(defaultCube.vertices[i], camRotY);
                    v_next = RotateX(v_next, camRotX);
                    if (v_next.z < -3.9f) continue; // Skip vertices behind camera

                    Vector2 p_next = ProjectToScreen(v_next, camZoom, 4.0f);
                    float sdx = p_next.x - p_curr.x;
                    float sdy = p_next.y - p_curr.y;

                    // Dot product to check if vertex is in the input direction
                    float dot = sdx * dirX + sdy * dirY;
                    if (dot > 0.1f) {
                        // Scoring heuristic to favour vertices closer and more aligned with the axis
                        float score = (sdx * sdx + sdy * sdy) / (dot * dot);
                        if (score < minScore) {
                            minScore = score;
                            bestIdx = i;
                        }
                    }
                }
                if (bestIdx != -1) selectedVertex = bestIdx;
            };

            if(event == ftxui::Event::Character('w')) { jumpSelection(0, -1); return true; }
            if(event == ftxui::Event::Character('s')) { jumpSelection(0, 1); return true; }
            if(event == ftxui::Event::Character('a')) { jumpSelection(-1, 0); return true; }
            if(event == ftxui::Event::Character('d')) { jumpSelection(1, 0); return true; }
            
            float dmx = 0, dmy = 0;
            bool do_move = false;
            if(event == ftxui::Event::ArrowUp)    {dmy = -0.1f; do_move = true;}
            if(event == ftxui::Event::ArrowDown)  {dmy =  0.1f; do_move = true;}
            if(event == ftxui::Event::ArrowLeft)  {dmx = -0.1f; do_move = true;}
            if(event == ftxui::Event::ArrowRight) {dmx =  0.1f; do_move = true;}

            if(do_move && selectedVertex != -1){
                Vector3 delta(dmx, dmy, 0);
                // Inverse rotation for relative 3D movement
                delta = RotateX(delta, -camRotX);
                delta = RotateY(delta, -camRotY);
                defaultCube.vertices[selectedVertex].x += delta.x;
                defaultCube.vertices[selectedVertex].y += delta.y;
                defaultCube.vertices[selectedVertex].z += delta.z;
                return true;
            }
        }
        return false;
    });

    screen.Loop(event_listener);

    return 0;
}
