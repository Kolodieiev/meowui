#pragma GCC optimize("O3")
#include "ContextManager.h"
#include "meowui_setup/display_setup.h"
#include "meow/manager/spi/SPI_Manager.h"
#include "../../ui/context/IContext.h"

#include "meowui_setup/context_id.h"
#include "meowui_setup/ui_setup.h"

namespace meow
{
    void ContextManager::run()
    {
#ifdef GRAPHICS_ENABLED
        SPI_Manager::initBus(HSPI, TFT_SCLK, TFT_MISO, TFT_MOSI);
        _display.init();
#endif
        _input._init();

        IContext *context = new START_CONTEXT();

        // Основний цикл GUI.
        while (1)
        {
            if (!context->isReleased())
            {
                context->tick();
            }
            else
            {
                ContextID next_context_id = context->getNextContextID();
                delete context;

                auto it = _context_id_map.find(next_context_id);
                if (it == _context_id_map.end())
                {
                    log_e("Невідомий ідентифікатор контексту: %u", next_context_id);
                    esp_restart();
                }
                else
                {
                    context = it->second();
                }
            }
        }
    }
}