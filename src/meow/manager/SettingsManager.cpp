#include "SettingsManager.h"
#include "SD_Manager.h"

namespace meow
{
    const char DATA_ROOT[] = "/.data";
    const char PREF_ROOT[] = "/.data/preferences";

    bool SettingsManager::set(const char *pref_name, const char *value, const char *subdir)
    {
        if (!pref_name || !value)
        {
            log_e("Некоректні аргументи");
            return false;
        }

        String path = getSettingsFilePath(pref_name, subdir);

        if (path.isEmpty())
            return false;

        return _fs.writeFile(path.c_str(), value, strlen(value));
    }

    String SettingsManager::get(const char *pref_name, const char *subdir)
    {
        if (!pref_name)
        {
            log_e("Некоректний аргумент");
            return emptyString;
        }

        String path = getSettingsFilePath(pref_name, subdir);

        if (path.isEmpty())
            return emptyString;

        size_t file_size = _fs.getFileSize(path.c_str());

        if (file_size == 0)
            return emptyString;

        char *buffer = static_cast<char *>(malloc(file_size + 1));

        if (!buffer)
        {
            log_e("Bad memory alloc: %zu b", file_size);
            return emptyString;
        }

        size_t bytes_read = _fs.readFile(path.c_str(), buffer, file_size);
        buffer[bytes_read] = '\0';

        String ret;
        if (bytes_read > 0)
            ret = buffer;

        free(buffer);
        return ret;
    }

    String SettingsManager::getSettingsFilePath(const char *pref_name, const char *subdir)
    {
        if (!pref_name || !std::strcmp(pref_name, "") || !subdir)
        {
            log_e("Некоректний аргумент");
            return emptyString;
        }

        String path = getSettingsDirPath(subdir);

        if (path.isEmpty())
            return emptyString;

        path += "/";
        path += pref_name;

        return path;
    }

    String SettingsManager::getSettingsDirPath(const char *sub_dirname)
    {
        if (!sub_dirname)
        {
            log_e("Некоректний аргумент");
            return emptyString;
        }

        if (!_fs.dirExist(DATA_ROOT, true) && !_fs.createDir(DATA_ROOT))
            return emptyString;

        if (!_fs.dirExist(PREF_ROOT, true) && !_fs.createDir(PREF_ROOT))
            return emptyString;

        String path = PREF_ROOT;

        if (std::strcmp(sub_dirname, ""))
        {
            path += "/";
            path += sub_dirname;

            if (!_fs.dirExist(path.c_str()) && !_fs.createDir(path.c_str()))
                return emptyString;
        }

        return path;
    }

    bool SettingsManager::load(void *out_data_struct, size_t data_struct_size, const char *data_filename, const char *data_dirname)
    {
        String sets_path = SettingsManager::getSettingsFilePath(data_filename, data_dirname);

        if (sets_path.isEmpty())
            return false;

        if (!_fs.fileExist(sets_path.c_str(), true))
            return false;

        return _fs.readFile(sets_path.c_str(), out_data_struct, data_struct_size) == data_struct_size;
    }

    bool SettingsManager::save(const void *data_struct, size_t data_struct_size, const char *data_filename, const char *data_dirname)
    {
        String sets_path = SettingsManager::getSettingsFilePath(data_filename, data_dirname);

        if (sets_path.isEmpty())
            return false;

        return _fs.writeFile(sets_path.c_str(), data_struct, data_struct_size) == data_struct_size;
    }
}
