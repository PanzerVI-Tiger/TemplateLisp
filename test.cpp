#include <cxxabi.h>
#include <iostream>

#include "TemplateLispLib/HeaderFile.hpp"

template <typename x>
void print() {
    std::cout << abi::__cxa_demangle(typeid(x).name(), 0, 0, 0) << std::endl;
}
template <typename T>
void print(T x) {
    std::cout << x << std::endl;
}

int main(void)
{
    print<Int<1>>();
    print<Float<1.0f>>();
    print<Char<'1'>>();
    print<String<"123">>();
    print<MakeCharList<String<"456">>::result>();
    print<>( IsSameTemplate<String<"123">, String<"456">>::value );
    print<>( IsAllSameTemplate<Int<1>>::value );
    return 0;
}