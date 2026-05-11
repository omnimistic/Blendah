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
#include <sstream>

#include "include/types.h"
#include "include/mesh.h"
#include "include/camera.h"
#include "include/renderer.h"
#include "include/controls.h"
#include "include/obj_io.h"

// Stage Management
enum class AppStage {BOOT, MENU, IMPORT, SCENE};

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
R"( | $$    $$| $$    \| $$    \| $$  \$$$| $$    $$| $$  | $$| $$  | $$ )",      
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

    std::string importPath = "";
    std::string importError = "";

    // Blendah command Line Variables
    bool isCommandMode = false;
    std::string commandBuffer = "";
    std::string commandMessage = "";

    // Helper to strip quotes and whitespace from copied paths
    auto CleanPath = [](std::string path){
        
        if (path.empty()) return path;
        
        size_t first = path.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return std::string("");
        size_t last = path.find_last_not_of(" \t\r\n");
        path = path.substr(first, (last - first + 1));

        if (path.size() >= 2 && 
           ((path.front() == '"' && path.back() == '"') || 
            (path.front() == '\'' && path.back() == '\''))){
            path = path.substr(1, path.size() - 2);
        }

        return path;
    };

    // Blendah Command Line Parser
    auto ProcessCommand = [&](std::string cmd){
        if (cmd.empty()) return;

        std::istringstream iss(cmd);
        std::string action;
        iss >> action;

        // Converting to lowercase for case-insensitivity
        std::transform(action.begin(), action.end(), action.begin(), ::tolower);

        if (action == "export" || action == "w"){
            std::string filepath;
            std::getline(iss >> std::ws, filepath); // Get the rest of the line
            
            filepath = CleanPath(filepath);

            // Print Help Menu
            if (filepath == "-h" || filepath == "--help") {
                commandMessage = "Usage: :export [path] | Example: :export my_model.obj";
                return;
            }

            // Strip dash if user typed ":export -filename.obj"
            if (!filepath.empty() && filepath[0] == '-'){
                filepath = filepath.substr(1);
            }

            if (filepath.empty()){
                filepath = "blendah_export.obj"; // Fallback default
            }

            ExportOBJ(mesh, filepath);
            commandMessage = "\"" + filepath + "\" written.";
        } 

        else if (action == "q" || action == "quit"){
            currentStage = AppStage::MENU;
            commandMessage = "";
        }

        else{
            commandMessage = "E492: Not an editor command: " + action;
        }
    };

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
                logo_lines.push_back(ftxui::text(render_line) | ftxui::bold | ftxui::color(ftxui::Color::Orange1) | ftxui::center);
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

        // Import obj
        if (currentStage == AppStage::IMPORT){

            auto dialog_box = ftxui::vbox({
                
                ftxui::text("--- IMPORT OBJ ---") | ftxui::bold | ftxui::center,
                ftxui::separator(),
                ftxui::text("Enter file path:"),
                ftxui::text("> " + importPath + "_") | ftxui::color(ftxui::Color::CyanLight),
                ftxui::text(importError) | ftxui::color(ftxui::Color::Red),
                ftxui::separator(),
                ftxui::text("[ENTER] Load    [ESC] Cancel") | ftxui::dim | ftxui::center
                
            })

            | ftxui::border | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 60) | ftxui::center;

            return ftxui::vbox({
                ftxui::filler(),
                dialog_box,
                ftxui::filler()
            });
        }

        // HUD Mode Text
        std::string hud = (state.mode == appMode::VIEW) 
            ? "VIEW | WASD=Rotate +/-=Zoom r=Reset TAB=Edit :cmd ESC=Menu" : "EDIT | WASD/Q/E=Select Arrows=FreeMov ^A/^D=MoveX ^W/^S=MoveY ^Q/^E=MoveZ TAB=View :cmd ESC=Menu";

        // A single flat elements list to prevent nested flex elements from pushing content off-screen
        ftxui::Elements scene_elements = {
            ftxui::text(hud) | ftxui::bold,
            ftxui::separator(),
            scene | ftxui::flex
        };

        // Inject blendah command bar if active
        if (isCommandMode){
            scene_elements.push_back(ftxui::text(":" + commandBuffer + "_") | ftxui::bold | ftxui::color(ftxui::Color::Yellow));
        }
        else if (!commandMessage.empty()){
            scene_elements.push_back(ftxui::text(commandMessage) | ftxui::dim);
        }

        return ftxui::vbox(std::move(scene_elements));
    });

    auto component = ftxui::CatchEvent(renderer, [&](ftxui::Event ev){

        if (currentStage == AppStage::BOOT) return true;

        // Menu controls
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
                if (menuSelector == 0){
                    mesh = createCube(); 
                    state = AppState();
                    currentStage = AppStage::SCENE;
                    commandMessage = "";
                }
                if (menuSelector == 1){ // Open Import Dialog
                    importPath = "";
                    importError = "";
                    currentStage = AppStage::IMPORT;
                }
                if (menuSelector == 3) screen.Exit();
                return true;
            }
            return true;
        }

        // Import Dialog Inputs
        if (currentStage == AppStage::IMPORT){
            
            if (ev == ftxui::Event::Escape){
                currentStage = AppStage::MENU;
                return true;
            }
            
            if (ev == ftxui::Event::Backspace){
                if (!importPath.empty()) importPath.pop_back();
                importError = ""; // Clear error when user starts typing again
                return true;
            }
            
            if (ev == ftxui::Event::Return){
                std::string cleanPath = CleanPath(importPath);

                if (cleanPath.empty()){
                    importError = "Error: Path cannot be empty.";
                    return true;
                }
                
                Mesh imported = LoadOBJ(cleanPath);
                
                if (imported.vertices.empty()){
                    importError = "Error: Could not load file or file is empty.";
                }
                else{
                    mesh = imported;
                    state = AppState(); // Reset camera so it doesn't break
                    currentStage = AppStage::SCENE;
                    commandMessage = "";
                }
                return true;
            }

            if (ev.is_character()){
                importPath += ev.character();
                importError = "";
                return true;
            }

            return true; // Block other inputs from leaking into the scene
        }

        // Scene Inputs and command line intercept
        if (currentStage == AppStage::SCENE){

            if (isCommandMode){

                if (ev == ftxui::Event::Escape){
                    isCommandMode = false;
                    return true;
                }

                if (ev == ftxui::Event::Backspace){
                    if (!commandBuffer.empty()) commandBuffer.pop_back();
                    else isCommandMode = false; // Close command mode if backspacing on empty
                    return true;
                }

                if (ev == ftxui::Event::Return){
                    ProcessCommand(commandBuffer);
                    isCommandMode = false;
                    return true;
                }

                if (ev.is_character()){
                    commandBuffer += ev.character();
                    return true;
                }

                return true; // Block scene controls while typing command
            }

            else{

                if (ev == ftxui::Event::Character(':')){

                    isCommandMode = true;
                    commandBuffer = "";
                    commandMessage = "";
                    return true;
                }

                if (ev == ftxui::Event::Escape){

                    currentStage = AppStage::MENU;
                    commandMessage = "";
                    return true;
                }
            }
        }

        HandleInput(ev, state, mesh);
        
        // Clear message on next input if not in command mode
        if (currentStage == AppStage::SCENE && !isCommandMode && (ev.is_character() || ev.is_mouse())){
            commandMessage = ""; 
        }

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
