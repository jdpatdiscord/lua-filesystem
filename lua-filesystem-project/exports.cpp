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

PathValidationResult validate_path(const char* user_path)
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
}

int lfs_writefile(lua_State* lua_state)
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
		else throw std::runtime_error("could not create or write to file");
	}
	return 0;
};

int lfs_appendfile(lua_State* lua_state)
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
			else throw std::runtime_error("could not create or write to file");
		}
		else throw std::runtime_error("file does not exist or it is not a file");
	}
	return 0;
};

int lfs_readfile(lua_State* lua_state)
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
					throw std::runtime_error("could not allocate buffer for file");
				}
			}
			else throw std::runtime_error("could not open file");
		}
		else throw std::runtime_error("file does not exist or it is not a file");
	}
	lua_pushnil(lua_state);
	return 1; // string?
};

int lfs_delfile(lua_State* lua_state)
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
		else throw std::runtime_error("file does not exist or it is not a file");
	}
	return 0;
};

int lfs_delfolder(lua_State* lua_state)
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
		else throw std::runtime_error("file does not exist or it is not a file");
	}
	return 0;
};

int lfs_isfile(lua_State* lua_state)
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

int lfs_isfolder(lua_State* lua_state)
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

int lfs_loadfile(lua_State* lua_state)
{
	// Implementation defined
	return 0; // function?
};

int lfs_dofile(lua_State* lua_state)
{
	// Implementation defined
	return 0;
};

int lfs_listfiles(lua_State* lua_state)
{
	luaL_checkstring(lua_state, 1);
	auto path_result = validate_path(lua_tostring(lua_state, 1));
	if (path_result.is_valid_path == true)
	{
		auto& user_path = path_result.path;
		if (std::filesystem::exists(user_path) && std::filesystem::is_directory(user_path))
		{
			lua_createtable(lua_state, 0, 0);
			int table_index = 0;
			for (const auto& sub_object : std::filesystem::directory_iterator{ user_path })
			{
				auto object_name = sub_object.path().u8string();
				object_name.erase(0, path_result.working_directory.u8string().size() + 1);

				lua_pushnumber(lua_state, ++table_index);
				lua_pushstring(lua_state, (const char*)object_name.c_str());
				lua_rawset(lua_state, -3);
			}

			return 1;
		}
		else throw std::runtime_error("file system object does not exist or is not directory");
	}
	lua_pushnil(lua_state);
	return 1;
};

int lfs_makefolder(lua_State* lua_state)
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

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	lua_pushcfunction(L, lfs_makefolder);
	lua_setfield(L, LUA_GLOBALSINDEX, "makefolder");
	lua_pushcfunction(L, lfs_isfile);
	lua_setfield(L, LUA_GLOBALSINDEX, "isfile");
	lua_pushcfunction(L, lfs_isfolder);
	lua_setfield(L, LUA_GLOBALSINDEX, "isfolder");
	lua_pushcfunction(L, lfs_delfile);
	lua_setfield(L, LUA_GLOBALSINDEX, "delfile");
	lua_pushcfunction(L, lfs_readfile);
	lua_setfield(L, LUA_GLOBALSINDEX, "readfile");
	lua_pushcfunction(L, lfs_writefile);
	lua_setfield(L, LUA_GLOBALSINDEX, "writefile");
	lua_pushcfunction(L, lfs_listfiles);
	lua_setfield(L, LUA_GLOBALSINDEX, "listfiles");
	lua_pushcfunction(L, lfs_delfolder);
	lua_setfield(L, LUA_GLOBALSINDEX, "delfolder");
	lua_pushcfunction(L, lfs_dofile);
	lua_setfield(L, LUA_GLOBALSINDEX, "dofile");
	lua_pushcfunction(L, lfs_loadfile);
	lua_setfield(L, LUA_GLOBALSINDEX, "loadfile");

	luaL_loadstring(L, "print(\"Hello, World!\")");
	lua_call(L, 0, 0);

	lua_close(L);

	Sleep(1000000);

	return 0;
}