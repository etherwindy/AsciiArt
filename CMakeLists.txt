# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.30)

# 设置项目名称和版本
project(AsciiArt VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)

# 支持 unicode
add_definitions(-DUNICODE -D_UNICODE)

# 指定优化级别
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# 指定构建类型，如果不指定，默认的构建类型是 Release
set(CMAKE_BUILD_TYPE Debug)

# 添加头文件搜索路径
set(SRC_PATH ${PROJECT_SOURCE_DIR}/src)

# 添加可执行文件
add_executable(asciiart ${SRC_PATH}/main.cpp)