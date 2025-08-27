#include <Arduino.h>
#include "meow/Meowui.h"

void setup()
{
    // Запустити виконання Meowui.
    meow::MEOW.begin(80);
}

void loop()
{
    vTaskDelete(NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
