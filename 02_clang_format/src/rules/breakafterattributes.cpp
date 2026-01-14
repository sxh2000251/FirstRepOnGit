[[maybe_unused]]
const int i = 3;
[[gnu::const]] [[maybe_unused]]
int j = 1;

[[nodiscard]]
inline int f()
{
    return 1;
}

[[gnu::const]] [[nodiscard]]
int g()
{
    return 1;
}

int h()
{
    [[likely]]
    if (true)
        return f();
    else
        return g();
}

int iddd()
{
    int b = 1;
    switch (b)
    {
    [[unlikely]]
    case 1:
        ++b;
        break;
    [[likely]]
    default:
        break;
    }

    return b;
}