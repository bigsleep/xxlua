//-----------------------------------------------------------
//    mpl_utility
//-----------------------------------------------------------
#ifndef MPL_UTILITY_H
#define MPL_UTILITY_H

#include <cstddef>
#include <array>
#include <tuple>
#include <type_traits>

namespace xxlua{
namespace mpl{
    template<typename ... Tp>
    struct vector
    {};
    
    template<typename Tp, Tp ... N>
    struct vector_c
    {};
    
    template<typename ... A>
    void expand(A ... ){}
    
    //-----------------------------------------------------------
    //    is_type_sequence, is_integral_sequence
    //-----------------------------------------------------------
    template<typename Vec>
    struct is_type_sequence
    {
        static bool const value = false;
    };
    
    template<template<typename ... T> class Vector, typename ... Tp>
    struct is_type_sequence<Vector<Tp...>>
    {
        static bool const value = true;
    };
    
    template<typename Vec>
    struct is_integral_sequence
    {
        static bool const value = false;
    };
    
    template<template<typename T, T ... Val> class VectorC, typename Tp, Tp ... N>
    struct is_integral_sequence<VectorC<Tp, N...>>
    {
        static bool const value = true;
    };
    
    //-----------------------------------------------------------
    //    size
    //-----------------------------------------------------------
    template<typename Vec>
    struct size;
    
    template<template<typename T, T ... Val> class VectorC, typename Tp, Tp ... N>
    struct size<VectorC<Tp, N...>>
    {
        static std::size_t const value = sizeof...(N);
    };
    
    template<template<typename ... T> class Vector, typename ... Tp>
    struct size<Vector<Tp...>>
    {
        static std::size_t const value = sizeof...(Tp);
    };
    
    //-----------------------------------------------------------
    //    at
    //-----------------------------------------------------------
    template<std::size_t N, typename Vec>
    struct at;
    
    template<std::size_t N, template<typename ... T> class Vec,
        typename Head, typename ... Tail>
    struct at<N, Vec<Head, Tail...>>
    {
        typedef typename at<N - 1, Vec<Tail...>>::type type;
    };
    
    template<template<typename ... T> class Vec,
        typename Head, typename ... Tail>
    struct at<0, Vec<Head, Tail...>>
    {
        typedef Head type;
    };
    
    template<std::size_t N, template<typename T, T ... Val> class VecC,
        typename Tp, Tp Head, Tp ... Tail>
    struct at<N, VecC<Tp, Head, Tail...>>
    {
        static Tp const value = at<N - 1, VecC<Tp, Tail...>>::value;
        typedef std::integral_constant<Tp, value> type;
    };
    
    template<template<typename T, T ... Val> class VecC,
        typename Tp, Tp Head, Tp ... Tail>
    struct at<0, VecC<Tp, Head, Tail...>>
    {
        static Tp const value = Head;
        typedef std::integral_constant<Tp, Head> type;
    };
    
    //-----------------------------------------------------------
    //    clear
    //-----------------------------------------------------------
    template<typename Vec>
    struct clear;
    
    template<template<typename ... T> class Vec, typename ... Tp>
    struct clear<Vec<Tp...>>
    {
        typedef Vec<> type;
    };
    
    template<template<typename T, T ... Val> class VecC, typename Tp, Tp ... N>
    struct clear<VecC<Tp, N...>>
    {
        typedef VecC<Tp> type;
    };
    
    //-----------------------------------------------------------
    //    pop_front
    //-----------------------------------------------------------
    template<typename Vec>
    struct pop_front;
    
    
    template<template<typename ... T> class Vec, typename Head, typename ... Tail>
    struct pop_front<Vec<Head, Tail...>>
    {
        typedef Vec<Tail...> type;
    };
    
    template<template<typename T, T ... Val> class VecC, typename Tp, Tp Head, Tp ... Tail>
    struct pop_front<VecC<Tp, Head, Tail...>>
    {
        typedef VecC<Tp, Tail...> type;
    };
    
    //-----------------------------------------------------------
    //    push_front
    //-----------------------------------------------------------
    template<typename Vec, typename Tp>
    struct push_front;
    
    template<template<typename ... T> class Vec,
        typename Head, typename ... Tail>
    struct push_front<Vec<Tail...>, Head>
    {
        typedef Vec<Head, Tail...> type;
    };
    
    template<template<typename T, T ... Val> class VecC,
        template<typename T, T Val> class IntC,
        typename Tp, Tp Head, Tp ... Tail>
    struct push_front<VecC<Tp, Tail...>, IntC<Tp, Head>>
    {
        typedef VecC<Tp, Head, Tail...> type;
    };
    
    //-----------------------------------------------------------
    //    push_back
    //-----------------------------------------------------------
    template<typename Vec, typename Tp>
    struct push_back;
    
    template<template<typename ... T> class Vec,
        typename ... Head, typename Tail>
    struct push_back<Vec<Head...>, Tail>
    {
        typedef Vec<Head..., Tail> type;
    };
    
    template<template<typename T, T ... Val> class VecC,
        template<typename T, T Val> class IntC,
        typename Tp, Tp ... Head, Tp Tail>
    struct push_back<VecC<Tp, Head...>, IntC<Tp, Tail>>
    {
        typedef VecC<Tp, Head..., Tail> type;
    };
    
    //-----------------------------------------------------------
    //    cut_in_range
    //-----------------------------------------------------------
    template<std::size_t It, std::size_t End, typename Vec>
    struct cut_in_range;
    
    template<std::size_t It, std::size_t End, typename Vec, typename Result>
    struct _cut_in_range_helper;
    
    template<std::size_t Begin, std::size_t End, typename Vec>
    struct cut_in_range
    {
        static_assert(
            Begin <= End && End <= size<Vec>::value,
            "sykes::mpl::cut_in_range");
        
        typedef typename
            _cut_in_range_helper<Begin, End, Vec,
                typename clear<Vec>::type>::type type;
    };
    
    template<std::size_t It, std::size_t End, typename Vec, typename Result>
    struct _cut_in_range_helper
    {
        typedef typename
            _cut_in_range_helper<
                It + 1, End,
                Vec,
                typename push_back<
                    Result,
                    typename at<It, Vec>::type
                >::type
            >::type type;
    };
    
    template<std::size_t End, typename Vec, typename Result>
    struct _cut_in_range_helper<End, End, Vec, Result>
    {
        typedef Result type;
    };
    
    //-----------------------------------------------------------
    //    pop_back
    //-----------------------------------------------------------
    template<typename Vec>
    struct pop_back
    {
        typedef typename
            cut_in_range<
                0, size<Vec>::value - 1, Vec
            >::type type;
    };
    
    //-----------------------------------------------------------
    //    joint
    //-----------------------------------------------------------
    template<typename Vec1, typename Vec2>
    struct joint;
    
    template<template<typename ... T> class Vec1,
        template<typename ... T> class Vec2, typename ... T1, typename ... T2>
    struct joint<Vec1<T1...>, Vec2<T2...>>
    {
        typedef Vec1<T1..., T2...> type;
    };
    
    template<template<typename T, T ... Val> class VecC1,
        template<typename T, T ... Val> class VecC2, typename Tp, Tp ... N1, Tp ... N2>
    struct joint<VecC1<Tp, N1...>, VecC2<Tp, N2...>>
    {
        typedef VecC1<Tp, N1..., N2...> type;
    };

    //-----------------------------------------------------------
    //    convert sequence
    //-----------------------------------------------------------
    template<typename V1, template<typename ... T> class SeqType>
    struct convert_type_sequence;
    
    template<template<typename ... T> class Vec,
        template<typename ... T> class SeqType, typename ... T1>
    struct convert_type_sequence<Vec<T1...>, SeqType>
    {
        typedef SeqType<T1...> type;
    };
    
    template<typename V1, template<typename T, T ... V> class IntSeqType>
    struct convert_integral_sequence;
    
    template<template<typename T, T ... Val> class VecC1,
        template<typename T, T ... Val> class IntSeqType, typename Tp, Tp ... N1>
    struct convert_integral_sequence<VecC1<Tp, N1...>, IntSeqType>
    {
        typedef IntSeqType<Tp, N1...> type;
    };

    //-----------------------------------------------------------
    //    replace_at
    //-----------------------------------------------------------
    template<std::size_t N1, typename Vec, typename T>
    struct replace_at;
    
    template<std::size_t N1, typename Vec, typename Tr>
    struct replace_at
    {
        static_assert(
            N1 < size<Vec>::value,
            "struct sykes::mpl::replace_at");
        
        typedef typename
            joint<
                typename push_back<typename cut_in_range<0, N1, Vec>::type, Tr>::type,
                typename cut_in_range<
                    N1 + 1, size<Vec>::value, Vec
                >::type
            >::type type;
    };
    
    //-----------------------------------------------------------
    //    erase_at
    //-----------------------------------------------------------
    template<std::size_t N1, typename Vec>
    struct erase_at
    {
        static_assert(
            N1 < size<Vec>::value,
            "struct sykes::mpl::erase_at");
        
        typedef typename
            joint<
                typename cut_in_range<0, N1, Vec>::type,
                typename cut_in_range<
                    N1 + 1, size<Vec>::value, Vec
                >::type
            >::type type;
    };
    
    //-----------------------------------------------------------
    //    increment_at
    //-----------------------------------------------------------
    template<std::size_t N, typename VecC>
    struct increment_at
    {
        static_assert(
            (N < (size<VecC>::value)),
            "struct sykes::mpl::increment_at");
        
        typedef typename
            replace_at<
                N, VecC,
                std::integral_constant<
                    typename VecC::value_type,
                    (at<N, VecC>::value + 1)
                >
            >::type type;
    };
    
    //-----------------------------------------------------------
    //    to_tuple
    //-----------------------------------------------------------
    template<std::size_t N, typename Tp>
    struct tupleN;

    template<typename Tp>
    struct tupleN<0, Tp>
    {
        typedef std::tuple<> type;
    };
    
    template<typename Tp>
    struct tupleN<1, Tp>
    {
        typedef std::tuple<Tp> type;
    };

    template<std::size_t N, typename Tp>
    struct tupleN
    {
        typedef typename
            push_front<
                typename tupleN<N - 1, Tp>::type, Tp
            >::type type;
    };
    
    template<typename Vec>
    struct to_tuple;
    
    template<template<typename T, T ... V> class VecC, typename Tp, Tp ... N>
    struct to_tuple<VecC<Tp, N...>>
    {
        typedef typename tupleN<
                    sizeof...(N),
                    Tp
                >::type tuple_type;
        
        static tuple_type get()
        {
            return tuple_type(N...);
        }
    };
    
    //-----------------------------------------------------------
    //    to_array
    //-----------------------------------------------------------
    template<typename Vec>
    struct to_array;
    
    template<template<typename T, T ... V> class VecC, typename Tp, Tp ... N>
    struct to_array<VecC<Tp, N...>>
    {
        typedef std::array<Tp, sizeof...(N)> array_type;
        
        static array_type get()
        {
            return array_type{{N...}};
        }
    };
    
    //-----------------------------------------------------------
    //    is_sorted
    //-----------------------------------------------------------
    template<typename VecC>
    struct is_sorted;
    
    template<template<typename T, T ... V> class VecC,
        typename Tp, Tp N1, Tp N2, Tp ... N>
    struct is_sorted<VecC<Tp, N1, N2, N...>>
    {
        static bool const value = (N1 <= N2)
            && is_sorted<VecC<Tp, N2, N...>>::value;
    };
    
    template<template<typename T, T ... V> class VecC,
        typename Tp, Tp N1>
    struct is_sorted<VecC<Tp, N1>>
    {
        static bool const value = true;
    };
    
    template<template<typename T, T ... V> class VecC, typename Tp>
    struct is_sorted<VecC<Tp>>
    {
        static bool const value = true;
    };
    
    //-----------------------------------------------------------
    //    max
    //-----------------------------------------------------------
    template<typename VecC>
    struct max;
    
    template<template<typename T, T ... V> class VecC,
        typename Tp, Tp N1, Tp N2, Tp ... N>
    struct max<VecC<Tp, N1, N2, N...>>
    {
        static Tp const value
            = (N1 >= max<VecC<Tp, N2, N...>>::value) ?
                N1 : max<VecC<Tp, N2, N...>>::value;
        
        static std::size_t const position
            = (N1 >= max<VecC<Tp, N2, N...>>::value) ?
                0 : 1 + max<VecC<Tp, N2, N...>>::position;
    };
    
    template<template<typename T, T ... V> class VecC,
        typename Tp, Tp N>
    struct max<VecC<Tp, N>>
    {
        static Tp const value = N;
        static std::size_t const position = 0;
        typedef std::integral_constant<Tp, N> type;
    };
    
    //-----------------------------------------------------------
    //    min
    //-----------------------------------------------------------
    template<typename Vec>
    struct min;
    
    template<template<typename T, T ... V> class VecC,
        typename Tp, Tp N1, Tp N2, Tp ... N>
    struct min<VecC<Tp, N1, N2, N...>>
    {
        static Tp const value
            = (N1 <= min<VecC<Tp, N2, N...>>::value) ?
                N1 : min<VecC<Tp, N2, N...>>::value;
        
        static std::size_t const position
            = (N1 <= min<VecC<Tp, N2, N...>>::value) ?
                0 : 1 + min<VecC<Tp, N2, N...>>::position;
    };
    
    template<template<typename T, T ... V> class VecC,
        typename Tp, Tp N>
    struct min<VecC<Tp, N>>
    {
        static Tp const value = N;
        static std::size_t const position = 0;
        typedef std::integral_constant<Tp, N> type;
    };
    
    //-----------------------------------------------------------
    //    sort
    //-----------------------------------------------------------
    template<typename VecC>
    struct sort;
    
    template<template<typename T, T ... V> class VecC,
        typename Tp, Tp N1, Tp N2, Tp ... N>
    struct sort<VecC<Tp, N1, N2, N...>>
    {
        typedef typename
            push_front<
                typename sort<
                    typename
                    erase_at<
                        min<VecC<Tp, N1, N2, N...>>::position,
                        VecC<Tp, N1, N2, N...>
                    >::type
                >::type,
                typename min<VecC<Tp, N1, N2, N...>>::type
            >::type type;
    };
    
    template<template<typename T, T ... V> class VecC,
        typename Tp, Tp N>
    struct sort<VecC<Tp, N>>
    {
        typedef VecC<Tp, N> type;
    };
    
    //-----------------------------------------------------------
    //    reverse
    //-----------------------------------------------------------
    template<typename Vec>
    struct reverse;
    
    template<std::size_t N, std::size_t End, typename Vec1, typename Vec2>
    struct _reverse_helper;
    
    template<typename Vec>
    struct reverse
    {
        typedef typename
            _reverse_helper<
                0,
                size<Vec>::value,
                Vec,
                typename clear<Vec>::type
            >::type type;
    };
    
    template<std::size_t N, std::size_t End, typename Vec1, typename Vec2>
    struct _reverse_helper
    {
        typedef typename
            _reverse_helper<
                N + 1,
                End,
                Vec1,
                typename push_front<
                    Vec2,
                    typename at<N, Vec1>::type
                >::type
            >::type type;
    };
    
    template<std::size_t N, typename Vec1, typename Vec2>
    struct _reverse_helper<N, N, Vec1, Vec2>
    {
        typedef Vec2 type;
    };
    
    //-----------------------------------------------------------
    //    indexed
    //-----------------------------------------------------------
    template<typename T, T First, T Last>
    struct make_indexed_integral_sequence;
    
    template<typename T, T First, T Last>
    struct make_indexed_integral_sequence
    {
        typedef typename
            push_back<typename make_indexed_integral_sequence<T, First, Last - 1>::type,
                std::integral_constant<T, Last - 1>>::type type;
    };
    
    template<typename T, T A>
    struct make_indexed_integral_sequence<T, A, A>
    {
        typedef vector_c<T> type;
    };
    
    template<typename T, T First, T Last>
    struct indexed_integral_sequence
    {
        static_assert(First <= Last, "");
        typedef typename make_indexed_integral_sequence<T, First, Last>::type type;
    };
}}//----


#endif








