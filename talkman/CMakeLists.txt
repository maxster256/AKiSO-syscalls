cmake_minimum_required(VERSION 3.9)
project(talkman)

set(CMAKE_CXX_STANDARD 11)

add_executable(talkman
        client.c
        utilities.h
        server.c)
