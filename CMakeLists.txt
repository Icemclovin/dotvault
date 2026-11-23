cmake_minimum_required(VERSION 3.25)
project(dotvault CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dotvault main.cpp)
target_compile_options(dotvault PRIVATE -Wall -Wextra -Wpedantic)
