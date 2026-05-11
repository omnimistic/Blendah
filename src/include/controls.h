#pragma once

#include "types.h"
#include "camera.h"

// Forward declaration to avoid heavy include
namespace ftxui{
    class Event;
}


struct AppState{
    appMode mode = appMode::VIEW;
    int selectedVertex = -1;
    Camera camera;
    Vector3 vertexEntryPos;
    bool hasEntryPos = false;
    bool inAxisSession = false;
    LastMoved lastMoved = LastMoved::NONE;
};


void HandleInput(ftxui::Event ev, AppState& state, Mesh& mesh);