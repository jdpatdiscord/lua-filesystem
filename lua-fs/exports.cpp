#include "exports.hpp"

std::wstring utf8_to_utf16(const std::string& utf8_string)
{
	std::string cached_locale = std::locale().name();

	setlocale(LC_ALL, "en_US.utf8");

	std::wstring result_string;
	const char* temp_str_ptr = utf8_string.c_str();
	const char* temp_str_end = utf8_string.c_str() + utf8_string.size();

	std::mbstate_t state = std::mbstate_t();

	int result;
	char16_t result_char;
	while (result = mbrtoc16(&result_char, temp_str_ptr, temp_str_end - temp_str_ptr + 1, &state))
	{
		if (result == -3) // a new character was generated but the pointer doesn't need to advance
		{
			result_string += result_char;
		}
		else if (result == -2) // the next characters are valid but incomplete, just silent fail
		{
			break;
		}
		else if (result == -1) // there was a definite error, just silent fail
		{
			break;
		}
		else if (result >= 0) // continue or stop if == 0
		{
			result_string += result_char;
			// result returns a count of successfully converted characters in this case:
			if (result == 0) [[unlikely]]
				break;
			temp_str_ptr += result;
		}
	}

	setlocale(LC_ALL, cached_locale.c_str());

	return result_string;
}

std::string utf16_to_utf8(const std::wstring& utf16_string)
{
	std::string cached_locale = std::locale().name();
	setlocale(LC_ALL, "en_US.utf8");

	std::string result_string;
	std::mbstate_t state = std::mbstate_t();

	char ccache[MB_LEN_MAX] = { 0 };
	for (char16_t u16c : utf16_string)
	{
		auto result = c16rtomb(ccache, u16c, &state);
		if (result == -1) [[unlikely]]
			break;
		else
			result_string += ccache;
	}

	setlocale(LC_ALL, cached_locale.c_str());
	return result_string;
}

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

std::wstring application_working_directory;

lfs_validation validate_path(const char* user_path)
{
	if (application_working_directory.empty())
	{
		application_working_directory = platform_agnostic_findme();
	}

	std::filesystem::path working_directory = std::filesystem::path(application_working_directory).parent_path();
	working_directory += L"\\" LFS_WORKSPACE_DIRECTORY;
	std::filesystem::path user_defined_path = working_directory / utf8_to_utf16(std::string(user_path));

	if (!std::filesystem::is_directory(working_directory))
	{
		if (std::filesystem::is_regular_file(working_directory)) // band aid
		{
			working_directory = working_directory.parent_path();
		}
		std::filesystem::create_directory(working_directory);
	}

	const std::vector<std::wstring> blacklisted_extensions = {
		L".ace", L".ade", L".adp", L".ani", L".app", L".appcontent", L".appref", L".asp", L".aspx", L".asx",
		L".bas", L".bat",
		L".cdxml", L".cer", L".chm", L".cmd", L".cnt", L".com", L".cpl", L".crt", L".csh",
		L".der", L".diagcab", L".docm",
		L".doc", /* .doc format has RCE exploits */
		L".exe",
		L".fxp",
		L".gadget", L".grp",
		L".hlp", L".hpj", L".hta", L".htc",
		L".inf", L".ins", L".isp", L".its",
		L".jar", L".jnlp", L".js", L".jse",
		L".ksh",
		L".xls", L".xlsm", /* spreadsheets can contain macros and easily social engineer people */
		L".lnk",
		L".mad", L".maf", L".mag", L".mam", L".maq", L".mar", L".mas", L".mat", L".mau", L".mav", L".maw", L".mcf", L".mda", L".mdb", L".mde", L".mdt", L".mdw", L".mdz", L".mht", L".mhtml", L".ms", L".msc", L".msh", L".msh1", L".msh1xml", L".msh2", L".msh2xml", L".mshxml", L".msi", L".msp", L".mst", L".msu",
		L".ops", L".osd",
		L".pcd", L".pif", L".pl", L".plg", L".prf", L".prg", L".printerexport", L".ps1", L".ps1xml", L".ps2", L".ps2xml", L".psc1", L".psc2", L".psd1", L".psdm1", L".psdm1cdxml", L".pssc", L".pst", L".py", L".pyc", L".pyo", L".pyw", L".pyz", L".pyzw",
		L".reg",
		L".scf", L".scr", L".sct", L".settingcontent", L".shb", L".shs",
		L".theme", L".tmp",
		L".udl", L".url",
		L".vb", L".vbe", L".vbp", L".vbs", L".vsmacros", L".vss", L".vst", L".vsw",
		L".webpnp", L".website", L".ws", L".wsb", L".wsc", L".wsf", L".wsh",
		L".xbap", L".xll", L".xml", L".xnk"
	};

	auto is_valid_extension = [&](std::wstring ext)
	{
		if (ext.size() == 0) return true;
		if (ext.size() == 1 && ext[0] == L'.') return false;

		std::transform(ext.begin(), ext.end(), ext.begin(),
			[](wchar_t c) { return std::tolower(c); });

		for (auto& bext : blacklisted_extensions)
			if (ext == bext)
				return false;

		if ((ext.size() != 1) && (ext[0] == L'.') && (*(ext.end() - 1) == L' ')) return false;
		if ((ext.size() != 1) && (ext[0] == L'.') && (*(ext.end() - 1) != L' ')) return true;

		return true;
	};

	if (is_valid_extension(user_defined_path.extension().wstring()))
	{
		if (std::filesystem::exists(user_defined_path.parent_path()))
		{
			auto resolved_working_dir = std::filesystem::canonical(working_directory);
			auto resolved_user_dir = std::filesystem::canonical(user_defined_path.parent_path());

			if (resolved_user_dir.wstring().rfind(resolved_working_dir.wstring(), 0) == 0)
			{
				return { true, resolved_user_dir / user_defined_path.filename(), resolved_working_dir };
			}
		}
	}

	return { false, "", "" };
};

LFS_EXPORT int lfs_writefile(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 2);
	luaL_checkstring(lua_state, 1);

	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		size_t data_size;
		const char* data_string = lua_tolstring(lua_state, 2, &data_size);

		std::ofstream F(path_result.path, std::ios::binary | std::ios::trunc);
		if (F.is_open())
		{
			F.write(data_string, data_size);
			F.close();
		}
		else luaL_error(lua_state, "could not create or write to file");
	}
	return 0;
};

LFS_EXPORT int lfs_appendfile(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 2);
	luaL_checkstring(lua_state, 1);

	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		if (std::filesystem::exists(path_result.path) && std::filesystem::is_regular_file(path_result.path))
		{
			size_t data_size;
			const char* data_string = lua_tolstring(lua_state, 2, &data_size);

			std::ofstream F(path_result.path, std::ios::binary | std::ios::app);
			if (F.is_open())
			{
				F.write(data_string, data_size);
				F.close();
			}
			else luaL_error(lua_state, "could not create or write to file");
		}
		else luaL_error(lua_state, "file does not exist or it is not a file");
	}
	return 0;
};

LFS_EXPORT int lfs_readfile(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 1);
	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		auto& user_path = path_result.path;
		if (std::filesystem::exists(user_path) && std::filesystem::is_regular_file(user_path))
		{
			std::ifstream F(user_path, std::ios::binary);
			if (F.is_open())
			{
				size_t file_size = std::filesystem::file_size(user_path);
				char* file_buffer = (char*)malloc(file_size);
				if (file_buffer != NULL)
				{
					F.read(file_buffer, file_size);
					F.close();

					lua_pushlstring(lua_state, (const char*)file_buffer, file_size);
					free(file_buffer);
					return 1;
				}
				else
				{
					F.close();
					luaL_error(lua_state, "could not allocate buffer for file");
				}
			}
			else luaL_error(lua_state, "could not open file");
		}
		else luaL_error(lua_state, "file does not exist or it is not a file");
	}
	lua_pushnil(lua_state);
	return 1; // string?
};

LFS_EXPORT int lfs_delfile(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 1);
	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		auto& user_path = path_result.path;
		if (std::filesystem::exists(user_path) && std::filesystem::is_regular_file(user_path))
		{
			std::filesystem::remove(user_path);
		}
		else luaL_error(lua_state, "file does not exist or it is not a file");
	}
	return 0;
};

LFS_EXPORT int lfs_delfolder(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 1);
	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		auto& user_path = path_result.path;
		if (std::filesystem::exists(user_path) && std::filesystem::is_directory(user_path))
		{
			if (!std::filesystem::equivalent(path_result.working_directory, user_path))
			{
				std::filesystem::remove_all(user_path);
			}
		}
		else luaL_error(lua_state, "file does not exist or it is not a file");
	}
	return 0;
};

LFS_EXPORT int lfs_isfile(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 1);
	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		lua_pushboolean(lua_state, std::filesystem::exists(path_result.path) && std::filesystem::is_regular_file(path_result.path));
		return 1;
	}
	lua_pushboolean(lua_state, false);
	return 1; // bool
};

LFS_EXPORT int lfs_isfolder(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 1);
	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		lua_pushboolean(lua_state, std::filesystem::exists(path_result.path) && std::filesystem::is_directory(path_result.path));
		return 1;
	}
	lua_pushboolean(lua_state, false);
	return 1; // bool
};

LFS_EXPORT int lfs_loadfile(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 1);
	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		auto& user_path = path_result.path;
		if (std::filesystem::exists(user_path) && std::filesystem::is_regular_file(user_path))
		{
			std::ifstream F(user_path, std::ios::binary);
			if (F.is_open())
			{
				size_t file_size = std::filesystem::file_size(user_path);
				char* file_buffer = (char*)malloc(file_size);
				if (file_buffer != NULL)
				{
					F.read(file_buffer, file_size);
					F.close();

					//lua_pushlstring(lua_state, (const char*)file_buffer, file_size);
					luaL_loadbuffer(lua_state, file_buffer, file_size, "@chunk.lua");
					free(file_buffer);
					return 1;
				}
				else
				{
					F.close();
					luaL_error(lua_state, "could not allocate buffer for file");
				}
			}
			else luaL_error(lua_state, "could not open file");
		}
		else luaL_error(lua_state, "file does not exist or it is not a file");
	}
	lua_pushnil(lua_state);
	return 1;
};

LFS_EXPORT int lfs_dofile(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 1);
	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		auto& user_path = path_result.path;
		if (std::filesystem::exists(user_path) && std::filesystem::is_regular_file(user_path))
		{
			std::ifstream F(user_path, std::ios::binary);
			if (F.is_open())
			{
				size_t file_size = std::filesystem::file_size(user_path);
				char* file_buffer = (char*)malloc(file_size);
				if (file_buffer != NULL)
				{
					F.read(file_buffer, file_size);
					F.close();

					//lua_pushlstring(lua_state, (const char*)file_buffer, file_size);
					luaL_loadbuffer(lua_state, file_buffer, file_size, "@chunk.lua");
					lua_pcall(lua_state, 0, 0, 0);
					free(file_buffer);
					return 1;
				}
				else
				{
					F.close();
					luaL_error(lua_state, "could not allocate buffer for file");
				}
			}
			else luaL_error(lua_state, "could not open file");
		}
		else luaL_error(lua_state, "file does not exist or it is not a file");
	}
	lua_pushnil(lua_state);
	return 1;
};

LFS_EXPORT int lfs_listfiles(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 1);
	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		auto& user_path = path_result.path;
		if (std::filesystem::exists(user_path) && std::filesystem::is_directory(user_path))
		{
			if (!std::filesystem::equivalent(path_result.working_directory.parent_path(), user_path))
			{
				lua_createtable(lua_state, 0, 0);
				int table_index = 0;
				for (const auto& sub_object : std::filesystem::directory_iterator{ user_path })
				{
					auto object_name = sub_object.path().filename().u8string();

					lua_pushnumber(lua_state, ++table_index);
					lua_pushstring(lua_state, (const char*)object_name.c_str());
					lua_rawset(lua_state, -3);
				}
				return 1;
			}
		}
		else luaL_error(lua_state, "file system object does not exist or is not directory");
	}
	lua_pushnil(lua_state);
	return 1;
};

LFS_EXPORT int lfs_makefolder(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 1);
	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		if (!std::filesystem::exists(path_result.path))
		{
			std::filesystem::create_directories(path_result.path);
		}
	}
	return 0;
};

static const luaL_Reg fs_funcs[] = {
	{"writefile", lfs_writefile},
	{"appendfile", lfs_appendfile},
	{"readfile", lfs_readfile},
	{"delfile", lfs_delfile},
	{"delfolder", lfs_delfolder},
	{"isfile", lfs_isfile},
	{"isfolder", lfs_isfolder},
	{"loadfile", lfs_loadfile},
	{"dofile", lfs_dofile},
	{"listfiles", lfs_listfiles},
	{"makefolder", lfs_makefolder},
	{NULL, NULL}
};

extern "C" LFS_EXPORT int luaopen_lfs(lua_State* lua_state)
{
#if defined(LFS_LUA_5_4) || defined(LFS_LUA_5_3) || defined(LFS_LUA_5_2)
	lua_pushglobaltable(lua_state);
	luaL_setfuncs(lua_state, fs_funcs, 0);
#else
	lua_pushvalue(lua_state, LUA_GLOBALSINDEX);
	luaL_register(lua_state, NULL, fs_funcs);
	lua_pop(lua_state, 1);
#endif
	return 1;
}