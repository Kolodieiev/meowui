#pragma once
#include <Arduino.h>
#include "../files/FileManager.h"

namespace meow
{
    /**
     * @brief Дозволяє зберігати та завантажувати окремі налаштування до бінарних файлів на карту пам'яті.
     * Бінарні файли з налаштуваннями зберігаються у папці "/.data/preferences"
     *
     */
    class SettingsManager : private FileManager
    {
    public:
        using FileManager::isMounted;

        /**
         * @brief Зберігає налаштування до бінарного файлу.
         *
         * @param pref_name Ім'я налаштування.
         * @param value Значення налаштування.
         * @param subdir Ім'я підкаталогу з налаштуваннями.
         * @return true - якщо операція виконана успішно.
         * @return false - якщо операція завершилася невдачею.
         */
        bool set(const char *pref_name, const char *value, const char *subdir = "");

        /**
         * @brief Читає налаштування з бінарного файлу.
         *
         * @param pref_name Ім'я налаштування.
         * @param subdir Ім'я підкаталогу з налаштуваннями.
         * @return String - Рядок, що містить дані налаштування, або порожній рядок, якщо файл з налаштуванням не вдалося прочитати.
         */
        String get(const char *pref_name, const char *subdir = "");

        /**
         * @brief Повертає повний шлях до файла в каталозі налаштувань.
         * Якщо каталог налаштувань не існує, буде виконана спроба його створення.
         *
         * @param pref_name Шлях до файла з налаштуваннями.
         * @param subdir Ім'я підкаталогу з налаштуваннями.
         * @return String - Рядок, що містить повний шлях до файла, або порожній рядок, у разі помилки.
         */
        String getSettingsFilePath(const char *pref_name, const char *subdir = "");
    };
}