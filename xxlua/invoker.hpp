#ifndef XXLUA_INVOKER_H
#define XXLUA_INVOKER_H

#include <iostream>
#include <tuple>
#include <utility>
#include <functional>
#include <stdexcept>
#include <cstddef>
#include <assert.h>

#include <lua.hpp>

#include <xxlua/mpl_utility.hpp>
#include <xxlua/stack_utility.hpp>


namespace xxlua{
    extern void* enabler;
    //-----------------------------------------------------------
    //    max index
    //-----------------------------------------------------------
    template<std::size_t ... I>
    struct max_index;
    
    template<std::size_t I0, std::size_t ... I>
    struct max_index<I0, I...>
    {
        static std::size_t const value = mpl::max<mpl::vector_c<int, I0, I...>>::value;
    };
    
    template<>
    struct max_index<>
    {
        static std::size_t const value = 0;
    };
    
    //-----------------------------------------------------------
    //    invoker
    //-----------------------------------------------------------
    template<typename Sig, typename F, typename Index, typename S = void>
    struct invoker;

    //-----------------------------------------------------------
    //    function pointer specialization
    //-----------------------------------------------------------
    template<typename R1, typename ... A1, typename R2, typename ... A2, std::size_t ... I>
    struct invoker<R1(A1...), R2(*)(A2...), mpl::vector_c<std::size_t, I...>>
    {
        static int apply(lua_State* L)
        {
            typedef mpl::vector_c<std::size_t, I...> index;
            try{
                // check argument number
                static int const max = max_index<I...>::value;
                int top = lua_gettop(L);
                assert(top >= max);
                if(top < max) throw std::runtime_error("xxlua: bad argument number");
                
                auto f = reinterpret_cast<R2(*)(A2...)>(lua_tocfunction(L, lua_upvalueindex(1)));
                R1 r = f(get_stack<A1>(L, I)...);
                push_stack(L, r);
            }
            catch(std::exception const& e){
                lua_pushstring(L, e.what());
                return lua_error(L);
            }
            catch(...){
                lua_pushstring(L, "xxlua: unknown error");
                return lua_error(L);
            }
            return 1;
        }
    };

    template<typename ... A1, typename R2, typename ... A2, std::size_t ... I>
    struct invoker<void(A1...), R2(*)(A2...), mpl::vector_c<std::size_t, I...>>
    {
        static int apply(lua_State* L)
        {
            try{
                // check argument number
                static int const max = max_index<I...>::value;
                int top = lua_gettop(L);
                assert(top >= max);
                if(top < max) throw std::runtime_error("xxlua: bad argument number");
                
                auto f = reinterpret_cast<R2(*)(A2...)>(lua_tocfunction(L, lua_upvalueindex(1)));
                f(get_stack<A1>(L, I)...);
            }
            catch(std::exception const& e){
                lua_pushstring(L, e.what());
                return lua_error(L);
            }
            catch(...){
                lua_pushstring(L, "xxlua: unknown error");
                return lua_error(L);
            }
            return 0;
        }
    };
    
    //-----------------------------------------------------------
    //    member function pointer specialization
    //-----------------------------------------------------------
    template<typename R1, typename ... A1, typename F, std::size_t ... I>
    struct invoker<R1(A1...), F, mpl::vector_c<std::size_t, I...>, typename std::enable_if<std::is_member_function_pointer<F>::value>::type>
    {
        static int apply(lua_State* L)
        {
            typedef typename class_of<F>::type Class;
            try{
                // check argument number
                static int const max = max_index<I...>::value;
                int top = lua_gettop(L);
                assert(top >= max);
                if(top < max) throw std::runtime_error("xxlua: bad argument number");
                
                auto p = static_cast<std::array<F, 1>*>(lua_touserdata(L, lua_upvalueindex(1)));
                F memfn = (*p)[0];
                auto self = static_cast<Class*>(lua_touserdata(L, 1));
                R1 r = (self->*memfn)(get_stack<A1>(L, I)...);
                push_stack(L, r);
            }
            catch(std::exception const& e){
                lua_pushstring(L, e.what());
                return lua_error(L);
            }
            catch(...){
                lua_pushstring(L, "xxlua: unknown error");
                return lua_error(L);
            }
            return 1;
        }
    };
    
    template<typename ... A1, typename F, std::size_t ... I>
    struct invoker<void(A1...), F, mpl::vector_c<std::size_t, I...>, typename std::enable_if<std::is_member_function_pointer<F>::value>::type>
    {
        static int apply(lua_State* L)
        {
            typedef typename class_of<F>::type Class;
            try{
                // check argument number
                static int const max = max_index<I...>::value;
                int top = lua_gettop(L);
                assert(top >= max);
                if(top < max) throw std::runtime_error("xxlua: bad argument number");
                
                auto p = static_cast<std::array<F, 1>*>(lua_touserdata(L, lua_upvalueindex(1)));
                F memfn = (*p)[0];
                auto self = static_cast<Class*>(lua_touserdata(L, 1));
                (self->*memfn)(get_stack<A1>(L, I)...);
            }
            catch(std::exception const& e){
                lua_pushstring(L, e.what());
                return lua_error(L);
            }
            catch(...){
                lua_pushstring(L, "xxlua: unknown error");
                return lua_error(L);
            }
            return 0;
        }
    };
    
    template<typename S, typename F, typename Index>
    lua_CFunction make_invoker()
    {
        lua_CFunction f = &invoker<typename adaptable_function<S>::type, F, Index>::apply;
        return f;
    }
}
#endif

