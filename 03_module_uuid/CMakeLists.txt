cmake_minimum_required(VERSION 4.0.3)

# 保证动态库文件和可执行文件输出到同目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
# 显示更详细的编译信息
# set(CMAKE_VERBOSE_MAKEFILE ON)

project(module_lib LANGUAGES CXX)

# 导入子目录
add_subdirectory(lib)
# 添加可执行文件目标
add_executable(main src/main.cpp)
# 链接动态库
target_link_libraries(main PRIVATE uuid_tool)
# 设置 C++ 标准和标准库模块支持
set_target_properties(main PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

