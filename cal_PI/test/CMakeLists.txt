# GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# GMP
find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIR})

# add_executable 要放在 target_link_libraries 前面
add_executable(unit_test Test_cal_PI.cpp ../src/cal_PI.cpp ../src/cal_PI.h)

# 链接测试库
target_link_libraries( unit_test
        ${GTEST_BOTH_LIBRARIES}
        pthread 
        ${GMP_LIBRARY} )


# 添加到测试
gtest_discover_tests(unit_test)