#ifndef TEMPLATELISP_HPP
#define TEMPLATELISP_HPP

#include <concepts>
#include <type_traits>

/****************/
/*  Basic types */
/****************/
/*
DataMaker, TypeDataList
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
DataMaker(const T (&)[N]) -> DataMaker<T[N-1]>;

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
    requires(IsDataMakerString<decltype(x)>) 
struct String {};

template <typename T, T... data>
struct TypeDataList {};

template <typename, typename>
struct ConcatCharList {};
template <char... Args1, char... Args2>
struct ConcatCharList<TypeDataList<char, Args1...>, TypeDataList<char, Args2...>> {
    using result = TypeDataList<char, Args1..., Args2...>;
};

template <typename>
struct MakeCharList {};
template <DataMaker s>
struct MakeCharList<String<s>> {
    template <size_t N, DataMaker str>
    requires(IsDataMakerString<decltype(str)>) struct MakeCharList_Helper {
        using result = typename ConcatCharList<typename MakeCharList_Helper<N - 1, str>::result, TypeDataList<char, str[N - 1]>>::result;
    };

    template <DataMaker str>
    requires(IsDataMakerString<decltype(str)>) struct MakeCharList_Helper<1, str> {
        using result = TypeDataList<char, str[0]>;
    };

    using result = typename MakeCharList_Helper<(size_t)s.dataLength, s>::result;
};

/************/
/* Variable */
/************/

template <DataMaker x>
    requires(IsDataMakerString<decltype(x)>) 
struct Var {};

#define Var( name ) Var<#name>

/*******************/
/* Data structures */
/*******************/

/*************/
/* Functions */
/*************/

template <typename...>
struct DataContainer {};

template <template <typename...> typename T, template <typename...> typename U>
struct _IsSameTemplate_Type : std::false_type {};
template <template <typename...> typename T>
struct _IsSameTemplate_Type<T, T> : std::true_type {};
template <template <auto...> typename T, template <auto...> typename U>
struct _IsSameTemplate_NonType : std::false_type {};
template <template <auto...> typename T>
struct _IsSameTemplate_NonType<T, T> : std::true_type {};
#if defined(__GNUC__)
template <>
struct _IsSameTemplate_NonType<String, String> : std::true_type {};
#endif
template <typename T, typename U>
struct IsSameTemplate : std::false_type {};
template <template <typename...> typename T, typename... TArgs, template <typename...> typename U, typename... UArgs>
struct IsSameTemplate<T<TArgs...>, U<UArgs...>> : _IsSameTemplate_Type<T, U> {};
template <template <auto...> typename T, auto... TArgs, template <auto...> typename U, auto... UArgs>
struct IsSameTemplate<T<TArgs...>, U<UArgs...>> : _IsSameTemplate_NonType<T, U> {};
template <typename... Rs>
struct IsAllSameTemplate {
    static constexpr bool value = true;
};
template <typename T, typename U, typename... Rs>
struct IsAllSameTemplate<T, U, Rs...> {
    static constexpr bool value = IsSameTemplate<T, U>::value && IsAllSameTemplate<U, Rs...>::value;
};
template <typename T, typename U>
struct IsAllSameTemplate<T, U> {
    static constexpr bool value = IsSameTemplate<T, U>::value;
};
template <typename... Ts>
    requires IsAllSameTemplate<Ts...>::value 
struct List : DataContainer<Ts...> {};

template<typename>
struct StringToInt{};
template<DataMaker str>
constexpr int _StringToInt_Impl() {
    int temp = 0;
    for( int i = 0; i < str.dataLength; ++i ){
        temp *= 10;
        temp += (str[i] - 48);
    }
    return temp;
}
template<DataMaker str>
struct StringToInt<String<str>> {
    using result = Int<_StringToInt_Impl<str>()>;
};

template<typename, typename, typename...>
struct Add{};
template<DataMaker a, DataMaker b>
struct Add<Int<a>, Int<b>>{
    using result = Int<a + b>;
};
template<DataMaker a, DataMaker b, DataMaker ...Args>
struct Add<Int<a>, Int<b>, Int<Args>...>{
    using result = typename Add < typename Add<Int<a>, Int<b>> ::result, Int<Args>... > ::result;
};

template<typename str>
struct EvalPlus{};

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