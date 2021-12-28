#include "TL_BasicType.hpp"

int main(void)
{
    print<Int<1>>();
    print<Float<1.0f>>();
    print<Char<'1'>>();
    print<String<"123">>();
    print<String2<"456">>();
    return 0;
}