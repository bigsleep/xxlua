#ifndef XXLUA_CLASS_DEFINITION_HPP
#define XXLUA_CLASS_DEFINITION_HPP

#include <string>
#include <map>
#include <lua.hpp>

namespace xxlua{
    //-----------------------------------------------------------
    //    
    //-----------------------------------------------------------
    template<typename T>
    class class_definition
    {
    public:
        static std::map<lua_State*, std::string> name;
    };
    
    template<typename T>
    std::map<lua_State*, std::string> class_definition<T>::name;
}
#endif

