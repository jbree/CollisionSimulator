cmake_minimum_required(VERSION 3.10)

project(collision_sim)
set(CMAKE_CXX_STANDARD 11)
enable_testing()

set(SOURCES
    src/Packet.cc
    src/Station.cc
)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        include
)

add_subdirectory(test)