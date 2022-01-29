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
// HasSameTemplate
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
    static constexpr bool value = false;
};

template <template <typename...> typename T>
struct IsSameTemplate_Type<T, T> {
    static constexpr bool value = true;
};
template <template <auto...> typename T, template <auto...> typename U>
struct IsSameTemplate_Value {
    static constexpr bool value = false;
};

template <template <auto...> typename T>
struct IsSameTemplate_Value<T, T> {
    static constexpr bool value = true;
};

template <typename... Ts>
struct HasSameTemplate {
    static constexpr bool value = true;
};
template <
    template <typename...> typename T, typename... TArgs,
    template <typename...> typename U, typename... UArgs,
    typename... Rs>
struct HasSameTemplate<T<TArgs...>, U<UArgs...>, Rs...> {
    static constexpr bool value = IsSameTemplate_Type<T, U>::value && HasSameTemplate<U<UArgs...>, Rs...>::value;
};
template <
    template <auto...> typename T, auto... TArgs,
    template <auto...> typename U, auto... UArgs,
    typename... Rs>
struct HasSameTemplate<T<TArgs...>, U<UArgs...>, Rs...> {
    static constexpr bool value = IsSameTemplate_Value<T, U>::value && HasSameTemplate<U<UArgs...>, Rs...>::value;
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
    T value[N]{};
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

template<size_t x>
struct Index{
    static constexpr size_t value = x;
};

template <Data x> requires(IsCppNumber<decltype(x)>) 
struct Number {};

template <Data x> requires(IsCppChar<decltype(x)>) 
struct Char {};

template <Data x> requires(IsCppBool<decltype(x)>) 
struct Bool {};

template <Data x> requires(IsCppCharArray<decltype(x)>) 
struct String {};

/*
template <template<auto...> typename T>
concept IsNumber = IsSameTemplate_Value<Number, T>::result;

template <template<auto...> typename T>
concept IsBool = IsSameTemplate_Value<Bool, T>::result;

template <template<auto...> typename T>
concept IsChar = IsSameTemplate_Value<Char, T>::result;

template <template<auto...> typename T>
concept IsString = IsSameTemplate_Value<String, T>::result;
*/

template <Data x> requires(IsCppCharArray<decltype(x)>) 
struct Var {};

#define Var(name) Var<#name>

template <typename... Ts>
struct DataContainer {
    static constexpr size_t size = sizeof...(Ts);
};

template <typename... Ts> 
    requires (HasSameTemplate<Ts...>::value)
struct List : DataContainer<Ts...> {
    static constexpr size_t size = sizeof...(Ts);
};

template<typename T>
concept HasContainerTemplate = HasSameTemplate<DataContainer<>, T>::value || HasSameTemplate<List<>, T>::value;

template<template<typename...> typename T>
concept IsContainer = HasContainerTemplate<T<>>;

template<typename T>
concept HasListTemplate = HasSameTemplate<List<>, T>::value;

template<template<typename...> typename T>
concept IsList = HasListTemplate<T<>>;


template <typename container1, typename container2, typename... containers>
    requires(HasContainerTemplate<container1> && HasSameTemplate<container1, container2, containers...>::value)
struct ConcatContainer {
    using result = typename ConcatContainer<typename ConcatContainer<container1, container2>::result, containers...>::result;
};
template <typename... elements1, typename... elements2, template<typename...> typename container> 
struct ConcatContainer<container<elements1...>, container<elements2...>> {
    using result = container<elements1..., elements2...>;
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

template <typename num1, typename num2, typename... numbers>
struct Add {
    using result = typename Add<typename Add<num1, num2>::result, numbers...>::result;
};
template <Data num1, Data num2>
struct Add<Number<num1>, Number<num2>> {
    using result = Number<num1 + num2>;
};

template <typename container, typename index>
    requires(HasContainerTemplate<container> && index::value <= container::size)
struct GetElementAt {};
template <template <typename...> typename container, typename head, typename... tail, size_t index>
struct GetElementAt<container<head, tail...>, Index<index>> {
    using result = typename GetElementAt<container<tail...>, Index<index - 1>>::result;
};
template <template <typename...> typename container, typename head, typename... tail>
struct GetElementAt<container<head, tail...>, Index<1>> {
    using result = head;
};

template <typename container, typename index> 
    requires(HasContainerTemplate<container> && index::value <= container::size)
struct IncreaseOne {};
template <template <typename...> typename container, typename head, typename... tail, size_t index>
struct IncreaseOne<container<head, tail...>, Index<index>> {
    using result = typename ConcatContainer<
        container<head>,
        typename IncreaseOne<container<tail...>, Index<index - 1>>::result>::result;
};
template <template <typename...> typename container, template <Data...> typename head, Data x, typename... tail>
struct IncreaseOne<container<head<x>, tail...>, Index<1>> { // TODO: head should be Number<> or Char<>
    using result = container<head<x + 1>, tail...>;
};
/*
template <template <typename...> typename container, Data x, typename... tail>
struct IncreaseOne<container<Number<x>, tail...>, Index<1>> { // TODO: head should be Number<> or Char<>
    using result = container<Number<x + 1>, tail...>;
};
template <template <typename...> typename container, Data x, typename... tail>
struct IncreaseOne<container<Char<x>, tail...>, Index<1>> { // TODO: head should be Number<> or Char<>
    using result = container<Char<x + char(1)>, tail...>;
};
*/

template <typename container, typename index> 
   requires(HasContainerTemplate<container> && index::value <= container::size)
struct DecreaseOne {};
template <template <typename...> typename container, typename head, typename... tail, size_t index>
struct DecreaseOne<container<head, tail...>, Index<index>> {
    using result = typename ConcatContainer<
        container<head>,
        typename DecreaseOne<container<tail...>, Index<index - 1>>::result>::result;
};
template <template <typename...> typename container, template <Data...> typename head, Data x, typename... tail>
struct DecreaseOne<container<head<x>, tail...>, Index<1>> { // TODO: head should be Number<> or Char<>
    using result = container<head<x - 1>, tail...>;
};

template <typename container, typename index, typename to_add>
    requires(HasContainerTemplate<container>)
struct ExpandAndFill {};
template <typename container, size_t index, typename to_add>
    requires(index <= container::size) 
struct ExpandAndFill<container, Index<index>, to_add> {
    using result = container;
};
template <template <typename...> typename container, typename... tail, size_t index, typename to_add>
    requires(index > container<tail...>::size && (IsContainer<container>)) 
struct ExpandAndFill<container<tail...>, Index<index>, to_add> {

    template <typename idx, typename ta>
    struct ExpandAndFill_Impl {};
    template <size_t idx, typename ta>
    struct ExpandAndFill_Impl<Index<idx>, ta> {
        using result = typename ConcatContainer<
            container<ta>,
            typename ExpandAndFill_Impl<Index<idx - 1>, ta>::result>::result;
    };
    template <typename ta>
    struct ExpandAndFill_Impl<Index<1>, ta> {
        using result = container<ta>;
    };

    using result = typename ConcatContainer<
        container<tail...>,
        typename ExpandAndFill_Impl<Index<index - container<tail...>::size>, to_add>::result>::result;
};

template<typename condition, typename result1, typename result2>
struct _IfElse{};
template<typename result1, typename result2>
struct _IfElse<Bool<true>, result1, result2>{
    using result = result1;
};
template<typename result1, typename result2>
struct _IfElse<Bool<false>, result1, result2>{
    using result = result2;
};

template<typename condition, typename result1, typename result2>
using IfElse = typename _IfElse<condition, result1, result2>::result;

template<typename container, typename to_push>
struct Push{};
template<template <typename...> typename container, typename... tail, typename to_push>
struct Push<container<tail...>, to_push>{
    using result = container<to_push, tail...>;
};

template<typename container>
struct Pop{};
template<template <typename...> typename container, typename head, typename... tail> // requires size >= 1
struct Pop<container<head, tail...>>{
    using result = container<tail...>;
};

template<typename container>
struct Peak{};
template<template <typename...> typename container, typename head, typename... tail> // requires size >= 1
struct Peak<container<head, tail...>>{
    using result = head;
};

template< typename source, typename recorder, typename src_ptr, typename memory, typename output, typename mem_ptr>
struct EvalBF_Impl{};
//----------------------------------------------------------------
template <typename cmd, typename source, typename recorder, typename src_ptr, typename data, typename memory, typename output, typename mem_ptr>
struct EvalBF_ImplHelper {};
template <typename source, typename recorder, size_t src_ptr_index, typename data, typename memory, typename output, size_t mem_ptr_index>
struct EvalBF_ImplHelper<Char<'+'>, source, recorder, Index<src_ptr_index>, data, memory, output, Index<mem_ptr_index>> {
    using result =
        typename EvalBF_Impl<
            source,
            recorder,
            Index<src_ptr_index + 1>,
            typename IncreaseOne<typename ExpandAndFill<memory, Index<mem_ptr_index>, Number<0>>::result, Index<mem_ptr_index>>::result,
            output,
            Index<mem_ptr_index>
        >::result;
};
template <typename source, typename recorder, size_t src_ptr_index, typename data, typename memory, typename output, size_t mem_ptr_index>
struct EvalBF_ImplHelper<Char<'-'>, source, recorder, Index<src_ptr_index>, data, memory, output, Index<mem_ptr_index>> {
    using result =
        typename EvalBF_Impl<
            source,
            recorder,
            Index<src_ptr_index + 1>,
            typename DecreaseOne<typename ExpandAndFill<memory, Index<mem_ptr_index>, Number<0>>::result, Index<mem_ptr_index>>::result,
            output,
            Index<mem_ptr_index>
        >::result;
};
template <typename source, typename recorder, size_t src_ptr_index, typename data, typename memory, typename output, size_t mem_ptr_index>
struct EvalBF_ImplHelper<Char<'>'>, source, recorder, Index<src_ptr_index>, data, memory, output, Index<mem_ptr_index>> {
    using result =
        typename EvalBF_Impl<
            source,
            recorder,
            Index<src_ptr_index + 1>,
            typename ExpandAndFill<memory, Index<mem_ptr_index + 1>, Number<0>>::result,
            output,
            Index<mem_ptr_index + 1>
        >::result;
};
template <typename source, typename recorder, size_t src_ptr_index, typename data, typename memory, typename output, size_t mem_ptr_index>
struct EvalBF_ImplHelper<Char<'<'>, source, recorder, Index<src_ptr_index>, data, memory, output, Index<mem_ptr_index>> {
    using result =
        typename EvalBF_Impl<
            source,
            recorder,
            Index<src_ptr_index + 1>,
            memory,
            output,
            Index<mem_ptr_index - 1>
        >::result;
};
template<typename source, typename recorder, size_t src_ptr_index, typename data, typename memory, typename output, size_t mem_ptr_index>
struct EvalBF_ImplHelper<Char<'.'>, source, recorder, Index<src_ptr_index>, data, memory, output, Index<mem_ptr_index>>{
    using result =
        typename EvalBF_Impl<
            source,
            recorder,
            Index<src_ptr_index + 1>,
            memory,
            typename ConcatContainer<output, List<typename GetElementAt<memory, Index<mem_ptr_index>>::result>>::result,
            Index<mem_ptr_index>
        >::result;
};
template <typename source, typename recorder, size_t src_ptr_index, typename data, typename memory, typename output, size_t mem_ptr_index>
struct EvalBF_ImplHelper<Char<'['>, source, recorder, Index<src_ptr_index>, data, memory, output, Index<mem_ptr_index>> {
    using result =
        typename EvalBF_Impl<
            source,
            typename Push<recorder, Index<src_ptr_index>>::result,
            Index<src_ptr_index + 1>,
            memory,
            output,
            Index<mem_ptr_index>
        >::result;
};
template <typename source, typename recorder, size_t src_ptr_index, typename data, typename memory, typename output, size_t mem_ptr_index>
struct EvalBF_ImplHelper<Char<']'>, source, recorder, Index<src_ptr_index>, data, memory, output, Index<mem_ptr_index>> {
    using result =
        typename EvalBF_Impl<
            source,
            typename Pop<recorder>::result,
            typename Peak<recorder>::result,
            memory,
            output,
            Index<mem_ptr_index>
        >::result;
};
template <typename source, typename recorder, size_t src_ptr_index, typename memory, typename output, size_t mem_ptr_index>
struct EvalBF_ImplHelper<Char<']'>, source, recorder, Index<src_ptr_index>, Number<0>, memory, output, Index<mem_ptr_index>> {
    using result =
        typename EvalBF_Impl<
            source,
            typename Pop<recorder>::result,
            Index<src_ptr_index + 1>,
            memory,
            output,
            Index<mem_ptr_index>
        >::result;
};
//----------------------------------------------------------------
template< typename source, typename recorder, size_t src_ptr_index, typename memory, typename output, size_t mem_ptr_index>
    requires(src_ptr_index <= source::size)
struct EvalBF_Impl< source, recorder, Index<src_ptr_index>, memory, output, Index<mem_ptr_index>>{
    using result = 
        typename EvalBF_ImplHelper<
            typename GetElementAt<source, Index<src_ptr_index>>::result,
            source,
            recorder,
            Index<src_ptr_index>,
            typename GetElementAt<memory, Index<mem_ptr_index>>::result,
            memory,
            output,
            Index<mem_ptr_index>
        >::result;
};
template< typename source, typename recorder, size_t src_ptr_index, typename memory, typename output, size_t mem_ptr_index>
    requires(src_ptr_index > source::size)
struct EvalBF_Impl< source, recorder, Index<src_ptr_index>, memory, output, Index<mem_ptr_index>>{
    using result = output;
};


template <typename str>
struct EvalBF {
    using result =
        typename EvalBF_Impl<
            typename StringToCharList<str>::result,
            List<>,
            Index<1>,
            List<Number<0>>,
            List<>,
            Index<1>
        >::result;
};


/*
template< typename source, typename recorder, typename cmdPtr, typename memory, typename output, typename memPtr>
constexpr auto EvalBF_Impl() -> {
    using curCmd = typename GetElementAt<source, cmdPtr>::result;
    if constexpr ( std::is_same_v< curCmd, Char<'+'> >) {
        EvalBF_Impl<source, recorder,>();
    }
}

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

/*
lisp:
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