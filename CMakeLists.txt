cmake_minimum_required(VERSION 3.21)

project(blendah)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Auto-discover all source files in the src/ directory
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.c")

add_executable(blendah ${SOURCES})

# --- PAIN Auto-Linker Hook ---
include(.pain_deps.cmake OPTIONAL)
