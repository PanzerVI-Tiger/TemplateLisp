#include <cxxabi.h>
#include <iostream>
#include <type_traits>
#include <concepts>

template <typename x>
void print() {
    std::cout << abi::__cxa_demangle(typeid(x).name(), 0, 0, 0) << std::endl;
}

template <typename T>
struct DataMaker {
    using dataType = T;
    static constexpr size_t dataLength = 0;
    T value;
    constexpr DataMaker(T _value) : value(_value) {}
    constexpr operator T() const { return value; }
    constexpr T operator()() const { return value; }
    template <typename U>
    requires(!std::same_as<T, U>) constexpr DataMaker(U) = delete;
    constexpr DataMaker() = default;
    constexpr ~DataMaker() = default;
};

template <typename T, size_t N>
struct DataMaker<T[N]> {
    using dataType = T[N];
    static constexpr size_t dataLength = N;
    T value[N];
    constexpr DataMaker(const T (&_value)[N]) {
        std::copy(_value, _value + N, value);
    }
    constexpr operator const T*() const { return value; }
    constexpr const T* operator()() const { return value; }
    constexpr T operator []( const size_t n ) { return value[n]; }
    constexpr DataMaker() = default;
    constexpr ~DataMaker() = default;
};

template<typename T, size_t N> 
DataMaker(const T (&)[N])  -> DataMaker<T[N]>;

template<typename T> 
DataMaker(T)  -> DataMaker<T>;

template <typename T>
concept IsDataMakerString = std::same_as<typename T::dataType, char[T::dataLength]>;

template <DataMaker<int>>
struct Int {};
template <DataMaker<char>>
struct Char {};
template <DataMaker<float>>
struct Float {};
template <DataMaker x>
    requires(IsDataMakerString<decltype(x)>) 
struct String {};

template <template <typename...> typename T, template <typename...> typename U>
struct _IsSameTemplate_Type : std::false_type {};
template <template <typename...> typename T>
struct _IsSameTemplate_Type<T, T> : std::true_type {};

template <template <auto...> typename T, template <auto...> typename U>
struct _IsSameTemplate_NonType : std::false_type {};
template <template <auto...> typename T>
struct _IsSameTemplate_NonType<T, T> : std::true_type {};
template <>
struct _IsSameTemplate_NonType<String, String> : std::true_type {};

template <template <typename...> typename T, template <typename...> typename U>
constexpr bool IsSameTemplate() {
    return _IsSameTemplate_Type<T, U>::value;
}
template <template <auto...> typename T, template <auto...> typename U>
constexpr bool IsSameTemplate() {
    return _IsSameTemplate_NonType<T, U>::value;
}
template <template <typename...> typename T, template <auto...> typename U>
constexpr bool IsSameTemplate() {
    return false;
}
template <template <auto...> typename T, template <typename...> typename U>
constexpr bool IsSameTemplate() {
    return false;
}

template <typename T, T... data>
struct BasicType {};

template <typename A, typename B>
struct concat {};
template <char... Args1, char... Args2>
struct concat<BasicType<char, Args1...>, BasicType<char, Args2...>> {
    using result = BasicType<char, Args1..., Args2...>;
};

template <size_t N, DataMaker str>
    requires(IsDataMakerString<decltype(str)>) 
struct MakeString {
    using result = typename concat<typename MakeString<N - 1, str>::result, BasicType<char, str[N - 1]>>::result;
};

template <DataMaker str>
    requires(IsDataMakerString<decltype(str)>) 
struct MakeString<1, str> {
    using result = BasicType<char, str[0]>;
};

template <DataMaker str>
using String2 = typename MakeString<(size_t)str.dataLength, str>::result;

