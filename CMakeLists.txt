cmake_minimum_required(VERSION 3.26)
project(Laboratorio_4)

set(CMAKE_CXX_STANDARD 20)

set(SDL2_INCLUDE_DIR C:/Users/marka/Coding/Recursos/Librerías/SDL2/include)
set(SDL2_LIB_DIR C:/Users/marka/Coding/Recursos/Librerías/SDL2/lib/x64)

find_package(glm CONFIG REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(Laboratorio_4 src/main.cpp)

target_link_libraries(Laboratorio_4 SDL2main SDL2 glm::glm)
