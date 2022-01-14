#ifndef TEMPLATELISP_HPP
#define TEMPLATELISP_HPP

#include <concepts>
#include <utility>
#include <type_traits>

/****************/
/*  Basic types */
/****************/
/*
DataMaker, std::integer_sequence
*/
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
    constexpr DataMaker(const T *_value) {
        std::copy(_value, _value + N, value);
    }
    constexpr operator const T *() const { return value; }
    constexpr const T *operator()() const { return value; }
    constexpr T operator[](const size_t n) { return value[n]; }
    constexpr DataMaker() = default;
    constexpr ~DataMaker() = default;
};

template <typename T, size_t N>
DataMaker(const T (&)[N]) -> DataMaker<T[N]>;

template <size_t N>
DataMaker(const char (&)[N]) -> DataMaker<char[N - 1]>;

template <typename T>
DataMaker(T) -> DataMaker<T>;

template <typename T>
concept IsDataMakerString = std::same_as<typename T::dataType, char[T::dataLength]>;

template <DataMaker<int>>
struct Int {};
template <DataMaker<char>>
struct Char {};
template <DataMaker<float>>
struct Float {};
template <DataMaker x>
requires(IsDataMakerString<decltype(x)>) struct String {
};

template <typename, typename>
struct ConcatCharSequence {};
template <char... Args1, char... Args2>
struct ConcatCharSequence<std::integer_sequence<char, Args1...>, std::integer_sequence<char, Args2...>> {
    using result = std::integer_sequence<char, Args1..., Args2...>;
};

template <typename>
struct MakeCharSequence {};
template <DataMaker s>
struct MakeCharSequence<String<s>> {
    template <size_t N, DataMaker str>
    requires(IsDataMakerString<decltype(str)>) struct MakeCharSequence_Helper {
        using result = typename ConcatCharSequence<typename MakeCharSequence_Helper<N - 1, str>::result, std::integer_sequence<char, str[N - 1]>>::result;
    };

    template <DataMaker str>
    requires(IsDataMakerString<decltype(str)>) struct MakeCharSequence_Helper<1, str> {
        using result = std::integer_sequence<char, str[0]>;
    };

    using result = typename MakeCharSequence_Helper<(size_t)s.dataLength, s>::result;
};

/************/
/* Variable */
/************/

template <DataMaker x>
requires(IsDataMakerString<decltype(x)>) struct Var {
};

#define Var(name) Var<#name>

/*************/
/* Functions */
/*************/

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

/*******************/
/* Data structures */
/*******************/
template <typename... Ts>
requires HasSameTemplateName<Ts...>::value struct List : DataContainer<Ts...> {
};

template <typename>
struct StringToInt {};
template <DataMaker str>
constexpr int _StringToInt_Impl() {
    int temp = 0;
    for (int i = 0; i < str.dataLength; ++i) {
        temp *= 10;
        temp += (str[i] - 48);
    }
    return temp;
}
template <DataMaker str>
struct StringToInt<String<str>> {
    using result = Int<_StringToInt_Impl<str>()>;
};

template <typename, typename, typename...>
struct Add {};
template <DataMaker a, DataMaker b>
struct Add<Int<a>, Int<b>> {
    using result = Int<a + b>;
};
template <DataMaker a, DataMaker b, DataMaker... Args>
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