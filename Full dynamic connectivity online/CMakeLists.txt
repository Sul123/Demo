cmake_minimum_required(VERSION 3.8)
project(Dzen)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
set(SOURCE_FILES main.cpp et_forest.h dynamic_graph.h dynamic_graph.cpp et_forest.cpp)
add_executable(Dzen ${SOURCE_FILES})