#ifndef DATASTRUCTURE_HPP
#define DATASTRUCTURE_HPP

#include "BasicTypes.hpp"
#include <concepts>
#include <type_traits>

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

#if defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
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