cmake_minimum_required(VERSION 3.20)
project(tictactoe)
project(test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(tictactoe Game.cpp
        main.cpp
        Game.h)

add_executable(test test.cpp)
