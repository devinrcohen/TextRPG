cmake_minimum_required(VERSION 4.1)
project(TextRPG C)

set(CMAKE_C_STANDARD 23)

add_executable(TextRPG main.c
        ui.c
        ui.h
        krono.h
        game.c
        game.h)
target_compile_options(TextRPG PRIVATE -Wall -Wextra -O3)
target_link_libraries(TextRPG PRIVATE curses panel)