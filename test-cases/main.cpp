#include "main.hpp"

int main(int argc, char* argv[])
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	//lua_getglobal(L, "print");
	luaL_loadstring(L, R"(
		require("lfs")
		print(makefolder)
	)");
	//lua_pcall(L, 0, 0, 1);
	lua_call(L, 0, 0);

	lua_close(L);

	return 0;
}