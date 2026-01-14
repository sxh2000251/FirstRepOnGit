#include <cstring>

struct Foo
{
    int a, b, c;
};

namespace Ns
{
class Bar
{
public:
    struct Foobar
    {
        int a;
        int b;
    };

private:
    int t;

    void method1()
    {
        // ...
    }

    enum List
    {
        ITEM1,
        ITEM2,
    };

    template <typename T>
    void method2(T x)
    {
        // ...
    }

    int i, j, k;
    int t2;
    double m6;

    void method3(int par)
    {
        // ...
    }

    void method4(int par)
    {
        // ...
    }
};

class C
{
};
} // namespace Ns