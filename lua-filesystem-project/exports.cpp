#include "exports.hpp"

std::wstring platform_agnostic_findme()
{
	#if defined(_WIN32)
		WCHAR WorkingPath[MAX_PATH];
		GetModuleFileNameW(NULL, WorkingPath, sizeof(WorkingPath) / sizeof(*WorkingPath));
		return std::wstring(WorkingPath, sizeof(WorkingPath) / sizeof(*WorkingPath));
	#elif defined(__linux__)
		
	#elif defined(__APPLE__)

	#elif defined(__bsd__) || defined(__FreeBSD__) || defined(__NetBSD__)

	#else
		#error "Nonsupported platform"
	#endif
}

PathValidationResult validate_path(const char* user_path, bool allow_same_path)
{
	std::wstring application_working_directory;
	
	if (application_working_directory.empty())
	{
		application_working_directory = platform_agnostic_findme();
	}

	std::filesystem::path working_directory = std::filesystem::path(application_working_directory).parent_path();
	working_directory += L"\\" LFS_WORKSPACE_DIRECTORY;
	std::filesystem::path user_defined_path = working_directory / utf8_to_utf16(std::string(user_path));
	
	if (!std::filesystem::is_directory(working_directory))
	{

	}
}

int lfs_writefile(uintptr_t lua_state)
{
	return 0;
};

int lfs_appendfile(uintptr_t lua_state)
{
	return 0;
};

int lfs_readfile(uintptr_t lua_state)
{
	return 1; // string?
};

int lfs_delfile(uintptr_t lua_state)
{
	return 0;
};

int lfs_delfolder(uintptr_t lua_state)
{
	return 0;
};

int lfs_isfile(uintptr_t lua_state)
{
	return 0; // bool
};

int lfs_isfolder(uintptr_t lua_state)
{
	return 0; // bool
};

int lfs_loadfile(uintptr_t lua_state)
{
	// Implementation defined
	return 0; // function?
};

int lfs_dofile(uintptr_t lua_state)
{
	// Implementation defined
	return 0;
};

int lfs_listfiles(uintptr_t lua_state)
{
	return 1; // table{key=number,value=string}
};

int lfs_makefolder(uintptr_t lua_state)
{
	return 0;
};

int main()
{
	std::string utf8_test = (const char*)u8"コンピューター翻訳テスト";

	std::wstring utf16_test = utf8_to_utf16(utf8_test);

	wprintf(L"ya mom or something: %s\n", utf16_test.c_str());

	return 0;
}