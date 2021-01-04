cmake_minimum_required(VERSION 3.16)
project(SuperQuickSort)

# MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER MPICXX)
set(CMAKE_C_COMPILER MPIC)

# add_definitions(-D LOG)

add_executable(${PROJECT_NAME} src/main.cpp)
