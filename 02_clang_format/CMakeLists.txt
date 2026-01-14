cmake_minimum_required(VERSION 4.0.3)

# 保证动态库文件和可执行文件输出到同目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

project(module_lib LANGUAGES CXX)

file(GLOB_RECURSE CXX_CPP_FILES CONFIGURE_DEPENDS src/*.cpp)

# 添加可执行文件目标
add_executable(main ${CXX_CPP_FILES})

# 设置 C++ 标准和标准库模块支持
set_target_properties(main PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

