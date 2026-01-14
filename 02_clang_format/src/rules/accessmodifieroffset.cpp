class TestClass
{
    // clang-format off: 演示 AccessModifierOffset(访问说明符 public/private 的偏移，与类对齐)
public:         // AccessModifierOffset = -4
  protected:    // AccessModifierOffset = -2
    private:    // AccessModifierOffset = 0
    // clang-format on: 演示结束
};