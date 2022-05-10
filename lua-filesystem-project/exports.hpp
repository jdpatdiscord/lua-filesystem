#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <string>
#include <fstream>
#include <filesystem>
#include <locale>
#include <codecvt>

#define LUA_5_1_5

#if defined(LUA_5_1_5)
extern "C" {
#include "lua-5.1.5/src/luaconf.h"
#include "lua-5.1.5/src/lua.h"
#include "lua-5.1.5/src/lualib.h"
#include "lua-5.1.5/src/lobject.h"
#include "lua-5.1.5/src/lauxlib.h"
};
#endif

#include "utilities.hpp"

#if defined(_WIN32)
#include <Windows.h>
#include <tchar.h>
#define LFS_PLATFORM_EXPORT __declspec(dllexport)
#elif defined(__linux__)
#define LFS_PLATFORM_EXPORT __attribute__((visibility("default")))
#elif defined(__APPLE__)
// Untested
#define LFS_PLATFORM_EXPORT __attribute__((visibility("default")))
#elif defined(__bsd__) || defined(__FreeBSD__) || defined(__NetBSD__)
// Untested
#define LFS_PLATFORM_EXPORT __attribute__((visibility("default")))
#else
#error "Nonsupported platform"
#endif

#define LFS_WORKSPACE_DIRECTORY "workspace"

struct PathValidationResult
{
	bool is_valid_path;
	std::filesystem::path path;
	std::filesystem::path working_directory;
};

LFS_PLATFORM_EXPORT int lfs_writefile(lua_State* lua_state);
LFS_PLATFORM_EXPORT int lfs_appendfile(lua_State* lua_state);
LFS_PLATFORM_EXPORT int lfs_readfile(lua_State* lua_state);
LFS_PLATFORM_EXPORT int lfs_delfile(lua_State* lua_state);
LFS_PLATFORM_EXPORT int lfs_delfolder(lua_State* lua_state);
LFS_PLATFORM_EXPORT int lfs_isfile(lua_State* lua_state);
LFS_PLATFORM_EXPORT int lfs_isfolder(lua_State* lua_state);
LFS_PLATFORM_EXPORT int lfs_loadfile(lua_State* lua_state);
LFS_PLATFORM_EXPORT int lfs_dofile(lua_State* lua_state);
LFS_PLATFORM_EXPORT int lfs_listfiles(lua_State* lua_state);
LFS_PLATFORM_EXPORT int lfs_makefolder(lua_State* lua_state);