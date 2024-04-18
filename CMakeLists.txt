# 设置最低cmake版本,3.6-Morden CMake
cmake_minimum_required(VERSION 3.6)

# ---------------------------------------------------------------------------------------
# Start EasyCache project
# ---------------------------------------------------------------------------------------
# 项目名称-EasyCache
project(EasyCache VERSION 1.0.0
  DESCRIPTION "A cache library/server for C++"
  LANGUAGES CXX C)

# 导入GunDirs
include(GNUInstallDirs)

# ---------------------------------------------------------------------------------------
# Compiler config
# ---------------------------------------------------------------------------------------
# 指定cpp11标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# 生成compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 判断编译类型
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
  add_definitions("-DECACHE_DEBUG")
endif()

# ---------------------------------------------------------------------------------------
# config thrid_party
# ---------------------------------------------------------------------------------------
# 编译gtest库
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/gtest)
enable_testing()

# 编译spdlog库
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/spdlog)

# ---------------------------------------------------------------------------------------
# EasyCache编译
# ---------------------------------------------------------------------------------------



# ---------------------------------------------------------------------------------------
# EasyCache库单元测试
# ---------------------------------------------------------------------------------------
file(GLOB_RECURSE EasyCache_UNITTEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")
add_executable(EasyCacheUnitTest ${EasyCache_UNITTEST_SOURCES})
target_include_directories(EasyCacheUnitTest PUBLIC ${CMAKE_SOURCE_DIR}/easycache)
target_link_libraries(EasyCacheUnitTest ${EasyCache_LIB_NAME})
target_link_libraries(EasyCacheUnitTest gtest gtest_main)

# ---------------------------------------------------------------------------------------
# EasyCache使用案例
# ---------------------------------------------------------------------------------------
option(BUILD_WITH_EXAMPLES "是否编译examples" OFF)
if (BUILD_WITH_EXAMPLES)
  add_subdirectory(examples)
endif()
