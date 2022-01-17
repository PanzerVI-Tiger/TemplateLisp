#ifndef TEMPLATELISP_HPP
#define TEMPLATELISP_HPP

#include <concepts>
#include <type_traits>
#include <utility>

/****************/
/*  Basic types */
/****************/
// Basic struct: Data
// type: Number, Bool, Char, String

/************/
/* concepts */
/************/
// IsCppCharArray
// IsCppNumber
// IsCppChar
// IsCppBool
// IsNumber
// IsChar
// IsBool
// IsString


/************/
/* Variable */
/************/
// Var

/*************/
/* Functions */
/*************/
// ConcatContainer
// StringToCharList
// IsSameTemplate_Type
// IsSameTemplate_Value
// HasSameTemplateName
// TODO: StringToInt
// Add
// GetBasicType

/*******************/
/* Data structures */
/*******************/
// DataContainer
// List

//------------------------------------------------------------------------------------

template <template <typename...> typename T, template <typename...> typename U>
struct IsSameTemplate_Type {
    static constexpr bool result = false;
};

template <template <typename...> typename T>
struct IsSameTemplate_Type<T, T> {
    static constexpr bool result = true;
};
template <template <auto...> typename T, template <auto...> typename U>
struct IsSameTemplate_Value {
    static constexpr bool result = false;
};

template <template <auto...> typename T>
struct IsSameTemplate_Value<T, T> {
    static constexpr bool result = true;
};

template <typename... Ts>
struct HasSameTemplateName {
    static constexpr bool result = true;
};
template <
    template <typename...> typename T, typename... TArgs,
    template <typename...> typename U, typename... UArgs,
    typename... Rs>
struct HasSameTemplateName<T<TArgs...>, U<UArgs...>, Rs...> {
    static constexpr bool result = IsSameTemplate_Type<T, U>::result && HasSameTemplateName<U<UArgs...>, Rs...>::result;
};
template <
    template <auto...> typename T, auto... TArgs,
    template <auto...> typename U, auto... UArgs,
    typename... Rs>
struct HasSameTemplateName<T<TArgs...>, U<UArgs...>, Rs...> {
    static constexpr bool result = IsSameTemplate_Value<T, U>::result && HasSameTemplateName<U<UArgs...>, Rs...>::result;
};

template <typename T>
struct Data {
    using dataType = T;
    T value;
    constexpr Data(T _value) : value(_value) {}
    constexpr operator T() const { return value; }
    constexpr T operator()() const { return value; }
    constexpr Data() = default;
    constexpr ~Data() = default;
};

template <typename T, size_t N>
struct Data<T[N]> {
    using dataType = T[N];
    static constexpr size_t dataLength = N;
    T value[N];
    constexpr Data(const T *_value) {
        std::copy(_value, _value + N, value);
    }
    constexpr operator const T *() const { return value; }
    constexpr const T *operator()() const { return value; }
    constexpr T operator[](const size_t n) { return value[n]; }
    constexpr Data() = default;
    constexpr ~Data() = default;
};

template <typename T, size_t N>
Data(const T (&)[N]) -> Data<T[N]>;

template <size_t N>
Data(const char (&)[N]) -> Data<char[N - 1]>;

template <typename T>
Data(T) -> Data<T>;

template <typename T>
struct GetBasicType {};
template <template <Data> typename T, Data x>
struct GetBasicType<T<x>> {
    using result = typename decltype(x)::dataType;
};

template <typename T>
concept IsCppCharArray = std::same_as<typename T::dataType, char[T::dataLength]>;

template <typename T>
concept IsCppNumber = (std::integral<typename T::dataType> || std::floating_point<typename T::dataType>)&&!std::same_as<typename T::dataType, char> && !std::same_as<typename T::dataType, bool>;

template <typename T>
concept IsCppChar = std::same_as<typename T::dataType, char>;

template <typename T>
concept IsCppBool = std::same_as<typename T::dataType, bool>;

template <Data x> requires(IsCppNumber<decltype(x)>) 
struct Number {};

template <Data x> requires(IsCppChar<decltype(x)>) 
struct Char {};

template <Data x> requires(IsCppBool<decltype(x)>) 
struct Bool {};

template <Data x> requires(IsCppCharArray<decltype(x)>) 
struct String {};

template <template<auto...> typename T>
concept IsNumber = IsSameTemplate_Value<Number, T>::result;

template <template<auto...> typename T>
concept IsBool = IsSameTemplate_Value<Bool, T>::result;

template <template<auto...> typename T>
concept IsChar = IsSameTemplate_Value<Char, T>::result;

template <template<auto...> typename T>
concept IsString = IsSameTemplate_Value<String, T>::result;

template <Data x> requires(IsCppCharArray<decltype(x)>) 
struct Var {};

#define Var(name) Var<#name>

template <typename... Ts>
struct DataContainer {
    static constexpr size_t size = sizeof...(Ts);
};

template <typename... Ts> requires HasSameTemplateName<Ts...>::result 
struct List : DataContainer<Ts...> {
    static constexpr size_t size = sizeof...(Ts);
};

template<template<typename...> typename T>
concept IsDataContainer = IsSameTemplate_Type<T, DataContainer>::result || IsSameTemplate_Type<T, List>::result;

template <typename T, typename U, typename... Rs>
struct ConcatContainer {
    using result = typename ConcatContainer<typename ConcatContainer<T, U>::result, Rs...>::result;
};
template <typename... Args1, typename... Args2, template<typename...> typename Container> 
    requires (IsDataContainer<Container>)
struct ConcatContainer<Container<Args1...>, Container<Args2...>> {
    using result = Container<Args1..., Args2...>;
};

template <typename>
struct StringToCharList {};
template <Data s>
struct StringToCharList<String<s>> {

    template <size_t N, Data str> 
        requires(IsCppCharArray<decltype(str)>) 
    struct StringToCharList_Helper {
        using result = typename ConcatContainer<typename StringToCharList_Helper<N - 1, str>::result,
                                                   List<Char<str[N - 1]>>>::result;
    };

    template <Data str> 
        requires(IsCppCharArray<decltype(str)>) 
    struct StringToCharList_Helper<1, str> {
        using result = List<Char<str[0]>>;
    };

    using result = typename StringToCharList_Helper<(size_t)s.dataLength, s>::result;
};


/*
template <typename>
struct StringToNumber {};
template <Data str>
constexpr int _StringToInt_Impl() {
    int temp = 0;
    for (int i = 0; i < str.dataLength; ++i) {
        temp *= 10;
        temp += (str[i] - 48);
    }
    return temp;
}
template <Data str>
struct StringToInt<String<str>> {
    using result = Int<_StringToInt_Impl<str>()>;
};
*/

template <typename T, typename U, typename... Rs>
struct Add {
    using result = typename Add<typename Add<T, U>::result, Rs...>::result;
};
template <Data a, Data b>
struct Add<Number<a>, Number<b>> {
    using result = Number<a + b>;
};

template <typename str>
struct EvalPlus {};

/*
template< typename ... Pair>
struct Eval{};
template< typename ... Exprs >
struct Eval{};
template< typename Lambda, typename Param, typename Env>
struct Call{};
template< typename Func, typename Param, typename Env>
struct Call{};
template< typename Name, typename Lambda, typename Env>
struct Define{};
template< typename Param, typename Body >
struct Lambda{};
*/

#endif