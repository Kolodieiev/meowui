#include "SettingsManager.h"
#include "../sd/SD_Manager.h"

namespace meow
{
    const char DATA_ROOT[] = "/.data";
    const char PREF_ROOT[] = "/.data/preferences";

    bool SettingsManager::set(const char *pref_name, const char *value)
    {
        if (!pref_name || !value)
        {
            log_e("Некоректні аргументи");
            return false;
        }

        String path = getSettingsFilePath(pref_name);

        if (path.isEmpty())
            return false;

        return writeFile(path.c_str(), value, strlen(value));
    }

    String SettingsManager::get(const char *pref_name)
    {
        if (!pref_name)
        {
            log_e("Некоректний аргумент");
            return emptyString;
        }

        String path = getSettingsFilePath(pref_name);

        if (path.isEmpty())
            return emptyString;

        size_t file_size = getFileSize(path.c_str());

        if (file_size == 0)
            return emptyString;

        char *buffer = (char *)malloc(file_size + 1);

        if (!buffer)
        {
            log_e("Bad memory alloc: %zu b", file_size);
            return emptyString;
        }

        size_t bytes_read = readFile(path.c_str(), buffer, file_size);
        buffer[bytes_read] = '\0';

        String ret;
        if (bytes_read > 0)
            ret = buffer;

        free(buffer);
        return ret;
    }

    String SettingsManager::getSettingsFilePath(const char *pref_name)
    {
        if (!pref_name)
        {
            log_e("Некоректний аргумент");
            return emptyString;
        }

        if (!dirExist(DATA_ROOT))
            if (!createDir(DATA_ROOT))
            {
                log_e("Помилка створення %s", DATA_ROOT);
                return emptyString;
            }

        if (!dirExist(PREF_ROOT))
            if (!createDir(PREF_ROOT))
            {
                log_e("Помилка створення %s", PREF_ROOT);
                return emptyString;
            }

        String path = PREF_ROOT;
        path += "/";
        path += pref_name;

        return path;
    }
}
