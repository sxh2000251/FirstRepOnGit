export module Hello;

// 动态链接库的导出符号，参考CMakeLists.txt
#ifndef LIB_EXPORT
#define LIB_EXPORT
#endif

export namespace Hello
{
    // 函数声明需要使用导出符号标记（注意这个是编译器相关的导出符号，而不是export关键字）
    LIB_EXPORT void say();
} // namespace Hello
