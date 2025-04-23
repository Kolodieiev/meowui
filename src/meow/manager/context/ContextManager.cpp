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

        IContext *context = new START_CONTEXT();

        // Основний цикл GUI.
        while (1)
        {
            if (!context->isReleased())
                context->tick();
            else
            {
                delete context;
                switch (context->getNextContextID())
                {
                    SCREEN_CASES
                default:
                    log_e("Некоректний context_id: %i", context->getNextContextID());
                    esp_restart();
                }
            }
        }
    }
}