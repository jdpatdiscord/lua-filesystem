#include "main.hpp"

int main(int argc, char* argv[])
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	lua_getglobal(L, "print");
/*	int status = luaL_loadstring(L, R"(
		local dll = io.open("../x64/Release/lfs.dll", "rb")
		local dll_data = dll:read("*a")
		dll:close()
		local new_dll = io.open("lfs.dll", "wb")
		new_dll:write(dll_data)
		new_dll:close()
		require("lfs")
		print(fs)
	)");*/
	luaL_loadstring(L, R"(
		require("lfs")
		print(fs)
	)");
	lua_pcall(L, 0, 0, 1);

	lua_close(L);

	return 0;
}