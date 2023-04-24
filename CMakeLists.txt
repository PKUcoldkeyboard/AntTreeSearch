cmake_minimum_required(VERSION 3.8)

project(AntTreeSearch)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")


include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
CHECK_CXX_COMPILER_FLAG("-mtune=generic" COMPILER_SUPPORTS_MTUNE_GENERIC)

if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
endif()

if(COMPILER_SUPPORTS_MTUNE_GENERIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=generic")
endif()

# 添加项目源文件
set(SOURCES
    src/main.cpp
)

# 添加头文件目录
include_directories(include)

# 定义可执行文件
add_executable(ATS ${SOURCES})

