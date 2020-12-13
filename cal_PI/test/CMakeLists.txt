# 查找 GTest 库
find_package(GTest REQUIRED)
# GTest 的头文件
include_directories(${GTEST_INCLUDE_DIRS})

# add_executable 要放在 target_link_libraries 前面
add_executable(unit_test Test_cal_PI.cpp)

# 链接测试库
target_link_libraries( unit_test
        ${GTEST_BOTH_LIBRARIES}
        pthread )


# 添加到测试
gtest_discover_tests(unit_test)