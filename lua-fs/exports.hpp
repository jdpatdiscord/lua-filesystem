#pragma once

#include "config.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <string>
#include <fstream>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <string>
#include <iomanip>
#include <clocale>
#include <cwchar>
#include <cuchar>
#include <unordered_map>
#include <map>

#if defined(_WIN32)
#include <Windows.h>
#include <tchar.h>
#define LFS_EXPORT extern "C" __declspec(dllexport)
#elif defined(__linux__)
#define LFS_EXPORT __attribute__((visibility("default")))
#elif defined(__APPLE__)
#define LFS_EXPORT __attribute__((visibility("default")))
#elif defined(__bsd__) || defined(__FreeBSD__) || defined(__NetBSD__)
#define LFS_EXPORT __attribute__((visibility("default")))
#else
#error "Nonsupported platform"
#endif

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

LFS_EXPORT int lfs_writefile(lua_State* lua_state);
LFS_EXPORT int lfs_appendfile(lua_State* lua_state);
LFS_EXPORT int lfs_readfile(lua_State* lua_state);
LFS_EXPORT int lfs_delfile(lua_State* lua_state);
LFS_EXPORT int lfs_delfolder(lua_State* lua_state);
LFS_EXPORT int lfs_isfile(lua_State* lua_state);
LFS_EXPORT int lfs_isfolder(lua_State* lua_state);
LFS_EXPORT int lfs_loadfile(lua_State* lua_state);
LFS_EXPORT int lfs_dofile(lua_State* lua_state);
LFS_EXPORT int lfs_listfiles(lua_State* lua_state);
LFS_EXPORT int lfs_makefolder(lua_State* lua_state);
LFS_EXPORT int luaopen_lfs(lua_State* lua_state);

#define LFS_WORKSPACE_DIRECTORY "workspace"

struct lfs_validation
{
	bool is_valid_path;
	std::filesystem::path path;
	std::filesystem::path working_directory;
};