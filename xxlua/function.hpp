#ifndef XXLUA_FUNCTION_HPP
#define XXLUA_FUNCTION_HPP

#include <string>
#include <lua.hpp>

#include <xxlua/invoker.hpp>

namespace xxlua{
    extern void* enabler;
    
    template<typename R, typename ... A>
    void def(lua_State* L, std::string const& name, R (*f)(A...))
    {
        auto invoker = make_invoker<R(A...), decltype(f),
            typename mpl::indexed_integral_sequence<std::size_t, 1, sizeof...(A) + 1>::type>();
        lua_pushcfunction(L, reinterpret_cast<lua_CFunction>(f));
        lua_pushcclosure(L, invoker, 1);
        lua_setglobal(L, name.c_str());
    }
}
#endif


