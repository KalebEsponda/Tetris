cmake_minimum_required(VERSION 3.20)
project(Tetris LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW ON
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

add_executable(Tetris src/main.cpp src/game.cpp src/menu.cpp src/save.cpp src/board.cpp src/piece.cpp src/style.cpp src/effects.cpp src/audio.cpp)
target_link_libraries(Tetris PRIVATE raylib)

# Copia la carpeta assets junto al ejecutable en cada compilación
add_custom_target(copiar_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:Tetris>/assets)
add_dependencies(Tetris copiar_assets)