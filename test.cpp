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
    print<Number<1>>();
    print<Bool<true>>();
    print<Char<'1'>>();
    print<String<"1">>();

    print<Add< Number<1>, Number<3.0f>>::result>();

    print<>(HasSameTemplateName<Number<1>, Bool<true>>::result);

    print< List<Number<1>> >();

    print<StringToCharList<String<"fuck you">>::result>();
    print<>(StringToCharList<String<"fuck you">>::result::size);
    
    return 0;
}