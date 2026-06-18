# BLENDAH

**Blendah** is a lightweight, terminal-based 3D wireframe engine and editor. It allows you to load, view and modify 3D models (`.obj` files) directly in your terminal, complete with a Neovim-style command interface.

## How It Works

Standard 3D software (like Blender) use your graphics card to draw solid surfaces made of triangles. Blendah does things a bit differently, relying purely on your CPU and some clever maths to draw directly into the terminal:

1. **The Skeleton:** Instead of solid shapes, Blendah only cares about the wireframe. When you import an `.obj` file, it just reads the raw vertices and the lines connecting them i.e. the edges.
2. **The Math:** To rotate the model it uses basic trigonometry to spin each individual vertices around an invisible axis.
3. **The Projection:** A computer screen is flat(2D), but models are 3D. Blendah uses a math trick called "Perspective Divide." It takes the X and Y coordinates of a point(vertex) and divides them by how far away they are i.e. the Z depth. This squishes distant points closer to the center, creating the illusion of 3D depth. I learnt about this neat trick from [Tsoding](https://youtu.be/qjWkNZ0SXfo?si=iDA6cTUS5RfU2pTu).
4. **The Canvas:** Finally, it uses the **FTXUI** library to connect the dots i.e. the vertices, drawing straight lines using Braille characters right in the terminal. I wanted to make my own rendering engine but my attempts were simply inferior to what FTXUI was providing so I stuck to it.

## Features

* **Terminal Native:** Runs entirely in the terminal.
* **Wireframe Rendering:** Renders `.obj` files using high-resolution Braille characters.
* **View Mode:** Freely orbit and zoom around your 3D models.
* **Edit Mode:** Select individual vertices and move them in 3D space, with optional axis-locking.
* **The Command Line:** Press `:` to open a command bar to export files or quit the scene.

## Controls

### View Mode

* **WASD** - Rotate the camera
* **+/-** - Zoom in and out
* **r** - Reset camera
* **TAB** - Switch to Edit Mode

### Edit Mode

* **WASD / Q / E** - Spatially jump between vertices to select them
* **Arrow Keys** - Free move the selected vertex relative to your view
* **Ctrl + A/D** - Lock movement to X-Axis (Red)
* **Ctrl + W/S** - Lock movement to Y-Axis (Green)
* **Ctrl + Q/E** - Lock movement to Z-Axis (Blue)
* **TAB** - Switch back to View Mode

### Command Line

* **`:`** - Open the command bar
* **`:w [path]`** or **`:export [path]`** - Export the mesh to an `.obj` file
* **`:q`** or **`:quit`** - Quit back to the main menu

## Getting Started

Blendah is written in C++ and uses the FTXUI library for the terminal interface. You can build it using CMake and your preferred C++ compiler. I use my own project/package manager *[pain](https://github.com/omnimistic/pain)* for all my C++ projects so you might need to modify the cmake and vcpkg files unless you're also using *pain*.

---

Go to my [blog](https://omnimistic.github.io/blogs.html) to read more technical details.
