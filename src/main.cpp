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
}
