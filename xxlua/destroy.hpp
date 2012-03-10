#ifndef XXLUA_DESTROY_HPP
#define XXLUA_DESTROY_HPP

#include <lua.hpp>

namespace xxlua{
    template<typename T>
    int destroy(lua_State* L)
    {
        T* p = static_cast<T*>(lua_touserdata(L, 1));
        p->~T();
        return 0;
    }
}
#endif

