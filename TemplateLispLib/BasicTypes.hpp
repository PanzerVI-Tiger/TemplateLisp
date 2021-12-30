#ifndef BASICTYPES_HPP
#define BASICTYPES_HPP

#include <concepts>
#include <type_traits>

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
    constexpr operator const T *() const { return value; }
    constexpr const T *operator()() const { return value; }
    constexpr T operator[](const size_t n) { return value[n]; }
    constexpr DataMaker() = default;
    constexpr ~DataMaker() = default;
};

template <typename T, size_t N>
DataMaker(const T (&)[N]) -> DataMaker<T[N]>;

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

template <DataMaker x>
    requires(IsDataMakerString<decltype(x)>) 
struct Var {};

#define Var( name ) Var<#name>

#endif