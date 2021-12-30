cmake_minimum_required(VERSION 3.20)
project(tictactoe)

set(CMAKE_CXX_STANDARD 14)

add_executable(tictactoe Game.cpp
        main.cpp
        Game.h)
