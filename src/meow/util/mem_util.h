#pragma once
#include <Arduino.h>

namespace meow
{
#define __malloc_heap_psram(size) \
    heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL)

    void print_free_stack();
    void print_free_heap();
    void print_free_psram();
    void print_check_heap_integrity();
}