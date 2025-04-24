#pragma GCC optimize("O3")
#include "meowui_setup/sd_setup.h"
#include "FileManager.h"
#include "../sd/SD_Manager.h"
#include "esp_task_wdt.h"
#include <errno.h>
#include <sys/stat.h>

namespace meow
{
    bool FileManager::isSdMounted()
    {
        bool result = true;

        if (!SD_Manager::getInst().mount())
        {
            log_e("Карту пам'яті не примонтовано");
            result = false;
        }

        return result;
    }

    void FileManager::makeFullPath(String &out_path, const char *path)
    {
        out_path = "";
        out_path = SD_MOUNTPOINT;
        out_path += path;
    }

    uint8_t FileManager::getEntryType(const char *path, dirent *entry)
    {
        if (entry && entry->d_type != DT_UNKNOWN)
            return entry->d_type;
        else
        {
            struct stat st;
            if (stat(path, &st) == 0)
            {
                if (S_ISREG(st.st_mode))
                    return DT_REG;
                else if (S_ISDIR(st.st_mode))
                    return DT_DIR;
            }

            return DT_UNKNOWN;
        }
    }

    size_t FileManager::getFileSize(const char *path)
    {
        String full_path;
        makeFullPath(full_path, path);

        struct stat st;
        if (stat(full_path.c_str(), &st) != 0 || !S_ISREG(st.st_mode))
            return 0;

        return static_cast<size_t>(st.st_size);
    }

    bool FileManager::readStat(struct stat &out_stat, const char *path)
    {
        String full_path;
        makeFullPath(full_path, path);

        if (stat(full_path.c_str(), &out_stat) != 0)
            return false;

        return true;
    }

    bool FileManager::fileExist(const char *path, bool silently)
    {
        String full_path;
        makeFullPath(full_path, path);

        bool result = getEntryType(full_path.c_str()) == DT_REG;

        if (!result && !silently)
            log_e("File %s not found", full_path.c_str());

        return result;
    }

    bool FileManager::dirExist(const char *path, bool silently)
    {
        String full_path;
        makeFullPath(full_path, path);

        bool result = getEntryType(full_path.c_str()) == DT_DIR;

        if (!result && !silently)
            log_e("Dir %s not found", full_path.c_str());

        return result;
    }

    bool FileManager::exists(const char *path, bool silently)
    {
        String full_path;
        makeFullPath(full_path, path);

        uint8_t type = getEntryType(full_path.c_str());

        if (type == DT_REG || type == DT_DIR)
            return true;

        log_e("[ %s ] not exist", full_path.c_str());
        return false;
    }

    bool FileManager::createDir(const char *path)
    {
        String full_path;
        makeFullPath(full_path, path);

        errno = 0;

        bool result = !mkdir(full_path.c_str(), 0777);

        if (!result)
        {
            log_e("Помилка створення директорії: %s", path);
            if (errno == EEXIST)
                log_e("Директорія існує");
        }

        return result;
    }

    size_t FileManager::readFile(const char *path, void *out_buffer, size_t len, int32_t seek_pos)
    {
        String full_path;
        makeFullPath(full_path, path);

        FILE *f = fopen(full_path.c_str(), "rb");

        if (!f)
        {
            log_e("Помилка відркиття файлу: %s", path);
            return 0;
        }

        if (seek_pos > 0 && fseek(f, seek_pos, SEEK_SET))
        {
            log_e("Помилка встановлення позиції(%d) у файлі %s", seek_pos, path);
            return 0;
        }

        size_t bytes_read = fread(out_buffer, 1, len, f);

        if (bytes_read != len)
            log_e("Прочитано: [ %zu ]  Очікувалося: [ %zu ]", bytes_read, len);

        fclose(f);
        return bytes_read;
    }

    bool FileManager::readFromFile(FILE *file, void *out_buffer, size_t len, int32_t seek_pos)
    {
        if (len == 0 || !file)
        {
            log_e("Bad arguments");
            return false;
        }

        if (seek_pos > 0 && fseek(file, seek_pos, SEEK_SET))
        {
            log_e("Помилка встановлення позиції: %d", seek_pos);
            return false;
        }

        size_t section_read = fread(out_buffer, len, 1, file);

        if (section_read == 0)
        {
            log_e("Не вдалося прочитати всі %zu байтів", len);
            return false;
        }

        return true;
    }

    size_t FileManager::writeFile(const char *path, const void *buffer, size_t len)
    {
        if (!path || !buffer || len == 0)
        {
            log_e("Bad arguments");
            return 0;
        }

        String full_path;
        makeFullPath(full_path, path);

        FILE *f = fopen(full_path.c_str(), "wb");

        if (!f)
        {
            log_e("Помилка відркиття файлу: %s", path);
            return 0;
        }

        size_t written = writeOptimal(f, buffer, len);

        fclose(f);

        return written;
    }

    size_t FileManager::writeToFile(FILE *file, const void *buffer, size_t len)
    {
        if (!file || !buffer || len == 0)
        {
            log_e("Bad arguments");
            return 0;
        }

        return writeOptimal(file, buffer, len);
    }

    size_t FileManager::writeOptimal(FILE *file, const void *buffer, size_t len)
    {
        size_t opt_size = 256;
        size_t total_written = 0;

        size_t full_blocks = len / opt_size;
        size_t remaining_bytes = len % opt_size;

        unsigned long last_delay_time = millis();
        for (size_t i = 0; i < full_blocks; ++i)
        {
            total_written += fwrite((uint8_t *)buffer + total_written, opt_size, 1, file) * opt_size;
            if (millis() - last_delay_time > 1000)
            {
                vTaskDelay(1 / portTICK_PERIOD_MS);
                last_delay_time = millis();
            }
        }

        if (remaining_bytes > 0)
            total_written += fwrite((uint8_t *)buffer + total_written, remaining_bytes, 1, file) * remaining_bytes;

        fflush(file);

        if (total_written != len)
            log_e("Записано: [ %zu ]  Очікувалося: [ %zu ]", total_written, len);

        return total_written;
    }

    FILE *FileManager::getFileDescriptor(const char *path, const char *mode)
    {
        String full_path;
        makeFullPath(full_path, path);

        FILE *f = fopen(full_path.c_str(), mode);

        if (!f)
            log_e("Помилка взяття дескриптора для %s", full_path.c_str());

        return f;
    }

    void FileManager::closeFile(FILE *&file)
    {
        if (file)
        {
            fclose(file);
            file = nullptr;
        }
    }

    bool FileManager::seekPos(FILE *file, int32_t pos, uint8_t mode)
    {
        if (!file)
            return false;

        if (fseek(file, pos, mode))
        {
            log_e("Помилка встановлення позиції [%d]", pos);
            return false;
        }
        return true;
    }

    size_t FileManager::getPos(FILE *file)
    {
        if (!file)
            return 0;

        return ftell(file);
    }

    size_t FileManager::available(FILE *file, size_t file_size)
    {
        if (!file || feof(file))
            return 0;

        long tell = ftell(file);

        if (file_size < tell)
            return 0;

        return file_size - tell;
    }

    void FileManager::rm()
    {
        bool result = false;

        String full_path;
        makeFullPath(full_path, _rm_path.c_str());

        bool is_dir = dirExist(_rm_path.c_str(), true);

        if (!is_dir)
            result = rmFile(full_path.c_str());
        else
            result = rmDir(full_path.c_str());

        if (result)
            log_i("Успішно видалено: %s", full_path.c_str());

        taskDone(result);
    }

    bool FileManager::rmFile(const char *path, bool make_full)
    {
        bool result;

        if (make_full)
        {
            String full_path;
            makeFullPath(full_path, path);
            result = !remove(full_path.c_str());
        }
        else
            result = !remove(path);

        if (!result)
            log_e("Помилка видалення файлу: %s", path);

        return result;
    }

    bool FileManager::rmDir(const char *path, bool make_full)
    {
        bool result = false;

        dirent *dir_entry{nullptr};
        DIR *dir;

        if (make_full)
        {
            String full_path;
            makeFullPath(full_path, path);
            dir = opendir(full_path.c_str());
        }
        else
        {
            dir = opendir(path);
        }

        if (!dir)
            goto exit;

        errno = 0;

        while (!_is_canceled)
        {
            dir_entry = readdir(dir);
            if (!dir_entry)
            {
                if (!errno)
                    result = true;
                break;
            }

            if (std::strcmp(dir_entry->d_name, ".") == 0 || std::strcmp(dir_entry->d_name, "..") == 0)
                continue;

            String full_path = path;
            full_path += "/";
            full_path += dir_entry->d_name;

            uint8_t entr_type = getEntryType(full_path.c_str(), dir_entry);

            if (entr_type == DT_REG)
            {
                result = rmFile(full_path.c_str());
                if (!result)
                    goto exit;
            }
            else if (entr_type == DT_DIR)
            {
                result = rmDir(full_path.c_str());

                if (!result)
                    goto exit;
            }
            else
            {
                log_e("Невідомий тип або не вдалося прочитати: %s", path);
                goto exit;
            }

            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

    exit:
        if (dir)
            closedir(dir);

        if (result)
            result = !rmdir(path);
        else
            log_e("Помилка під час видалення: %s", path);

        return result;
    }

    void FileManager::rmTask(void *params)
    {
        FileManager *instance = static_cast<FileManager *>(params);
        instance->rm();
    }

    bool FileManager::startRemoving(const char *path)
    {
        if (_task_handler)
        {
            log_e("Вже працює інша задача");
            return false;
        }

        if (!exists(path))
            return false;

        _rm_path = path;
        _is_canceled = false;

        BaseType_t result = xTaskCreatePinnedToCore(rmTask, "rmTask", TASK_SIZE, this, 10, &_task_handler, 1);

        if (result == pdPASS)
        {
            _is_working = true;
            log_i("rmTask is working now");
            return true;
        }
        else
        {
            log_e("rmTask was not running");
            return false;
        }
    }

    bool FileManager::rename(const char *old_name, const char *new_name)
    {
        if (!exists(old_name))
            return false;

        String old_n;
        makeFullPath(old_n, old_name);
        String new_n;
        makeFullPath(new_n, new_name);

        if (new_n.length() >= old_n.length() &&
            (new_n.c_str()[old_n.length()] == '/' || new_n.c_str()[old_n.length()] == '\0') &&
            strncmp(old_n.c_str(), new_n.c_str(), old_n.length()) == 0)
        {
            log_e("Старе і нове ім'я збігаються або спроба переміщення каталогу до самого себе");
            return false;
        }

        return !::rename(old_n.c_str(), new_n.c_str());
    }

    void FileManager::copyFile()
    {
        uint16_t counter = 1;
        String to_temp = _copy_to_path;
        while (_copy_from_path.equals(_copy_to_path) || fileExist(_copy_to_path.c_str(), true))
        {
            _copy_to_path = to_temp.substring(0, to_temp.lastIndexOf("."));
            _copy_to_path += "(";
            _copy_to_path += counter;
            _copy_to_path += ")";
            _copy_to_path += to_temp.substring(to_temp.lastIndexOf("."));
            ++counter;
        }

        String from;
        String to;

        makeFullPath(from, _copy_from_path.c_str());
        makeFullPath(to, _copy_to_path.c_str());

        FILE *n_f = fopen(to.c_str(), "a");

        if (!n_f)
        {
            log_e("Помилка створення файлу: %s", to.c_str());
            taskDone(false);
            return;
        }

        FILE *o_f = fopen(from.c_str(), "r");

        if (!o_f)
        {
            log_e("Помилка читання файлу: %s", from.c_str());
            fclose(n_f);
            taskDone(false);
            return;
        }

        size_t buf_size = 1024;
        uint8_t *buffer;

        if (psramInit())
        {
            buf_size *= 160;
            buffer = (uint8_t *)ps_malloc(buf_size);
        }
        else
        {
            buf_size *= 16;
            buffer = (uint8_t *)malloc(buf_size);
        }

        if (!buffer)
        {
            fclose(n_f);
            fclose(o_f);

            log_e("Помилка виділення пам'яті: %zu b", buf_size);
            esp_restart();
        }

        size_t file_size = getFileSize(_copy_from_path.c_str());

        if (file_size > 0)
        {
            log_i("Починаю копіювання");
            log_i("Із: %s", from.c_str());
            log_i("До: %s", to.c_str());

            size_t writed_bytes_counter{0};
            size_t bytes_read;

            unsigned long last_delay_time = millis();
            size_t byte_aval = 0;

            while (!_is_canceled && (byte_aval = available(o_f, file_size)) > 0)
            {
                if (byte_aval < buf_size)
                    bytes_read = fread(buffer, byte_aval, 1, o_f) * byte_aval;
                else
                    bytes_read = fread(buffer, buf_size, 1, o_f) * buf_size;
                //
                writed_bytes_counter += writeOptimal(n_f, buffer, bytes_read);
                _copy_progress = ((float)writed_bytes_counter / file_size) * 100;

                if (millis() - last_delay_time > 1000)
                {
                    vTaskDelay(1 / portTICK_PERIOD_MS);
                    last_delay_time = millis();
                }
            }
        }
        else
        {
            _copy_progress = 100;
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }

        free(buffer);

        fclose(n_f);
        fclose(o_f);

        if (_is_canceled)
        {
            rmFile(to.c_str());
            taskDone(false);
        }
        else
        {
            taskDone(true);
        }
    }

    void FileManager::copyFileTask(void *params)
    {
        FileManager *instance = static_cast<FileManager *>(params);
        instance->copyFile();
    }

    bool FileManager::startCopyFile(const char *from, const char *to)
    {
        if (!from || !to)
        {
            log_e("Bad arguments");
            return false;
        }

        if (_task_handler)
        {
            log_e("Вже працює інша задача");
            return false;
        }

        if (!fileExist(from))
            return false;

        _copy_from_path = from;
        _copy_to_path = to;

        _is_canceled = false;
        _copy_progress = 0;

        BaseType_t result = xTaskCreatePinnedToCore(copyFileTask, "copyFileTask", TASK_SIZE, this, 10, &_task_handler, 1);

        if (result == pdPASS)
        {
            _is_working = true;
            log_i("copyFileTask is working now");
            return true;
        }
        else
        {
            log_e("copyFileTask was not running");
            return false;
        }
    }

    void FileManager::startIndex(std::vector<FileInfo> &out_vec, const char *dir_path, IndexMode mode, const char *file_ext)
    {
        if (!dirExist(dir_path))
            return;

        String full_path;
        makeFullPath(full_path, dir_path);

        DIR *dir = opendir(full_path.c_str());
        if (!dir)
        {
            log_e("Помилка відкриття директорії %s", full_path.c_str());
            taskDone(false);
            return;
        }

        out_vec.clear();

        dirent *dir_entry{nullptr};
        String file_name;
        bool is_dir;

        unsigned long last_delay_time = millis();

        while (1)
        {
            dir_entry = readdir(dir);
            if (!dir_entry)
                break;

            file_name = dir_entry->d_name;

            if (file_name == "." || file_name == "..")
                continue;

            uint8_t entr_type = getEntryType(full_path.c_str(), dir_entry);

            if (entr_type == DT_REG)
                is_dir = false;
            else if (entr_type == DT_DIR)
                is_dir = true;
            else
                continue;

            out_vec.reserve(10);
            switch (mode)
            {
            case INDX_MODE_DIR:
                if (is_dir)
                    out_vec.emplace_back(file_name, true);
                break;
            case INDX_MODE_FILES:
                if (!is_dir)
                    out_vec.emplace_back(file_name, false);
                break;
            case INDX_MODE_FILES_EXT:
                if (!is_dir && file_name.endsWith(file_ext))
                    out_vec.emplace_back(file_name, false);
                break;
            case INDX_MODE_ALL:
                if (is_dir)
                    out_vec.emplace_back(file_name, true);
                else
                    out_vec.emplace_back(file_name, false);
                break;
            }
            out_vec.shrink_to_fit();

            if (millis() - last_delay_time > 1000)
            {
                vTaskDelay(1 / portTICK_PERIOD_MS);
                last_delay_time = millis();
            }
        }

        std::sort(out_vec.begin(), out_vec.end());

        if (dir)
            closedir(dir);
    }

    void FileManager::indexFilesExt(std::vector<FileInfo> &out_vec, const char *dir_path, const char *file_ext)
    {
        return startIndex(out_vec, dir_path, INDX_MODE_FILES_EXT, file_ext);
    }

    void FileManager::indexFiles(std::vector<FileInfo> &out_vec, const char *dir_path)
    {
        return startIndex(out_vec, dir_path, INDX_MODE_FILES);
    }

    void FileManager::indexDirs(std::vector<FileInfo> &out_vec, const char *dir_path)
    {
        return startIndex(out_vec, dir_path, INDX_MODE_DIR);
    }

    void FileManager::indexAll(std::vector<FileInfo> &out_vec, const char *dir_path)
    {
        return startIndex(out_vec, dir_path, INDX_MODE_ALL);
    }

    void FileManager::taskDone(bool result)
    {
        _is_working = false;

        if (_doneHandler)
            _doneHandler(result, _doneArg);

        if (_task_handler)
        {
            TaskHandle_t temp_handler = _task_handler;
            _task_handler = nullptr;
            vTaskDelete(temp_handler);
        }
    }

    void FileManager::cancel()
    {
        _is_canceled = true;
    }

    void FileManager::setTaskDoneHandler(TaskDoneHandler handler, void *arg)
    {
        _doneHandler = handler;
        _doneArg = arg;
    }

    //------------------------------------------------------------------------------------------------------------------------

    FileInfo::FileInfo(const String &name, bool is_dir) : _is_dir{is_dir}, _name_len{name.length()}
    {
        if (psramInit())
            _name = (char *)ps_malloc(_name_len + 1);
        else
            _name = (char *)malloc(_name_len + 1);

        if (!_name)
        {
            log_e("Помилка виділення буферу");
            esp_restart();
        }

        std::memcpy(_name, name.c_str(), _name_len);
        _name[_name_len] = '\0';
    }

    FileInfo::~FileInfo()
    {
        free(_name);
    }

    bool FileInfo::nameEndsWith(const char *suffix) const
    {
        if (!suffix)
            return false;

        size_t suffix_len = strlen(suffix);

        if (_name_len < suffix_len)
            return false;

        return strcmp(_name + _name_len - suffix_len, suffix) == 0;
    }

    bool FileInfo::operator<(const FileInfo &other) const
    {
        if (_is_dir != other._is_dir)
            return _is_dir;

        const char *lhs = _name;
        const char *rhs = other._name;

        while (*lhs && *rhs)
        {
            if (std::isdigit(*lhs) && std::isdigit(*rhs))
            {
                char *end_lhs;
                char *end_rhs;
                long num_lhs = std::strtol(lhs, &end_lhs, 10);
                long num_rhs = std::strtol(rhs, &end_rhs, 10);

                if (num_lhs != num_rhs)
                {
                    return num_lhs < num_rhs;
                }
                lhs = end_lhs;
                rhs = end_rhs;
            }
            else
            {
                if (*lhs != *rhs)
                {
                    return *lhs < *rhs;
                }
                ++lhs;
                ++rhs;
            }
        }

        return std::strcmp(lhs, rhs) < 0;
    }

    FileInfo::FileInfo(FileInfo &&other) noexcept : _name(other._name), _is_dir(other._is_dir), _name_len{other._name_len}
    {
        other._name = nullptr;
    }

    FileInfo &FileInfo::operator=(FileInfo &&other) noexcept
    {
        if (this != &other)
        {
            free(_name);
            _name = other._name;
            _is_dir = other._is_dir;
            _name_len = other._name_len;
            other._name = nullptr;
        }
        return *this;
    }

    //------------------------------------------------------------------------------------------------------------------------

    int FileStream::available()
    {
        if (!_file || feof(_file))
            return 0;

        return _size - ftell(_file);
    }

    size_t FileStream::readBytes(char *buffer, size_t length)
    {
        if (!_file)
            return 0;
        return fread(buffer, length, 1, _file) * length;
    }

    int FileStream::read()
    {
        if (!_file || feof(_file))
            return -1;
        return fgetc(_file);
    }

    size_t FileStream::write(uint8_t byte)
    {
        if (!_file)
            return 0;

        return fwrite(&byte, 1, 1, _file);
    }

    int FileStream::peek()
    {
        if (!_file || feof(_file))
            return -1;

        int c = fgetc(_file);
        if (c != EOF)
            ungetc(c, _file);
        return c;
    }
}