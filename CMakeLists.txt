cmake_minimum_required(VERSION 3.16)
project(myGTP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-g)

file(GLOB_RECURSE sources      src/*.cpp)
file(GLOB_RECURSE headers      src/*.h)
file(GLOB_RECURSE sources_test tests/*.cpp)

include_directories(src tests)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(src)
add_subdirectory(tests) 