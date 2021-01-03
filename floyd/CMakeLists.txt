cmake_minimum_required(VERSION 3.16)


project(floyd)

add_definitions(-std=c++11)

# MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER MPICXX)
set(CMAKE_C_COMPILER MPIC)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(
        -DDEBUG
    )
endif ()

# add_definitions(-DDEVLOG)

add_executable(${PROJECT_NAME} src/main.cpp src/stopwatch.cpp src/stopwatch.h src/floyd.h src/floyd.cpp)

#测试用程序
add_executable(test test/main.cpp src/floyd.h src/floyd.cpp)


target_link_libraries(${PROJECT_NAME} )

