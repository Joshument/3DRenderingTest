cmake_minimum_required(VERSION 3.1.0)

project(3DRenderingTest VERSION 0.0.0)

add_executable(3DRenderingTest src/main.cpp src/glad.c src/shader.cpp src/stb_image.cpp include/camera.hpp)

target_link_directories(3DRenderingTest PUBLIC lib)
target_link_libraries(3DRenderingTest PUBLIC glfw3 X11)
target_include_directories(3DRenderingTest PUBLIC src include)

add_compile_options(3DRenderingTest PRIVATE -fsanitize=address,undefined -std=c++20)
add_link_options(3DRenderingTest PRIVATE -fsanitize=address,undefined)