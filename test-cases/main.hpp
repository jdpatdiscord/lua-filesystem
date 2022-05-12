#include "../lua-fs/include.hpp"

#if defined(LFS_LUA_5_1)
#if (INTPTR_MAX == INT32_MAX)
#pragma comment(lib, "../Win32/Release/lua-5.1.5.lib")
#elif (INTPTR_MAX == INT64_MAX)
#pragma comment(lib, "../x64/Release/lua-5.1.5.lib")
#endif
#elif defined(LFS_LUA_5_2)
#if (INTPTR_MAX == INT32_MAX)
#pragma comment(lib, "../Win32/Release/lua-5.2.4.lib")
#elif (INTPTR_MAX == INT64_MAX)
#pragma comment(lib, "../x64/Release/lua-5.2.4.lib")
#endif
#elif defined(LFS_LUA_5_3)
#if (INTPTR_MAX == INT32_MAX)
#pragma comment(lib, "../Win32/Release/lua-5.3.6.lib")
#elif (INTPTR_MAX == INT64_MAX)
#pragma comment(lib, "../x64/Release/lua-5.3.6.lib")
#endif
#elif defined(LFS_LUA_5_4)

#elif defined(LFS_LUAU)

#elif defined(LFS_LUAJIT_200)

#endif