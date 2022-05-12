#include <stdint.h>

#define LFS_LUA_5_4

#if defined(LFS_LUA_5_1)
extern "C" {
#include "../lua-5.1.5/src/lua.h"
#include "../lua-5.1.5/src/lualib.h"
#include "../lua-5.1.5/src/lauxlib.h"
};
#elif defined(LFS_LUA_5_2)
extern "C" {
#include "../lua-5.2.4/src/lua.h"
#include "../lua-5.2.4/src/lualib.h"
#include "../lua-5.2.4/src/lauxlib.h"
};
#elif defined(LFS_LUA_5_3)
extern "C" {
#include "../lua-5.3.6/src/lua.h"
#include "../lua-5.3.6/src/lualib.h"
#include "../lua-5.3.6/src/lauxlib.h"
}
#elif defined(LFS_LUA_5_4)
extern "C" {
#include "../lua-5.4.4/src/lua.h"
#include "../lua-5.4.4/src/lualib.h"
#include "../lua-5.4.4/src/lauxlib.h"
}
#elif defined(LFS_LUAU)

#elif defined(LFS_LUAJIT_200)

#endif