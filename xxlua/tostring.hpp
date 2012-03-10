#ifndef XXLUA_TOSTRING_HPP
#define XXLUA_TOSTRING_HPP

#include <string>
#include <lua.hpp>

#include <xxlua/class_definition.hpp>

namespace xxlua{
    template<typename T>
    int tostring(lua_State* L)
    {
        std::string name("");
        if(class_definition<typename std::decay<T>::type>::name.count(L))
            name = class_definition<typename std::decay<T>::type>::name[L];
        lua_pushstring(L, name.c_str());
        return 1;
    }
}
#endif

