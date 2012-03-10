#ifndef XXLUA_CALL_FUNCTION_HPP
#define XXLUA_CALL_FUNCTION_HPP

#include <type_traits>
#include <lua.hpp>

#include <xxlua/type_traits.hpp>
#include <xxlua/stack_utility.hpp>

namespace xxlua{
    extern void* enabler;
    
    template<typename R, typename ... A,
        typename std::enable_if<!std::is_same<R, void>::value>::type *& S = enabler>
    typename adaptable_result<R>::type
    call_function(lua_State* L, std::string const& name, A ... args)
    {
        typedef typename adaptable_result<R>::type result_type;
        // push function
        lua_getglobal(L, name.c_str());
        // push arguments
        auto i = {(push_stack(L, args), 0)...};
        // call function
        int bad = lua_pcall(L, sizeof...(A), 1, 0);
        if(bad)
            throw std::runtime_error("xxlua: error in call_function()");
        result_type r = get_stack<result_type>(L);
        return r;
    }
    
    template<typename R, typename ... A,
        typename std::enable_if<std::is_same<R, void>::value>::type *& S = enabler>
    void call_function(lua_State* L, std::string const& name, A ... args)
    {
        // push function
        lua_getglobal(L, name.c_str());
        // push arguments
        auto i = {(push_stack(L, args), 0)...};
        // call function
        int bad = lua_pcall(L, sizeof...(A), 0, 0);
        if(bad)
            throw std::runtime_error("xxlua: error in call_function()");
    }

}
#endif

