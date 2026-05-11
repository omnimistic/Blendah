#include "../include/renderer.h"
#include "../include/math.h"
#include <ftxui/screen/terminal.hpp>

ftxui::Element RenderScene(const Mesh& mesh, const Camera& cam, appMode mode, int selectedVertex,bool inAxisSession, LastMoved lastMoved){

    ftxui::Dimensions term = ftxui::Terminal::Size();

    int cw = term.dimx * 2;
    int ch = term.dimy * 4;
    
    ftxui::Canvas canvas(cw, ch);
    int cx = cw / 2, cy = ch / 2;

    const float NEAR = -3.9f;
    const float ZOFF = 4.0f;

    auto rotated = [&](Vector3 v) -> Vector3{
        v = RotateY(v, cam.rotY);
        v = RotateX(v, cam.rotX);
        return v;
    };

    auto project = [&](Vector3 v) -> Vector2{
        
        v = rotated(v);
        
        if (v.z < NEAR){

            float t = NEAR / v.z;

            v.x *= t;
            v.y *= t;
            v.z = NEAR;
        }
        return ProjectToScreen(v, cam.zoom, ZOFF);
    };

    // World Axes
    const float AL = 100.0f;

    std::pair<Vector3, ftxui::Color> worldAxes[] = {
        {{AL,0,0}, ftxui::Color::Red},
        {{-AL,0,0}, ftxui::Color::Red},

        {{0,AL,0}, ftxui::Color::Green},
        {{0,-AL,0}, ftxui::Color::Green},

        {{0,0,AL}, ftxui::Color::Blue},
        {{0,0,-AL}, ftxui::Color::Blue}
    };

    Vector2 pO = project({0,0,0});
    for (auto& [pt, col] : worldAxes){
        Vector2 p = project(pt);
        canvas.DrawPointLine(pO.x+cx, pO.y+cy, p.x+cx, p.y+cy, col);
    }

    // Local Axis Guides
    if (mode == appMode::EDIT && selectedVertex != -1 && inAxisSession){

        Vector3 v = mesh.vertices[selectedVertex];
        const float H = 1.5f;
        
        struct Guide { Vector3 a, b; ftxui::Color col; LastMoved id; };
        
        Guide guides[] = {
            {{v.x+H, v.y, v.z}, {v.x-H, v.y, v.z}, ftxui::Color::Red,   LastMoved::X},
            {{v.x, v.y+H, v.z}, {v.x, v.y-H, v.z}, ftxui::Color::Green, LastMoved::Y},
            {{v.x, v.y, v.z+H}, {v.x, v.y, v.z-H}, ftxui::Color::Blue,  LastMoved::Z}
        };

        for (auto& g : guides){

            if (lastMoved != LastMoved::NONE && lastMoved != g.id) continue;

            Vector2 p1 = project(g.a), p2 = project(g.b);

            canvas.DrawPointLine(p1.x+cx, p1.y+cy, p2.x+cx, p2.y+cy, g.col);
        }
    }

    // Mesh Edges
    for (auto& [i1, i2] : mesh.edges){

        Vector3 r1 = rotated(mesh.vertices[i1]);
        Vector3 r2 = rotated(mesh.vertices[i2]);
        
        if (r1.z > NEAR && r2.z > NEAR){

            Vector2 p1 = ProjectToScreen(r1, cam.zoom, ZOFF);
            Vector2 p2 = ProjectToScreen(r2, cam.zoom, ZOFF);
            
            canvas.DrawPointLine(p1.x+cx, p1.y+cy, p2.x+cx, p2.y+cy, ftxui::Color::White);
        }
    }

    // Vertices
    for (int i = 0; i < (int)mesh.vertices.size(); ++i){

        Vector3 r = rotated(mesh.vertices[i]);
        if (r.z < NEAR) continue;
        Vector2 p = ProjectToScreen(r, cam.zoom, ZOFF);

        if (mode == appMode::EDIT && i == selectedVertex){

            canvas.DrawPointCircle(p.x + cx, p.y + cy, 4, ftxui::Color::Yellow);
            canvas.DrawPoint(p.x + cx, p.y + cy, true, ftxui::Color::Red);
        }
        
        else{
            canvas.DrawPoint(p.x + cx, p.y + cy, true, ftxui::Color::Orange1);
        }
    }

    return ftxui::canvas(std::move(canvas)) | ftxui::flex;
}