#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

namespace meow
{
    /**
     * @brief Обрізає розширення файлу в рядку.
     *
     * @param out_str Рядок, в якому потрібно видалити розширення.
     * @param ext Розширення.
     */
    void rmFilenameExt(String &out_str, const char *ext);
}