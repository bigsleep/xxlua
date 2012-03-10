#include <iostream>
#include <functional>
#include <lua.hpp>

#include <xxlua/xxlua.hpp>

class MyClass
{
    int m_number;
public:
    MyClass(int a) : m_number(a)
    {
        std::cout << "construct: " << a << std::endl;
    }
    
    MyClass(MyClass const& other) : m_number(other.m_number)
    {
        std::cout << "copy construct: " << m_number << std::endl;
    }
    
    int number() const
    {
        std::cout << "MyClass::number: " << m_number << std::endl;
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
    try{
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        
        // def_class MyClass
        xxlua::def_class<MyClass>(L, "MyClass")
            .def("test", &MyClass::test)
            .def("new", xxlua::constructor<MyClass(int)>())
            .def("number", &MyClass::number)
            .def("other", &MyClass::other);
        
        std::string s =
            "testfunc = function(a) print(\"testfunction\") a:number() end\n"
            "x = MyClass:new(999)\n"
            "testfunc(x)\n";
        luaL_loadstring(L, s.c_str());
        
        lua_call(L, 0, 0);
        
        MyClass a(888);
        
        xxlua::call_function<void>(L, "testfunc", std::ref(a));
        xxlua::call_function<void>(L, "testfunc", a);
        lua_close(L);
    }
    catch(std::exception const& e){
        std::cout << e.what() << std::endl;
        throw;
    }
}

