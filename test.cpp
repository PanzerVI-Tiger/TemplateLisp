#include "TemplateLispLib/BasicTypes.hpp"

template <typename x>
void print() {
    std::cout << abi::__cxa_demangle(typeid(x).name(), 0, 0, 0) << std::endl;
}

int main(void)
{
    print<Int<1>>();
    print<Float<1.0f>>();
    print<Char<'1'>>();
    print<String<"123">>();
    print<String2<"456">>();
    return 0;
}