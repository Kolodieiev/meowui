#include <Arduino.h>
#include "meow/manager/context/ContextManager.h"

void guiTask(void *params)
{
    meow::MEOW.run();
}

void setup()
{
    xTaskCreatePinnedToCore(guiTask, "guiTask", (1024 / 2) * 50, NULL, 10, NULL, 1);
}

void loop()
{
    vTaskDelete(NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
