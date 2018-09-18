cmake_minimum_required(VERSION 3.11)
project(SDL_ttf)

add_library(SDL2_ttf SDL_ttf.c SDL_ttf.h)
target_link_libraries(SDL2_ttf freetype)