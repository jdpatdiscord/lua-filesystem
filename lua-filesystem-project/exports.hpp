#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <string>
#include <fstream>
#include <filesystem>
#include <locale>
#include <codecvt>

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

LFS_PLATFORM_EXPORT int lfs_writefile(uintptr_t lua_state);
LFS_PLATFORM_EXPORT int lfs_appendfile(uintptr_t lua_state);
LFS_PLATFORM_EXPORT int lfs_readfile(uintptr_t lua_state);
LFS_PLATFORM_EXPORT int lfs_delfile(uintptr_t lua_state);
LFS_PLATFORM_EXPORT int lfs_delfolder(uintptr_t lua_state);
LFS_PLATFORM_EXPORT int lfs_isfile(uintptr_t lua_state);
LFS_PLATFORM_EXPORT int lfs_isfolder(uintptr_t lua_state);
LFS_PLATFORM_EXPORT int lfs_loadfile(uintptr_t lua_state);
LFS_PLATFORM_EXPORT int lfs_dofile(uintptr_t lua_state);
LFS_PLATFORM_EXPORT int lfs_listfiles(uintptr_t lua_state);
LFS_PLATFORM_EXPORT int lfs_makefolder(uintptr_t lua_state);