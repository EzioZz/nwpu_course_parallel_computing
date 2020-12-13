cmake_minimum_required(VERSION 3.16)
project(cal_PI)

# MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# GMP
find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIR})


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER MPICXX)
set(CMAKE_C_COMPILER MPIC)

# set(CMAKE_CXX_COMPILER g++)
# set(CMAKE_C_COMPILER gcc)

add_executable(${PROJECT_NAME} src/cal_PI.cpp)

target_link_libraries(${PROJECT_NAME} ${GMP_LIBRARY})

add_subdirectory(test)