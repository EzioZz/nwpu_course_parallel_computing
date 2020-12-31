cmake_minimum_required(VERSION 3.16)
project(N-queen)

# MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER /usr/local/bin/g++-8)
set(CMAKE_C_COMPILER /usr/local/bin/gcc-8)

FIND_PACKAGE(OpenMP REQUIRED)
IF(OPENMP_FOUND)
    message("OPENMP FOUND")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    # SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
ENDIF()

# add_executable(${PROJECT_NAME}  src/main.cpp src/n-queen.h src/n-queen.cpp src/stopwatch.cpp src/stopwatch.h)
add_executable(${PROJECT_NAME} src/MPI_N_queen.cpp)
