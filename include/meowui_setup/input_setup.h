#pragma once
#include <stdint.h>

// #define EXT_INPUT // Якщо стан кнопок зчитується з іншого МК по шині I2C.

#ifdef EXT_INPUT
#define EXT_INPUT_ADDR 0x66 // I2C - адреса МК, який відповідає за зчитування стану кнопок.
#define EXT_INPUT_B_NUM 1   // Кількість байт(які будуть прочитані по I2C) що містять інформацію про стан кнопок. 1 байт == 8 кнопок.
#endif

// #define TOUCHSCREEN_SUPPORT // Увімкнути підтримку сенсорного екрану

#ifdef TOUCHSCREEN_SUPPORT
#define TOUCH_ROTATION ROTATION_0
#define TOUCH_WIDTH 320
#define TOUCH_HEIGHT 480

// -------------------------------- Обрати драйвер сенсорного екрану
#define GT911_DRIVER // Поки що додано підтримку тільки 1 чіпа

// -------------------------------- Налаштуй піни, до яких підключено драйвер тачскріна
#ifdef GT911_DRIVER
#define TOUCH_SDA_PIN 33
#define TOUCH_SCL_PIN 32
#define TOUCH_INT_PIN 21
#define TOUCH_RST_PIN 25
#endif // GT911_DRIVER

#endif // TOUCHSCREEN_SUPPORT

// -------------------------------- Закріплення віртуальних назв кнопок до виводів МК
// Якщо стан кнопок зчитується з іншого МК, тоді ідентифікатором віртуальної кнопки має бути позиція біта(зліва направо) в масиві байт.

namespace meow
{
    enum BtnID : uint8_t
    {
        // BTN_UP = 0,
        // BTN_DOWN = 1,
        // BTN_LEFT = 2,
        // BTN_RIGHT = 3,

        // BTN_D = 4,
        // BTN_A = 5,
        // BTN_C = 6,
        // BTN_B = 7,

        // BTN_SAVE = 8, // START
        // BTN_EXIT = 9, // SELECT

        BTN_UP = 38,
        BTN_LEFT = 39,
        BTN_RIGHT = 40,
        BTN_DOWN = 41,

        BTN_D = 9,
        BTN_A = 5,
        BTN_C = 10,
        BTN_B = 6,

        BTN_SAVE = 4, // START
        BTN_EXIT = 0, // SELECT
    };
}

// -------------------------------- Макрос з ініціалізацією пінів. Між фігурними дужками потрібно додати ініціалізацію пінів, як показано нижче
// -------------------------------- Де Button(uint8_t key_id, bool is_touch)
#define BUTTONS                                    \
    {                                              \
        {BTN_UP, new Button(BTN_UP, false)},       \
        {BTN_DOWN, new Button(BTN_DOWN, false)},   \
        {BTN_LEFT, new Button(BTN_LEFT, false)},   \
        {BTN_RIGHT, new Button(BTN_RIGHT, false)}, \
        {BTN_D, new Button(BTN_D, false)},         \
        {BTN_A, new Button(BTN_A, false)},         \
        {BTN_C, new Button(BTN_C, false)},         \
        {BTN_B, new Button(BTN_B, false)},         \
        {BTN_SAVE, new Button(BTN_SAVE, false)},   \
        {BTN_EXIT, new Button(BTN_EXIT, false)},   \
    }

// -------------------------------- Налаштування чутливості сенсорних кнопок
#if defined(CONFIG_IDF_TARGET_ESP32S3)
#define BTN_TOUCH_TRESHOLD 50000
#else
#define BTN_TOUCH_TRESHOLD 65
#endif

#define PRESS_DURATION (unsigned long)1000 // Час, після спливання якого кнопка вважається утримуваною.

#define PRESS_LOCK (unsigned long)700
#define CLICK_LOCK (unsigned long)250
#define HOLD_LOCK (unsigned long)150
