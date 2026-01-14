class Foo
{
public:
    Foo()
        : m_name(1),
          m_gender(false)
    {
    }

    void f() { return; }

private:
    int m_name;
    bool m_gender;
};

class Doo : public Foo
{
public:
    Doo()
        : Foo(),
          m_age(2)
    {
    }

private:
    int m_age;
};