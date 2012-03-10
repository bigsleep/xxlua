#include <iostream>
#include <sstream>
#include <array>
#include <iomanip>

#include <lua.hpp>
#include <xxlua/class.hpp>

class MyClass
{
    int m_number;
public:
    MyClass(int a) : m_number(a)
    {
        std::cout << "construct MyClass from " << a << std::endl;
    }
    
    int number() const
    {
        std::cout << "number: " << m_number << std::endl;
        return m_number;
    }
    
    static int test(int a, int b, int c)
    {
        std::cout << "test: " << a << ", " << b << ", " << c << std::endl;
        return a + b + c;
    }
    
    int other(MyClass& o) const
    {
        std::cout << "other: " << o.m_number << std::endl;
        return o.m_number;
    }
};

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    
    // define MyClass in lua
    xxlua::def_class<MyClass>(L, "MyClass")
        .def("test", &MyClass::test)    // define static function test
        .def("new", xxlua::constructor<MyClass(int)>())     // define constructor
        .def("number", &MyClass::number)             // define member function number
        .def("other", &MyClass::other);      // define member function other
    
    // lua script
    std::string s =
        "a = MyClass:new(999)\n"
        "print(a:number())\n"
        "print(a)\n"
        "b = MyClass:new(111)\n"
        "print(a:other(b))\n"
        "print('hello')\n";
    
    // execute lua script
    luaL_loadstring(L, s.c_str());
    lua_call(L, 0, 0);
    
    lua_close(L);
}
