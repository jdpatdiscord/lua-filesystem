#include "config.hpp"

int lfs_writefile(lua_State* lua_state);
int lfs_appendfile(lua_State* lua_state);
int lfs_readfile(lua_State* lua_state);
int lfs_delfile(lua_State* lua_state);
int lfs_delfolder(lua_State* lua_state);
int lfs_isfile(lua_State* lua_state);
int lfs_isfolder(lua_State* lua_state);
int lfs_loadfile(lua_State* lua_state);
int lfs_dofile(lua_State* lua_state);
int lfs_listfiles(lua_State* lua_state);
int lfs_makefolder(lua_State* lua_state);
int luaopen_lfs(lua_State* lua_state);