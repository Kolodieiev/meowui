#include "lua_sd.h"
#include <Arduino.h>
#include "meow/manager/files/FileManager.h"
#include "meow/plugin/lua/res/lua_strs.h"

const char STR_FILE_TYPE[] = "FILE*";
//
const char STR_MEM_ALLOC_ERR[] = "Помилка створення буфера";
const char STR_FILE_READ_ERR[] = "Помилка читання файла";
//
const char STR_SD_IS_MOUNTED[] = "isMounted";
const char STR_SD_FILE_EXIST[] = "fileExist";
const char STR_SD_DIR_EXIST[] = "dirExist";
const char STR_SD_OPEN_FILE[] = "open";
const char STR_SD_CLOSE_FILE[] = "close";
const char STR_SD_GET_FILE_SIZE[] = "fileSize";
const char STR_SD_AVAILABLE[] = "available";
const char STR_SD_READ_FILE[] = "read";
const char STR_SD_READ_FROM_FILE[] = "readFrom";
const char STR_SD_WRITE_FILE[] = "write";
const char STR_SD_WRITE_TO_FILE[] = "writeTo";
const char STR_SD_COPY_FILE[] = "copyFile";
const char STR_SD_REMOVE[] = "remove";
const char STR_SD_RENAME[] = "rename";
const char STR_SD_CREATE_DIR[] = "mkDir";

int lua_sd_is_mounted(lua_State *L)
{
    lua_pushboolean(L, meow::_sd.isMounted());
    return 1;
}

int lua_sd_file_exist(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    lua_pushboolean(L, meow::_fs.fileExist(path, true));
    return 1;
}

int lua_sd_dir_exist(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    lua_pushboolean(L, meow::_fs.dirExist(path, true));
    return 1;
}

int lua_sd_open_file(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    const char *mode = luaL_checkstring(L, 2);

    FILE *fp = meow::_fs.openFile(path, mode);

    if (!fp)
    {
        lua_pushnil(L);
    }
    else
    {
        FILE *f = *(FILE **)lua_newuserdata(L, sizeof(FILE *));
        f = fp;
        luaL_getmetatable(L, STR_FILE_TYPE);
        lua_setmetatable(L, -2);
    }

    return 1;
}

int lua_sd_close_file(lua_State *L)
{
    FILE *f = *(FILE **)luaL_checkudata(L, 1, STR_FILE_TYPE);
    meow::_fs.closeFile(f);
    return 0;
}

int lua_sd_file_size(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    lua_pushinteger(L, meow::_fs.getFileSize(path));
    return 1;
}

int lua_sd_available(lua_State *L)
{
    FILE *f = *(FILE **)luaL_checkudata(L, 1, STR_FILE_TYPE);
    int size = luaL_checkinteger(L, 2);

    lua_pushinteger(L, meow::_fs.available(f, size));
    return 1;
}

int lua_sd_read_file(lua_State *L)
{
    bool has_seek_pos = lua_gettop(L) == 3;

    const char *path = luaL_checkstring(L, 1);
    int len = luaL_checkinteger(L, 2);
    int seek_pos = 0;
    if (has_seek_pos)
        seek_pos = luaL_checkinteger(L, 3);

    void *buffer;
    if (psramInit())
        buffer = ps_malloc(len);
    else
        buffer = malloc(len);

    if (!buffer)
        return luaL_error(L, STR_MEM_ALLOC_ERR);

    size_t bytes_read = meow::_fs.readFile(path, buffer, len, seek_pos);

    lua_pushlstring(L, (char *)buffer, bytes_read);
    free(buffer);
    return 1;
}

int lua_sd_read_from_file(lua_State *L)
{
    bool has_seek_pos = lua_gettop(L) == 3;

    FILE *f = *(FILE **)luaL_checkudata(L, 1, STR_FILE_TYPE);
    int len = luaL_checkinteger(L, 2);
    int seek_pos = 0;
    if (has_seek_pos)
        seek_pos = luaL_checkinteger(L, 3);

    void *buffer;
    if (psramInit())
        buffer = ps_malloc(len);
    else
        buffer = malloc(len);

    if (!buffer)
        return luaL_error(L, STR_MEM_ALLOC_ERR);

    bool successfull = meow::_fs.readFromFile(f, buffer, len, seek_pos);

    if (!successfull)
    {
        free(buffer);
        return luaL_error(L, STR_FILE_READ_ERR);
    }

    lua_pushlstring(L, (char *)buffer, len);
    free(buffer);

    return 1;
}

int lua_sd_write_file(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    size_t len;
    const char *buf = luaL_checklstring(L, 2, &len);

    size_t bytes_write = meow::_fs.writeFile(path, buf, len);

    lua_pushboolean(L, bytes_write == len);

    return 1;
}

int lua_sd_write_to_file(lua_State *L)
{
    FILE *f = *(FILE **)luaL_checkudata(L, 1, STR_FILE_TYPE);
    size_t len;
    const char *buf = luaL_checklstring(L, 2, &len);

    size_t bytes_write = meow::_fs.writeToFile(f, buf, len);

    lua_pushboolean(L, bytes_write == len);

    return 1;
}

int lua_sd_copy_file(lua_State *L)
{
    const char *from_path = luaL_checkstring(L, 1);
    const char *to_path = luaL_checkstring(L, 2);

    bool result = false;
    if (meow::_fs.startCopyFile(from_path, to_path))
    {
        while (meow::_fs.isWorking())
            vTaskDelay(2 / portTICK_PERIOD_MS);

        result = meow::_fs.lastTaskResult();
    }

    lua_pushboolean(L, result);

    return 1;
}

int lua_sd_remove(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);

    bool result = false;
    if (meow::_fs.startRemoving(path))
    {
        while (meow::_fs.isWorking())
            vTaskDelay(2 / portTICK_PERIOD_MS);

        result = meow::_fs.lastTaskResult();
    }

    lua_pushboolean(L, result);

    return 1;
}

int lua_sd_rename(lua_State *L)
{
    const char *old_name = luaL_checkstring(L, 1);
    const char *new_name = luaL_checkstring(L, 2);

    lua_pushboolean(L, meow::_fs.rename(old_name, new_name));

    return 1;
}

int lua_sd_create_dir(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);

    lua_pushboolean(L, meow::_fs.createDir(path));
    return 1;
}

int lua_sd_unload(lua_State *L)
{
    lua_pushnil(L);
    lua_setfield(L, LUA_REGISTRYINDEX, STR_FILE_TYPE);
    return 0;
}

const struct luaL_Reg LIB_SD[] = {
    {STR_SD_IS_MOUNTED, lua_sd_is_mounted},
    {STR_SD_FILE_EXIST, lua_sd_file_exist},
    {STR_SD_DIR_EXIST, lua_sd_dir_exist},
    {STR_SD_OPEN_FILE, lua_sd_open_file},
    {STR_SD_CLOSE_FILE, lua_sd_close_file},
    {STR_SD_GET_FILE_SIZE, lua_sd_file_size},
    {STR_SD_AVAILABLE, lua_sd_available},
    {STR_SD_READ_FILE, lua_sd_read_file},
    {STR_SD_READ_FROM_FILE, lua_sd_read_from_file},
    {STR_SD_WRITE_FILE, lua_sd_write_file},
    {STR_SD_WRITE_TO_FILE, lua_sd_write_to_file},
    {STR_SD_COPY_FILE, lua_sd_copy_file},
    {STR_SD_REMOVE, lua_sd_remove},
    {STR_SD_RENAME, lua_sd_rename},
    {STR_SD_CREATE_DIR, lua_sd_create_dir},
    {STR_LUA_UNLOAD, lua_sd_unload},
    {nullptr, nullptr},
};

int lua_open_sd(lua_State *L)
{
    luaL_newlib(L, LIB_SD);

    luaL_newmetatable(L, STR_FILE_TYPE);
    lua_pushcfunction(L, lua_sd_close_file);
    lua_setfield(L, -2, STR_LUA_GC);
    lua_pop(L, 1);

    return 1;
}