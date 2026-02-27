cmake_minimum_required(VERSION 3.31)
project(TextRPG C)

set(CMAKE_C_STANDARD 11)

include_directories(deps)
add_executable(TextRPG main.c
        ui.c
        ui.h
        krono.h
        game.c
        game.h
        menu.c
        menu.h
        deps/cJSON.c
        deps/cJSON.h
)
target_compile_options(TextRPG PRIVATE -Wall -Wextra -O0 -save-temps)
target_link_libraries(TextRPG PRIVATE curses panel)