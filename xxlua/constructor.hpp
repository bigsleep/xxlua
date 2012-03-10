#ifndef XXLUA_CONSTRUCTOR_HPP
#define XXLUA_CONSTRUCTOR_HPP

#include <utility>
#include <tuple>
#include <type_traits>
#include <stdexcept>
#include <assert.h>

#include <lua.hpp>

#include <xxlua/stack_utility.hpp>
#include <xxlua/mpl_utility.hpp>

namespace xxlua{
    template<typename Class, typename ... A, std::size_t ... I>
    void* construct_from_tuple(void* p, mpl::vector_c<std::size_t, I...>, std::tuple<A...>& args)
    {
        return new (p) Class(std::get<I>(args)...);
    }
    
    template<typename T>
    struct constructor;
    
    template<typename Class, typename ... A>
    struct constructor<Class(A...)>
    {
        static int apply(lua_State* L)
        {
            try{
                // check argument number
                int top = lua_gettop(L);
                int max = sizeof...(A) + 1;
                assert(top >= max);
                if(top < max) throw std::runtime_error("xxlua: bad argument number");
                
                // allocate user data
                void* p = lua_newuserdata(L, sizeof(Class));
                int data = lua_gettop(L);
                // index of lua start from 1 and bottom of stack is not used
                typedef typename mpl::indexed_integral_sequence<std::size_t, 2, (sizeof...(A) + 2)>::type index_type;
                auto args = get_stack<A...>(L, index_type());
                // construct
                construct_from_tuple<Class>(p, typename mpl::indexed_integral_sequence<std::size_t, 0, sizeof...(A)>::type(), args);
                
                // set metatable
                if(class_definition<Class>::name.count(L)){
                    luaL_getmetatable(L, class_definition<Class>::name[L].c_str());
                    if(lua_istable(L, lua_gettop(L))){
                        lua_setmetatable(L, data);
                    }
                }
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
}
#endif

