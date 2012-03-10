#ifndef XXLUA_TYPE_TRAITS_HPP
#define XXLUA_TYPE_TRAITS_HPP

#include <type_traits>
#include <functional>
#include <string>

namespace xxlua{
    template<typename T>
    struct is_boolean : std::is_same<typename std::decay<T>::type, bool>
    {};
    
    template<typename T>
    struct is_number
    {
        static bool const value =
            !is_boolean<typename std::decay<T>::type>::value &&
            (std::is_integral<typename std::decay<T>::type>::value ||
             std::is_floating_point<typename std::decay<T>::type>::value);
    };
    
    template<typename T>
    struct is_string
    {
        static bool const value =
            std::is_same<typename std::decay<T>::type, std::string>::value ||
            std::is_same<T, char const*>::value;
    };
    
    template<typename T>
    struct is_basic_type
    {
        static bool const value = 
            is_boolean<typename std::decay<T>::type>::value ||
            is_number<typename std::decay<T>::type>::value ||
            is_string<typename std::decay<T>::type>::value;
    };
    
    template<typename T, typename S = void>
    struct adaptable_argument
    {
        typedef T type;
    };
    
    template<typename T>
    struct adaptable_argument<T,
        typename std::enable_if<
            is_boolean<typename std::decay<T>::type>::value ||
            is_number<typename std::decay<T>::type>::value ||
            is_string<typename std::decay<T>::type>::value>::type>
    {
        typedef typename std::decay<T>::type type;
    };
    
    template<typename T>
    struct adaptable_result
    {
        typedef typename std::remove_const<
            typename std::remove_reference<T>::type>::type type;
    };
    
    template<typename T>
    struct adaptable_function;
    
    template<typename R, typename ... A>
    struct adaptable_function<R(A...)>
    {
        typedef typename adaptable_result<R>::type type(typename adaptable_argument<A>::type...);
    };
    
    //-----------------------------------------------------------
    //    remove function cv: this needs to be fixed when ref-quaifiers are implemented
    //-----------------------------------------------------------
    template<typename F>
    struct remove_function_quarifiers;
    
    template<typename R, typename ... A>
    struct remove_function_quarifiers<R(A...)>
    {
        typedef R type(A...);
    };
    
    template<typename R, typename ... A>
    struct remove_function_quarifiers<R(A...) const> : remove_function_quarifiers<R(A...)> {};
    template<typename R, typename ... A>
    struct remove_function_quarifiers<R(A...) volatile> : remove_function_quarifiers<R(A...)> {};
    template<typename R, typename ... A>
    struct remove_function_quarifiers<R(A...) const volatile> : remove_function_quarifiers<R(A...)> {};
    
    //-----------------------------------------------------------
    //    class of member
    //-----------------------------------------------------------
    template<typename M>
    struct class_of_impl;
    
    template<typename Class, typename T>
    struct class_of_impl<T Class::*>
    {
        typedef Class type;
    };
    
    template<typename Member>
    struct class_of
    {
        typedef typename class_of_impl<typename std::remove_cv<Member>::type>::type type;
    };
    
    //-----------------------------------------------------------
    //    member_signature
    //-----------------------------------------------------------
    template<typename M>
    struct member_signature;
    
    template<typename Class, typename T>
    struct member_signature<T Class::*>
    {
        typedef T type;
    };
        
    //-----------------------------------------------------------
    //    is_reference_wrapper
    //-----------------------------------------------------------
    template<typename M>
    struct is_reference_wrapper : std::false_type
    {};
    
    template<typename T>
    struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type
    {};
    
    //-----------------------------------------------------------
    //    arity
    //-----------------------------------------------------------
    template<typename F, typename S = void>
    struct arity;
    
    template<typename R, typename ... A>
    struct arity<R(A...), void>
    {
        static std::size_t const value = sizeof...(A);
    };
    
    template<typename F>
    struct arity<F, typename std::enable_if<std::is_member_function_pointer<F>::value>::type>
        : arity<typename remove_function_quarifiers<typename std::remove_pointer<typename member_signature<F>::type>::type>::type, void> {};
}
#endif












