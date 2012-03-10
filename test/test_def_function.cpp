#include <iostream>
#include <sstream>
#include <lua.hpp>

#include <xxlua/xxlua.hpp>

int func()
{
    return 999;
}

std::string hoge(const char* a, const char* b, const char* c)
{
    std::ostringstream ost;
    ost << a << " " << b << " " << c;
    return ost.str();
}

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    
    xxlua::def(L, "func", &func);
    xxlua::def(L, "hoge", &hoge);
    
    std::string s =
        "a = func()\n"
        "print(a)\n"
        "b = hoge(\"hello\", \"world\", \"!?!?!?\")"
        "print(b)";
    
    luaL_dostring(L, s.c_str());
    
    lua_close(L);
}


