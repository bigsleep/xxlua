#ifndef XXLUA_STACK_UTILITY_H
#define XXLUA_STACK_UTILITY_H

#include <iostream>
#include <type_traits>
#include <stdexcept>
#include <assert.h>
#include <lua.hpp>

#include <xxlua/type_traits.hpp>
#include <xxlua/mpl_utility.hpp>
#include <xxlua/class_definition.hpp>
#include <xxlua/destroy.hpp>

namespace xxlua
{
    extern void* enabler;
    //-----------------------------------------------------------
    //    push_stack
    //-----------------------------------------------------------
    template<typename T,
        typename std::enable_if<is_number<T>::value>::type *& S = enabler>
    void push_stack(lua_State* L, T a)
    {
        lua_pushnumber(L, static_cast<lua_Number>(a));
    }
    
    inline void push_stack(lua_State* L, bool a)
    {
        lua_pushboolean(L, a);
    }
    
    inline void push_stack(lua_State* L, char const* a)
    {
        lua_pushstring(L, a);
    }
    
    inline void push_stack(lua_State* L, std::string const& a)
    {
        lua_pushstring(L, a.c_str());
    }
    
    // other
    template<typename T,
        typename std::enable_if<!is_basic_type<typename std::decay<T>::type>::value>::type *& S = enabler>
    void push_stack(lua_State* L, T const& a)
    {
        typedef typename std::decay<T>::type user_type;
        static_assert(!is_reference_wrapper<user_type>::value, "xxlua: basic types cannot be pushed as reference");
        void* p = lua_newuserdata(L, sizeof(user_type));
        int data = lua_gettop(L);
        new (p) T(a);
        
        
        if(class_definition<user_type>::name.count(L)){
            luaL_getmetatable(L, class_definition<user_type>::name[L].c_str());
            if(lua_istable(L, lua_gettop(L))){
                lua_setmetatable(L, data);
                return;
            }
            lua_pop(L, 1);
        }
        lua_newtable(L);
        int t = lua_gettop(L);
        lua_pushliteral(L, "__gc");
        lua_pushcfunction(L, &destroy<user_type>);
        lua_settable(L, t);
        lua_setmetatable(L, data);
    }
    
    template<typename T,
        typename std::enable_if<!is_basic_type<T>::value>::type *& S = enabler>
    void push_stack(lua_State* L, std::reference_wrapper<T> const& a)
    {
        typedef typename std::remove_const<T>::type user_type;
        lua_pushlightuserdata(L, static_cast<void*>(const_cast<user_type*>(&(a.get()))));
        int data = lua_gettop(L);
        
        if(class_definition<user_type>::name.count(L)){
            luaL_getmetatable(L, class_definition<user_type>::name[L].c_str());
            if(lua_istable(L, lua_gettop(L))){
                lua_setmetatable(L, data);
                return;
            }
            lua_pop(L, 1);
        }
    }
    
    //-----------------------------------------------------------
    //    get_stack
    //-----------------------------------------------------------
    // boolean
    template<typename T,
        typename std::enable_if<is_boolean<T>::value>::type *& = enabler>
    bool get_stack(lua_State* L, int index)
    {
        assert(lua_isboolean(L, index));
        if(!lua_isboolean(L, index)) throw std::runtime_error("xxlua: bad argument");
        return static_cast<bool>(lua_tonumber(L, index));
    }
    
    // number
    template<typename T,
        typename std::enable_if<is_number<T>::value>::type *& = enabler>
    T get_stack(lua_State* L, int index)
    {
        assert(lua_isnumber(L, index));
        if(!lua_isnumber(L, index)) throw std::runtime_error("xxlua: bad argument");
        return static_cast<T>(lua_tonumber(L, index));
    }
    
    // string
    template<typename T,
        typename std::enable_if<is_string<T>::value>::type *& = enabler>
    char const* get_stack(lua_State* L, int index)
    {
        assert(lua_isstring(L, index));
        if(!lua_isstring(L, index)) throw std::runtime_error("xxlua: bad argument");
        return lua_tostring(L, index);
    }
    
    template<typename T,
        typename std::enable_if<!is_basic_type<T>::value>::type *& = enabler>
    T get_stack(lua_State* L, int index)
    {
        typedef typename std::remove_reference<T>::type type;
        auto a = static_cast<type*>(lua_touserdata(L, index));
        assert(a && lua_isuserdata(L, index));
        if(!(a && lua_isuserdata(L, index))) throw std::runtime_error("xxlua: bad argument");
        return *a;
    }
    
    template<typename ... A, std::size_t ... I>
    std::tuple<A...> get_stack(lua_State* L, mpl::vector_c<std::size_t, I...>)
    {
        return std::tuple<A...>(get_stack<A>(L, I)...);
    }
}
#endif

