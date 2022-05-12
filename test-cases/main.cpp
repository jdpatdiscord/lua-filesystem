#include "main.hpp"

int main(int argc, char* argv[])
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	lua_getglobal(L, "print");
	luaL_loadstring(L, R"(
		require("lfs")
		
		-- Sanity test
		
		writefile("test.txt", "Hello")
		assert(isfile("test.txt"), "should exist")
		assert(not isfolder("test.txt"), "wrong result")
		assert(readfile("test.txt") == "Hello", "wrong result")
		appendfile("test.txt", ", World!")
		assert(readfile("test.txt") == "Hello, World!", "wrong result")
		delfile("test.txt")
		assert(not isfile("test.txt"), "wrong result")
		makefolder("test")
		assert(isfolder("test"), "should exist")
		assert(not isfolder("test 2"), "should not exist")
		writefile("test/file.txt", "this is a test")
		assert(isfile("test/file.txt"), "wrong result")
		assert(#listfiles("test") == 1, "wrong result")
		assert(listfiles("test")[1] == "test/file.txt" or listfiles("test")[1] == "test\\file.txt", "wrong result") 
		delfile("test/file.txt")
		assert(not isfile("test/file.txt"), "wrong result")
		assert(#listfiles("test") == 0, "wrong result")
		delfolder("test")
		assert(not isfolder("test"), "should not exist")
		writefile("test.lua", "return 5")
		assert(type(loadfile("test.lua")) == 'function', "wrong type")
		assert(loadfile("test.lua")() == 5, "wrong result")
		pcall(loadfile("test.lua"), print)
		writefile("test.lua", "A = \"Hello!\"")
		dofile("test.lua")
		assert(A == "Hello!", "wrong result")
		delfile("test.lua")
		
		-- Basic sandbox testing

		writefile("../test.txt", "hello")
		assert(type(readfile("../test.txt")) == 'nil', "wrong type")
		writefile("../workspace/test.txt", "hello")
		assert(type(readfile("../workspace/test.txt")) == 'string', "wrong type")
		assert(readfile("test.txt") == readfile("../workspace/test.txt"), "wrong result")
		delfile("test.txt")
		writefile("test.exe", "text")
		assert(not isfile("test.exe"), "wrong result")
		writefile("test.exe.", "text")
		assert(not isfile("test.exe"), "wrong result")

		print("Test finished")
	)");
	lua_pcall(L, 0, 0, 1);

	lua_close(L);

	return 0;
}