module;
#include <iostream>
// 模块实现文件，开头必须包含 module XXX;
module Hello;

// 函数实现，无需使用导出符号标记
void Hello::say()
{
    std::cout << "Hello World!\n";
}
