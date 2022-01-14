#ifndef TEMPLATELISP_HPP
#define TEMPLATELISP_HPP

#include <concepts>
#include <utility>
#include <type_traits>

/****************/
/*  Basic types */
/****************/
// Basic struct: Data
// types: Int, Char, Float, String

/************/
/* Variable */
/************/
// Var

/*************/
/* Functions */
/*************/
// ConcatCharSequence
// MakeCharSequenceFromString
// IsSameTemplate_Type
// HasSameTemplateName 
// StringToInt
// Add

/*******************/
/* Data structures */
/*******************/
// DataContainer
// List

template <typename T>
struct Data {
    using dataType = T;
    static constexpr size_t dataLength = 0;
    T value;
    constexpr Data(T _value) : value(_value) {}
    constexpr operator T() const { return value; }
    constexpr T operator()() const { return value; }
    template <typename U>
    requires(!std::same_as<T, U>) constexpr Data(U) = delete;
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
concept IsDataString = std::same_as<typename T::dataType, char[T::dataLength]>;

template <Data<int>>
struct Int {};
template <Data<char>>
struct Char {};
template <Data<float>>
struct Float {};
template <Data x>
requires(IsDataString<decltype(x)>) struct String {
};

template <typename, typename>
struct ConcatCharSequence {};
template <char... Args1, char... Args2>
struct ConcatCharSequence<std::integer_sequence<char, Args1...>, std::integer_sequence<char, Args2...>> {
    using result = std::integer_sequence<char, Args1..., Args2...>;
};

template <typename>
struct MakeCharSequenceFromString {};
template <Data s>
struct MakeCharSequenceFromString<String<s>> {
    template <size_t N, Data str>
    requires(IsDataString<decltype(str)>) struct MakeCharSequenceFromString_Helper {
        using result = typename ConcatCharSequence<typename MakeCharSequenceFromString_Helper<N - 1, str>::result, std::integer_sequence<char, str[N - 1]>>::result;
    };

    template <Data str>
    requires(IsDataString<decltype(str)>) struct MakeCharSequenceFromString_Helper<1, str> {
        using result = std::integer_sequence<char, str[0]>;
    };

    using result = typename MakeCharSequenceFromString_Helper<(size_t)s.dataLength, s>::result;
};

template <Data x>
requires(IsDataString<decltype(x)>) struct Var {
};

#define Var(name) Var<#name>

template <typename...>
struct DataContainer {};

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

template <typename... Rs>
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

template <typename... Ts>
requires HasSameTemplateName<Ts...>::value struct List : DataContainer<Ts...> {
};

template <typename>
struct StringToInt {};
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

template <typename, typename, typename...>
struct Add {};
template <Data a, Data b>
struct Add<Int<a>, Int<b>> {
    using result = Int<a + b>;
};
template <Data a, Data b, Data... Args>
struct Add<Int<a>, Int<b>, Int<Args>...> {
    using result = typename Add<typename Add<Int<a>, Int<b>>::result, Int<Args>...>::result;
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