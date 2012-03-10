#ifndef XXLUA_APPLY_ARGUMENT_H
#define XXLUA_APPLY_ARGUMENT_H

#include <tuple>
#include <utility>

#include <xxlua/mpl_utility.hpp>

namespace xxlua{
    extern void* enabler;
    //-----------------------------------------------------------
    //    apply
    //-----------------------------------------------------------
    template<typename R, typename F, typename ... A, std::size_t ... I>
    R apply_helper(F f, mpl::vector_c<std::size_t, I...>, std::tuple<A...>&& tup) 
    { 
        return f(std::forward<A>(std::get<I>(tup))... ); 
    }

    template<typename R, typename F, typename ... A>
    R apply_helper(F f, mpl::vector_c<std::size_t>, std::tuple<A...>&& tup) 
    { 
        return f(); 
    }
    
    template<typename R, typename F, typename ... A>
    R apply_argument(F f, const std::tuple<A...>&  tup)
    {
        return apply_helper<R>(f,
            typename mpl::indexed_integral_sequence<std::size_t, 0, sizeof...(A)>::type(),
            std::tuple<A...>(tup));
    }

    template<typename R, typename F, typename ... A>
    R apply_argument(F f, std::tuple<A...>&&  tup)
    {
        return apply_helper<R>(f,
            typename mpl::indexed_integral_sequence<std::size_t, 0, sizeof...(A)>::type(),
            std::forward<std::tuple<A...>>(tup));
    }
}
#endif

