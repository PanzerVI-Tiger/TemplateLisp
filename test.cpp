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

    print<>(HasSameTemplate<Number<1>, Bool<true>>::value);

    print< List<Number<1>> >();

    print<StringToCharList<String<"fuck you">>::result>();
    print<>(StringToCharList<String<"fuck you">>::result::size);

    using test1 = GetElementAt< List<Number<1>, Number<2>, Number<3>, Number<4>>, Index<0>>::result;
    print<test1>();

    using test2 = ExpandAndFill< 
                    List<Number<1>, Number<2>, Number<3>, Number<4>>, 
                    Index<6>, 
                    Number<0>>::result;
    print<test2>();

    using test3 = IncreaseOne< List<Number<1>, Number<2>, Number<3>, Number<4>>, Index<3>>::result;
    print<test3>();

    
    return 0;
}