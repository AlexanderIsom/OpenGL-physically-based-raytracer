cmake_minimum_required(VERSION 2.6)
project(PBR)

include_directories(src)

add_library(PBR
	src/Eng/camera.h
	src/Eng/camera.cpp
)

target_link_libraries(PBR SDL2 opengl32 glew32)


add_executable(core
	src/Core/Main.cpp
)

target_link_libraries(core PBR)

