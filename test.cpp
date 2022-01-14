#include <cxxabi.h>
#include <iostream>

#include "TemplateLispLib/TemplateLisp.hpp"

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
    
    print<MakeCharSequence<String<"456">>::result>();
    print<>( IsSameTemplate_Value<Int, Int>::result );
    print<>( HasSameTemplateName<Int<1>, Int<2>>::result );
    print<Var(fuck)>();
    
    print<StringToInt<String<"123">>::result >();
    print<Add<Int<1>,Int<2>,Int<3>>::result>();

    return 0;
}