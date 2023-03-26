cmake_minimum_required(VERSION 3.22)

project(ebw2.0 VERSION 0.1)
add_executable(ebw sources/main.cpp sources/lexer.cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-g -Wall -pedantic)