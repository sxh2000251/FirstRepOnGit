cmake_minimum_required(VERSION 4.0.3)

# 预编译的模块接口文件无法跨编译器使用，不同编译器处理方式不同
# 预编译的模块接口文件无法跨版本使用，比如 C++20 的项目无法给 C++23 版本使用（即使编译器相同）

set(lib_name "HelloLib")
# 编译为静态库
# 只需修改 CMakeLists.txt ，将库类型设为 STATIC ，并删除最下方的导出符号宏定义即可
add_library(${lib_name} STATIC) 
# 编译为动态库
# add_library(${lib_name} SHARED) 

# 获取文件列表
file(GLOB_RECURSE CXX_MODULE_FILES "modules/*.cppm" "modules/*.ixx")
file(GLOB_RECURSE CXX_CPP_FILES "src/*.cpp")

## 添加源文件与模块接口文件
# 模块实现文件可以 private 隐藏
# 不推荐: 可以仅接口文件（将函数实现也写在接口），但编译速度较慢。
target_sources(${lib_name} PRIVATE ${CXX_CPP_FILES})

# 模块接口文件需要 public 暴露给下层，
# 模块接口文件必须使用 target_sources 命令配合 CXX_MODULES 类型加入目标。
target_sources(${lib_name} PUBLIC
        FILE_SET public_modules # FILE_SET任意取名
        TYPE CXX_MODULES    # 模块接口文件 需要使用 CXX_MODULES 类型
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/modules
        FILES ${CXX_MODULE_FILES}
)

# 设置 C++ 标准
set_target_properties(${lib_name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

## 设置动态链接库的导出符号，MSVC与Clang/GNU的才处理方式不同
## 方案1: 
#if(MSVC)
#    target_compile_definitions(${lib_name} PRIVATE "LIB_EXPORT=__declspec(dllexport)")
#else()
#### 控制visibility来控制导出的接口的数量
#    target_compile_options(${lib_name} PRIVATE -fvisibility=hidden)
#    target_compile_definitions(${lib_name} PRIVATE "LIB_EXPORT=__attribute__((visibility(\"default\")))")
#endif()

## 方案2: 更加简单, 导出所有符号, 缺点就是所有的都导出, 没法控制
# set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
