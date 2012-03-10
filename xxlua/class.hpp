#ifndef XXLUA_CLASS_HPP
#define XXLUA_CLASS_HPP

#include <array>
#include <type_traits>
#include <stdexcept>
#include <assert.h>
#include <lua.hpp>

#include <xxlua/invoker.hpp>
#include <xxlua/constructor.hpp>
#include <xxlua/destroy.hpp>
#include <xxlua/class_definition.hpp>
#include <xxlua/tostring.hpp>

namespace xxlua{
    //-----------------------------------------------------------
    //    def_class
    //-----------------------------------------------------------
    template<typename C>
    class def_class
    {
    public:
        static_assert(!std::is_reference<C>::value, "");
        
        def_class(lua_State* L, std::string const& name) :
            m_lua(L), m_name(name)
        {
            class_definition<C>::name[L] = name;
            
            // new methodtable
            lua_newtable(L);
            int table = lua_gettop(L);
            
            // new metatable
            luaL_newmetatable(L, name.c_str());
            int metatable = lua_gettop(L);

            lua_pushliteral(L, "__metatable");
            lua_pushvalue(L, table);
            lua_settable(L, metatable);

            lua_pushliteral(L, "__index");
            lua_pushvalue(L, table);
            lua_settable(L, metatable);

            lua_pushliteral(L, "__tostring");
            lua_pushcfunction(L, &tostring<C>);
            lua_settable(L, metatable);

            lua_pushliteral(L, "__gc");
            lua_pushcfunction(L, &destroy<C>);
            lua_settable(L, metatable);
            
            // pop metatable and methodtable
            lua_pop(L, 2);
        }
        
        def_class(def_class const& a)
            : m_lua(a.m_lua), m_name(a.m_name)
        {}
        
        // member function
        template<typename F,
            typename std::enable_if<std::is_member_function_pointer<F>::value
                                   >::type *& S = enabler>
        def_class const& def(std::string const& mname, F mem) const
        {
            typedef typename std::remove_reference<F>::type member_type;
            typedef typename remove_function_quarifiers<typename member_signature<F>::type>::type signature;
            // make member function invoker
            auto f = make_invoker<signature, member_type,
                typename mpl::indexed_integral_sequence<std::size_t, 2, arity<signature>::value + 2>::type>();
            
            // get metatable
            luaL_getmetatable(m_lua, m_name.c_str());
            int metatable = lua_gettop(m_lua);
            
            // get method table
            lua_getfield(m_lua, metatable, "__index");
            int methodtable = lua_gettop(m_lua);
            
            // push name of member
            lua_pushstring(m_lua, mname.c_str());
            
            // push size 1 array pointer of member pointer
            auto a = static_cast<std::array<member_type, 1>*>(lua_newuserdata(m_lua, sizeof(std::array<member_type, 1>)));
            int data = lua_gettop(m_lua);
            (*a)[0] = mem;
            
            // push closure
            lua_pushcclosure(m_lua, f, 1);
            
            // set closure to methodtable
            lua_settable(m_lua, methodtable);
            
            // pop method table and metatable
            lua_pop(m_lua, 2);
            
            // for chain
            return *this;
        }
        
        // constructor
        template<typename ... A>
        def_class const& def(std::string const& name, constructor<C(A...)>) const
        {
            // global
            auto f = &constructor<C(typename adaptable_argument<A>::type...)>::apply;
            {
                lua_getglobal(m_lua, m_name.c_str());
                int a = lua_gettop(m_lua);
                
                if(!lua_istable(m_lua, a)){
                    lua_pop(m_lua, 1);
                    lua_newtable(m_lua);
                    a = lua_gettop(m_lua);
                }
                
                lua_pushstring(m_lua, name.c_str());
                lua_pushcfunction(m_lua, f);
                lua_settable(m_lua, a);
                
                lua_setglobal(m_lua, m_name.c_str());
            }
            
            // metatable
            {
                // get metatable
                luaL_getmetatable(m_lua, m_name.c_str());
                int metatable = lua_gettop(m_lua);
#if 0
                // push literal
                lua_pushliteral(m_lua, "__call");
                
                // push cfunction
                lua_pushcfunction(m_lua, f);
                
                // set cfunction to metatable
                lua_settable(m_lua, metatable);
#endif
                // get method table
                lua_getfield(m_lua, metatable, "__index");
                int methodtable = lua_gettop(m_lua);
                
                // set constructor to method table
                lua_pushstring(m_lua, name.c_str());
                lua_pushcfunction(m_lua, f);
                lua_settable(m_lua, methodtable);
                
                // pop method table and metatable
                lua_pop(m_lua, 2);
            }
            return *this;
        }
        
        // static method
        template<typename R, typename ... A>
        def_class const& def(std::string const& fname, R (*func)(A...)) const
        {
            auto f = make_invoker<R(A...), decltype(func),
                typename mpl::indexed_integral_sequence<std::size_t, 2, sizeof...(A) + 2>::type>();
            lua_getglobal(m_lua, m_name.c_str());
            int a = lua_gettop(m_lua);
            
            if(!lua_istable(m_lua, a)){
                lua_pop(m_lua, 1);
                lua_newtable(m_lua);
                a = lua_gettop(m_lua);
            }
            
            lua_pushstring(m_lua, fname.c_str());
            lua_pushcfunction(m_lua, reinterpret_cast<lua_CFunction>(func));
            lua_pushcclosure(m_lua, f, 1);
            lua_settable(m_lua, a);
            
            lua_setglobal(m_lua, m_name.c_str());
            return *this;
        }
        
    private:
        lua_State* m_lua;
        std::string const m_name;
    };
}
#endif
