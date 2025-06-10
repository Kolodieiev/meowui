#include <Arduino.h>
#include "meow/manager/context/ContextManager.h"

void guiTask(void *params)
{
    meow::MEOW.run();
}

void setup()
{
    // ESP_IDF_VERSION_MAJOR;
    // ESP_ARDUINO_VERSION_MAJOR;
    xTaskCreatePinnedToCore(guiTask, "guiTask", (1024 / 2) * 50, NULL, 10, NULL, 1);
}

void loop()
{
    vTaskDelete(NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
