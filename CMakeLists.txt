# 设置最低cmake版本,3.6-Morden CMake
cmake_minimum_required(VERSION 3.6)

# ---------------------------------------------------------------------------------------
# Start EasyCache project
# ---------------------------------------------------------------------------------------
# 项目名称-EasyCache
project(
  EasyCache
  VERSION 0.0.1
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

# ---------------------------------------------------------------------------------------
# config thrid_party
# ---------------------------------------------------------------------------------------
find_package(GTest REQUIRED)
if(GTest_FOUND)
  message(STATUS "GTest found")
endif()

find_package(spdlog REQUIRED)
if(spdlog_FOUND)
  message(STATUS "spdlog found")
endif()

# ---------------------------------------------------------------------------------------
# EasyCache编译
# ---------------------------------------------------------------------------------------
set(LIB_NAME "easycache")
file(GLOB_RECURSE EASYCACHE_SOURCES "${CMAKE_SOURCE_DIR}/easycache/*.cpp")
add_library(${LIB_NAME} STATIC ${EASYCACHE_SOURCES})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/easycache)
target_link_libraries(${LIB_NAME} spdlog::spdlog)

# ---------------------------------------------------------------------------------------
# EasyCache编译服务端和客户端
# ---------------------------------------------------------------------------------------
option(BUILD_EXE "是否编译成可执行程序" OFF)
set(SVR_NAME "cache_svr")
set(CLI_NAME "cache_cli")
if(BUILD_EXE)
  add_executable(${SVR_NAME} cmd/cache_svr/cache_svr.cpp)
  target_link_libraries(${SVR_NAME} ${LIB_NAME})
  target_include_directories(${SVR_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
  add_executable(${CLI_NAME} cmd/cache_cli/cache_cli.cpp)
  target_link_libraries(${CLI_NAME} ${LIB_NAME})
  target_include_directories(${SVR_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
endif(BUILD_EXE)
unset(SVR_NAME)
unset(CLI_NAME)

# ---------------------------------------------------------------------------------------
# EasyCache库单元测试
# ---------------------------------------------------------------------------------------
option(BUILD_TEST "是否编译测试用例" OFF)
if(BUILD_TEST)
  file(GLOB_RECURSE EasyCache_UNITTEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")
  add_executable(EasyCacheUnitTest ${EasyCache_UNITTEST_SOURCES})
  target_include_directories(
    EasyCacheUnitTest PUBLIC ${CMAKE_SOURCE_DIR}/include
                             ${CMAKE_SOURCE_DIR}/easycache)
  target_link_libraries(EasyCacheUnitTest ${LIB_NAME} GTest::gtest
                        GTest::gtest_main)
endif()
