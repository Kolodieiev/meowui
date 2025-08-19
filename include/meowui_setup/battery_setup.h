// #define HAS_BATTERY // Закоментуй, якщо в проєкті відсутній акумулятор або відсутня перевірка його напруги.
#define PIN_VOLT_MEASH 4
#define R_DIV_K 0.7745f // (R2 / (R1 + R2)) +- Корекція ацп та похибка резисторів.
#define VOLTAGE_SAMP_NUM 128