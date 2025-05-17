cmake_minimum_required(VERSION 3.10)
project(OpenGL_Playground C)

set(CMAKE_C_STANDARD 11)

# Указываем путь к include и src
include_directories(include)
include_directories(libs/glfw/include)

file(GLOB SOURCES src/*.c)

# Добавляем исполняемый файл
add_executable(OpenGL_Playground ${SOURCES})

# GLFW и OpenGL
link_directories(libs/glfw/lib-mingw-w64)

target_link_libraries(OpenGL_Playground
    glfw3
    opengl32
)
