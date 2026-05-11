#include "../include/controls.h"
#include "../include/math.h"
#include <ftxui/component/event.hpp>
#include <algorithm>

// Ctrl+key constants
static const char CTRL_A = 0x01;
static const char CTRL_D = 0x04;
static const char CTRL_E = 0x05;
static const char CTRL_Q = 0x11;
static const char CTRL_S = 0x13;
static const char CTRL_W = 0x17;


void HandleInput(ftxui::Event ev, AppState& state, Mesh& mesh){

    // TAB: Toggle between VIEW and EDIT mode
    if (ev == ftxui::Event::Tab){
        if (state.mode == appMode::VIEW){

            state.mode = appMode::EDIT;
            state.inAxisSession = false;
            state.lastMoved = LastMoved::NONE;

            if (state.selectedVertex == -1){
                float best = 1e10f;
                
                for (int i = 0; i < (int) mesh.vertices.size(); ++i){

                    Vector3 v = RotateY(mesh.vertices[i], state.camera.rotY);
                    
                    v = RotateX(v, state.camera.rotX);
                    
                    Vector2 p = ProjectToScreen(v, state.camera.zoom, 4.0f);
                    
                    float d = p.x * p.x + p.y * p.y;
                    
                    if (d < best){
                        best = d;
                        state.selectedVertex = i;
                    }
                }
            }

            state.vertexEntryPos = mesh.vertices[state.selectedVertex];
            state.hasEntryPos = true;
        }

        else{
            state.mode = appMode::VIEW;
            state.inAxisSession = false;
            state.lastMoved = LastMoved::NONE;
        }

        return;
    }

    // Reset camera
    if (ev == ftxui::Event::Character('r')){
        state.camera.rotX = 0.0f;
        state.camera.rotY = 0.0f;
        state.camera.zoom = 20.0f;
        return;
    }

    // VIEW Mode Controls
    if (state.mode == appMode::VIEW){

        if (ev == ftxui::Event::Character('w')){state.camera.rotX += 0.1f; return;}
        if (ev == ftxui::Event::Character('s')){state.camera.rotX -= 0.1f; return;}
        if (ev == ftxui::Event::Character('a')){state.camera.rotY -= 0.1f; return;}
        if (ev == ftxui::Event::Character('d')){state.camera.rotY += 0.1f; return;}

        if (ev == ftxui::Event::Character('+') || ev == ftxui::Event::Character('=')){
            state.camera.zoom += 2.0f;
            return;
        }

        if (ev == ftxui::Event::Character('-') || ev == ftxui::Event::Character('_')){
            state.camera.zoom = std::max(1.0f, state.camera.zoom - 2.0f);
            return;
        }

        return;
    }

    // EDIT Mode Controls
    if (state.selectedVertex == -1) return;

    auto& vtx = mesh.vertices[state.selectedVertex];
    const float step = 0.1f;

    auto endAxisSession = [&](){
        state.inAxisSession = false;
        state.lastMoved = LastMoved::NONE;
    };

    // Ctrl + Axis Constrained Movement
    if (ev.input().size() == 1){

        char c = ev.input()[0];

        if (c == CTRL_D){
            state.inAxisSession = true;
            vtx.x += step;
            state.lastMoved = LastMoved::X;
            return;
        }
        if (c == CTRL_A){
            state.inAxisSession = true;
            vtx.x -= step;
            state.lastMoved = LastMoved::X;
            return;
        }
        if (c == CTRL_S){
            state.inAxisSession = true;
            vtx.y += step;
            state.lastMoved = LastMoved::Y;
            return;
        }
        if (c == CTRL_W){
            state.inAxisSession = true;
            vtx.y -= step;
            state.lastMoved = LastMoved::Y;
            return;
        }
        if (c == CTRL_E){
            state.inAxisSession = true;
            vtx.z += step;
            state.lastMoved = LastMoved::Z;
            return;
        }
        if (c == CTRL_Q){
            state.inAxisSession = true;
            vtx.z -= step;
            state.lastMoved = LastMoved::Z;
            return;
        }
    }

    // Normal WASDQE to jumping between vertices
    auto jumpTo = [&](float dirX, float dirY, float dirDepth){

        Vector3 vc = RotateY(mesh.vertices[state.selectedVertex], state.camera.rotY);
        vc = RotateX(vc, state.camera.rotX);
        Vector2 pc = ProjectToScreen(vc, state.camera.zoom, 4.0f);
        float dc = vc.z;

        int best = -1;
        float bestScore = 1e10f;

        for (int i = 0; i < (int)mesh.vertices.size(); ++i){

            if (i == state.selectedVertex) continue;
            
            Vector3 vn = RotateY(mesh.vertices[i], state.camera.rotY);
            vn = RotateX(vn, state.camera.rotX);

            if (vn.z < -3.9f) continue;

            Vector2 pn = ProjectToScreen(vn, state.camera.zoom, 4.0f);
            float sdx = pn.x - pc.x;
            float sdy = pn.y - pc.y;
            float score;

            if (dirDepth != 0.0f){
                float dd = (vn.z - dc) * dirDepth;
                if (dd <= 0.01f) continue;
                score = (sdx*sdx + sdy*sdy) / (dd*dd);
            }
            else{
                float dot = sdx*dirX + sdy*dirY;
                if (dot <= 0.1f) continue;
                score = (sdx*sdx + sdy*sdy) / (dot*dot);
            }

            if (score < bestScore){
                bestScore = score;
                best = i;
            }
        }

        if (best != -1){
            state.selectedVertex = best;
            state.vertexEntryPos = mesh.vertices[best];
            endAxisSession();
        }
    };

    if (ev == ftxui::Event::Character('w')){jumpTo(0, -1, 0); return;}
    if (ev == ftxui::Event::Character('s')){jumpTo(0,  1, 0); return;}
    if (ev == ftxui::Event::Character('a')){jumpTo(-1, 0, 0); return;}
    if (ev == ftxui::Event::Character('d')){jumpTo(1,  0, 0); return;}
    if (ev == ftxui::Event::Character('q')){jumpTo(0, 0, -1); return;}
    if (ev == ftxui::Event::Character('e')){jumpTo(0, 0,  1); return;}

    // Arrow keys for free movement
    float dmx = 0, dmy = 0;
    bool doMove = false;

    if (ev == ftxui::Event::ArrowUp)    {dmy = -0.1f; doMove = true;}
    if (ev == ftxui::Event::ArrowDown)  {dmy =  0.1f; doMove = true;}
    if (ev == ftxui::Event::ArrowLeft)  {dmx = -0.1f; doMove = true;}
    if (ev == ftxui::Event::ArrowRight) {dmx =  0.1f; doMove = true;}

    if (doMove){
        
        endAxisSession();
        Vector3 delta(dmx, dmy, 0);
        delta = RotateX(delta, -state.camera.rotX);
        delta = RotateY(delta, -state.camera.rotY);

        vtx.x += delta.x;
        vtx.y += delta.y;
        vtx.z += delta.z;
        return;
    }
}