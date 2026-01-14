class Foo
{
    void f() { return; }
};

void f0()
{
    return;
}

void ff0()
{
}

// clang-format off: 演示值为 None
class Foo1
{
    void f()
    {
        return; 
    }
};
void f1()
{
    return; 
}
void ff1()
{
}

// clang-format on: 演示结束

// clang-format off: 演示值为 InlineOnly
class Foo2
{
    void f() { return;  }
};
void f2()
{
    return; 
}
void ff2()
{
}

// clang-format on: 演示结束

// clang-format off: 演示值为 Empty
class Foo3
{
    void f()
    {
        return; 
    }
};
void f3()
{
    return; 
}
void ff3() {}

// clang-format on: 演示结束

// clang-format off: 演示值为 Inline
class Foo4
{
    void f() { return;  }
};
void f4()
{
    return; 
}
void ff4() {}

// clang-format on: 演示结束

// clang-format off: 演示值为 All
class Foo5
{
    void f() { return; }
};
void f5() { return;  }
void ff5() {}

// clang-format on: 演示结束