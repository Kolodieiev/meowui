#pragma GCC optimize("O3")
#include "ContextManager.h"
#include "../../ui/context/IContext.h"

#include "meowui_setup/context_id.h"
#include "meowui_setup/ui_setup.h"

namespace meow
{
    ContextManager MEOW;

    void ContextManager::run()
    {
#ifdef GRAPHICS_ENABLED
        _display.init();
#endif
        _input._init();

        IContext *context = new START_CONTEXT();

        // Основний цикл GUI.
        while (1)
        {
            if (!context->isReleased())
                context->tick();
            else
            {
                ContextID next_context_id = context->getNextContextID();

                delete context;

                switch (next_context_id)
                {
                    SCREEN_CASES
                default:
                    log_e("Некоректний context_id: %u", next_context_id);
                    esp_restart();
                }
            }
        }
    }
}