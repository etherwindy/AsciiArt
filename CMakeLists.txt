# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(asciiArt VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)

# 支持 unicode
add_definitions(-DUNICODE -D_UNICODE)

# 设置不同系统下的编译选项
if(WIN32)
    message(STATUS "Now is windows")
    set(CMAKE_GENERATOR "MinGW Makefiles")
elseif(LINUX)
    message(STATUS "Now is Linux")
endif()

# 指定构建类型，如果不指定，默认的构建类型是 Release
set(CMAKE_BUILD_TYPE Debug)

# 添加头文件搜索路径
set(SRC_PATH ${PROJECT_SOURCE_DIR}/src)

# 添加可执行文件
add_executable(asciiArt ${SRC_PATH}/main.cpp)