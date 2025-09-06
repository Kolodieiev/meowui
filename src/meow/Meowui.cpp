#include "Meowui.h"
#include "meow/manager/ContextManager.h"

namespace meow
{
    void Meowui::begin(uint32_t stack_depth_kb)
    {
        xTaskCreatePinnedToCore(meowContextTask, "mct", stack_depth_kb * 512, NULL, 10, NULL, 1);
    }

    void Meowui::meowContextTask(void *params)
    {
        ContextManager context_manager;
        context_manager.run();
    }

    Meowui MEOW;
}