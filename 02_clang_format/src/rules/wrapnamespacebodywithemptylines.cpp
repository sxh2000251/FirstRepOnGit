
// WrapNamespaceBodyWithEmptyLines: Always
namespace TestNameSpace
{

int a = 3;

namespace NestedNameSpace
{

int b = 3;

}
} // namespace TestNameSpace

// clang-format off: 演示 WrapNamespaceBodyWithEmptyLines: Never
namespace TestNameSpace2
{
int a = 3;

namespace NestedNameSpace2
{
int b = 3;
}
} // namespace TestNameSpace2

// clang-format on: 演示结束