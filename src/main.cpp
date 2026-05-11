#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>

#include <string>
#include <cstdio>

#include "include/types.h"
#include "include/mesh.h"
#include "include/camera.h"
#include "include/renderer.h"
#include "include/controls.h"

int main(){

    Mesh mesh = createCube();
    
    AppState state;

    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

    auto renderer = ftxui::Renderer([&]{
        ftxui::Element scene = RenderScene(mesh, state.camera, state.mode, state.selectedVertex, state.inAxisSession, state.lastMoved);

        // HUD Mode Text
        std::string hud = (state.mode == appMode::VIEW) ? "VIEW | WASD=Rotate +/-=Zoom r=Reset TAB=Edit" : "EDIT | WASD/Q/E=Select Arrows=FreeMov ^A/^D=MoveX ^W/^S=MoveY ^Q/^E=MoveZ TAB=View";

        // Coordinate and Delta Display
        std::string coords;
        if (state.mode == appMode::EDIT && state.selectedVertex != -1 && state.hasEntryPos){

            Vector3& cv = mesh.vertices[state.selectedVertex];
            
            float dx = cv.x - state.vertexEntryPos.x;
            float dy = cv.y - state.vertexEntryPos.y;
            float dz = cv.z - state.vertexEntryPos.z;

            auto format = [](float v) -> std::string{
                char buf[16];
                std::snprintf(buf, sizeof(buf), "%+.2f", v);
                return buf;
            };

            std::string axisLabel;

            if (state.inAxisSession) {
                
                if (state.lastMoved == LastMoved::X) axisLabel = " [AXIS: X]";
                else if (state.lastMoved == LastMoved::Y) axisLabel = " [AXIS: Y]";
                else if (state.lastMoved == LastMoved::Z) axisLabel = " [AXIS: Z]";
            }

            coords = "Pos: X=" + format(cv.x) +
                     " Y=" + format(cv.y) +
                     " Z=" + format(cv.z) + "  " +
                     " Delta: dX=" + format(dx) +
                     " dY=" + format(dy) +
                     " dZ=" + format(dz) +
                     axisLabel;
        }

        return ftxui::vbox({
            
            ftxui::text(hud) | ftxui::bold,
            ftxui::text(coords),
            std::move(scene)
        });
    });

    auto component = ftxui::CatchEvent(renderer, [&](ftxui::Event ev){
        HandleInput(ev, state, mesh);
        return true;
    });

    screen.Loop(component);
    return 0;
}
