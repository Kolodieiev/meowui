#include "mem_util.h"

namespace meow
{
    void print_free_stack()
    {
        UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        log_i("Free stack (bytes): %u\n", stackHighWaterMark * sizeof(StackType_t));
    }

    void print_free_heap()
    {
        size_t free_internal_heap = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        log_i("Free internal heap (no PSRAM): %u bytes\n", free_internal_heap);
    }

    void print_free_psram()
    {
        size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
        log_i("Free PSRAM heap: %u bytes\n", free_psram);
    }

    void print_check_heap_integrity()
    {
        log_i("Heap integrity check: %d", heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true));
    }
}