#pragma once
#include "types.h"
#include "camera.h"
#include <ftxui/dom/elements.hpp>

ftxui::Element RenderScene(
    const Mesh& mesh,
    const Camera& cam,
    appMode mode,
    int selectedVertex,
    bool inAxisSession,
    LastMoved lastMoved
);