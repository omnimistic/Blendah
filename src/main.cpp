#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>

#include <string>
#include <cstdio>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

#include "include/types.h"
#include "include/mesh.h"
#include "include/camera.h"
#include "include/renderer.h"
#include "include/controls.h"

// Stage Management
enum class AppStage {BOOT, MENU, SCENE};

// ASCII Constant for bootup
const std::string GRADIENT = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^'.";
const int GRADIENT_LENGTH = GRADIENT.length();

// Bootup logo
std::vector<std::string> bootLogo = {
    "BBBB   L      EEEEE  N   N  DDDD    A    H   H",
    "B   B  L      E      NN  N  D   D  A A   H   H",
    "BBBB   L      EEE    N N N  D   D AAAAA  HHHHH",
    "B   B  L      E      N  NN  D   D A   A  H   H",
    "BBBB   LLLLL  EEEEE  N   N  DDDD  A   A  H   H"
};

// Main Menu animation
std::vector<std::vector<std::string>> menuLogos = {
{
R"(  /$$$$$$$  /$$       /$$$$$$$$ /$$   /$$ /$$$$$$$   /$$$$$$  /$$   /$$ )",    
R"( | $$__  $$| $$      | $$_____/| $$$ | $$| $$__  $$ /$$__  $$| $$  | $$ )",    
R"( | $$  \ $$| $$      | $$      | $$$$| $$| $$  \ $$| $$  \ $$| $$  | $$ )",    
R"( | $$$$$$$ | $$      | $$$$$   | $$ $$ $$| $$  | $$| $$$$$$$$| $$$$$$$$ )",    
R"( | $$__  $$| $$      | $$__/   | $$  $$$$| $$  | $$| $$__  $$| $$__  $$ )",    
R"( | $$  \ $$| $$      | $$      | $$\  $$$| $$  | $$| $$  | $$| $$  | $$ )",    
R"( | $$$$$$$/| $$$$$$$$| $$$$$$$$| $$ \  $$| $$$$$$$/| $$  | $$| $$  | $$ )",    
R"( |_______/ |________/|________/|__/  \__/|_______/ |__/  |__/|__/  |__/ )",    
R"(                                                                        )"  
},
{
R"( $$$$$$$\  $$\       $$$$$$$$\ $$\   $$\ $$$$$$$\   $$$$$$\  $$\   $$\  )",    
R"( $$  __$$\ $$ |      $$  _____|$$$\  $$ |$$  __$$\ $$  __$$\ $$ |  $$ | )",    
R"( $$ |  $$ |$$ |      $$ |      $$$$\ $$ |$$ |  $$ |$$ /  $$ |$$ |  $$ | )",    
R"( $$$$$$$\ |$$ |      $$$$$\    $$ $$\$$ |$$ |  $$ |$$$$$$$$ |$$$$$$$$ | )",    
R"( $$  __$$\ $$ |      $$  __|   $$ \$$$$ |$$ |  $$ |$$  __$$ |$$  __$$ | )",    
R"( $$ |  $$ |$$ |      $$ |      $$ |\$$$ |$$ |  $$ |$$ |  $$ |$$ |  $$ | )",    
R"( $$$$$$$  |$$$$$$$$\ $$$$$$$$\ $$ | \$$ |$$$$$$$  |$$ |  $$ |$$ |  $$ | )",    
R"( \_______/ \________|\________|\__|  \__|\_______/ \__|  \__|\__|  \__| )",   
R"(                                                                        )"
},
{
R"(  _______   __        ________  __    __  _______    ______   __    __  )",      
R"( /       \ /  |      /        |/  \  /  |/       \  /      \ /  |  /  | )",      
R"( $$$$$$$  |$$ |      $$$$$$$$/ $$  \ $$ |$$$$$$$  |/$$$$$$  |$$ |  $$ | )",      
R"( $$ |__$$ |$$ |      $$ |__    $$$  \$$ |$$ |  $$ |$$ |__$$ |$$ |__$$ | )",      
R"( $$    $$< $$ |      $$    |   $$$$  $$ |$$ |  $$ |$$    $$ |$$    $$ | )",      
R"( $$$$$$$  |$$ |      $$$$$/    $$ $$ $$ |$$ |  $$ |$$$$$$$$ |$$$$$$$$ | )",      
R"( $$ |__$$ |$$ |_____ $$ |_____ $$ |$$$$ |$$ |__$$ |$$ |  $$ |$$ |  $$ | )",      
R"( $$    $$/ $$       |$$       |$$ | $$$ |$$    $$/ $$ |  $$ |$$ |  $$ | )",      
R"( $$$$$$$/  $$$$$$$$/ $$$$$$$$/ $$/   $$/ $$$$$$$/  $$/   $$/ $$/   $$/  )",
},
{
R"(  _______   __        ________  __    __  _______    ______   __    __  )",    
R"( |       \ |  \      |        \|  \  |  \|       \  /      \ |  \  |  \ )",
R"( | $$$$$$$\| $$      | $$$$$$$$| $$\ | $$| $$$$$$$\|  $$$$$$\| $$  | $$ )",    
R"( | $$__/ $$| $$      | $$__    | $$$\| $$| $$  | $$| $$__| $$| $$__| $$ )",
R"( | $$    $$| $$      | $$  \   | $$$$\ $$| $$  | $$| $$    $$| $$    $$ )",      
R"( | $$$$$$$\| $$      | $$$$$   | $$\$$ $$| $$  | $$| $$$$$$$$| $$$$$$$$ )",      
R"( | $$__/ $$| $$_____ | $$_____ | $$ \$$$$| $$__/ $$| $$  | $$| $$  | $$ )",      
R"( | $$    $$| $$     \| $$     \| $$  \$$$| $$    $$| $$  | $$| $$  | $$ )",      
R"(  \$$$$$$$  \$$$$$$$$ \$$$$$$$$ \$$   \$$ \$$$$$$$  \$$   \$$ \$$   \$$ )",
}
};

std::vector<std::string> menuOptions = {
    "create blank",
    "import obj",
    "help",
    "exit"
};


int main(){

    Mesh mesh = createCube();

    AppState state;

    AppStage currentStage = AppStage::BOOT;
    int menuSelector = 0;

    float bootTimer = 0.0f;
    const float BOOT_DURATION = 2.0f;

    auto screen = ftxui::ScreenInteractive::Fullscreen();

    auto renderer = ftxui::Renderer([&]{
        
        // Bootup
        if (currentStage == AppStage::BOOT){

            bootTimer += 0.03f;
            float progress = bootTimer / BOOT_DURATION;
            
            if (progress >= 1.0f) currentStage = AppStage::MENU;

            int gradient_idx = static_cast<int>((1.0f - progress) * (GRADIENT_LENGTH - 1));
            char fill_char = GRADIENT[std::clamp(gradient_idx, 0, GRADIENT_LENGTH - 1)];

            ftxui::Elements logo_lines;
            for (const auto& line : bootLogo){

                std::string render_line = "";
                for (char c : line) render_line += (c == ' ') ? " " : std::string(1, fill_char);
                logo_lines.push_back(ftxui::text(render_line) | ftxui::bold | ftxui::center);
            }

            return ftxui::vbox({ftxui::filler(), ftxui::vbox(std::move(logo_lines)), ftxui::filler()});
        }

        // Main Menu
        if (currentStage == AppStage::MENU){
            
            static int frameCounter = 0;
            static int logoIndex = 0;

            // Timer logic to swap the frames every 0.2s (approx 7 frames at 30ms refresh)
            frameCounter++;
            if (frameCounter >= 7){
                frameCounter = 0;
                logoIndex = (logoIndex + 1) % 4;
            }

            ftxui::Elements logo_static;
            const auto& currentLogo = menuLogos[logoIndex];
            for (const auto& line : currentLogo){
                logo_static.push_back(ftxui::text(line) | ftxui::bold | ftxui::color(ftxui::Color::White) | ftxui::center);
            }

            ftxui::Elements menu_list;
            for (int i = 0; i < (int)menuOptions.size(); ++i){
                bool is_selected = (i == menuSelector);
                std::string label = (is_selected ? "> " : "  ") + menuOptions[i];
                auto option_element = ftxui::text(label) | ftxui::center;
                if (is_selected) option_element = option_element | ftxui::inverted | ftxui::bold;
                menu_list.push_back(option_element);
            }

            return ftxui::vbox({
                ftxui::filler(),
                ftxui::vbox(std::move(logo_static)),
                ftxui::filler(),
                ftxui::vbox(std::move(menu_list)),
                ftxui::filler()
            });
        }

        ftxui::Element scene = RenderScene(mesh, state.camera, state.mode, state.selectedVertex, state.inAxisSession, state.lastMoved);

        // HUD Mode Text
        std::string hud = (state.mode == appMode::VIEW) ? "VIEW | WASD=Rotate +/-=Zoom r=Reset TAB=Edit" : "EDIT | WASD/Q/E=Select Arrows=FreeMov ^A/^D=MoveX ^W/^S=MoveY ^Q/^E=MoveZ TAB=View";

        return ftxui::vbox({
            
            ftxui::text(hud) | ftxui::bold,
            ftxui::separator(),
            scene | ftxui::flex
        });
    });

    auto component = ftxui::CatchEvent(renderer, [&](ftxui::Event ev){
        
        if (currentStage == AppStage::BOOT) return true;

        if (currentStage == AppStage::MENU){
            
            if (ev == ftxui::Event::ArrowUp){
                menuSelector = (menuSelector - 1 + (int)menuOptions.size()) % (int)menuOptions.size();
                return true;
            }
            if (ev == ftxui::Event::ArrowDown){
                menuSelector = (menuSelector + 1) % (int)menuOptions.size();
                return true;
            }
            if (ev == ftxui::Event::Return || ev == ftxui::Event::Character(' ')){

                if (menuSelector == 0) currentStage = AppStage::SCENE; // Create blank

                //TODO: add import and help here
                
                if (menuSelector == 3) screen.Exit(); // Exit
                
                return true;
            }
            return true;
        }

        HandleInput(ev, state, mesh);
        return true;
    });

    // Refresh thread to drive animations (for Bootup and Menu Logo)
    std::thread refresh_thread([&]{
        while (true){
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            screen.Post(ftxui::Event::Custom);
        }
    });

    refresh_thread.detach();

    screen.Loop(component);
    return 0;
}
