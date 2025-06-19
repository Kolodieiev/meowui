#pragma once
#include <stdint.h>

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

namespace meow
{
    enum BtnID : uint8_t
    {
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

        // BTN_UP = 0, // Для швидкого тесту на випадок, якщо плата не має кнопок.
        // BTN_LEFT = 0,
        // BTN_RIGHT = 0,
        // BTN_DOWN = 0,

        // BTN_D = 0,
        // BTN_A = 0,
        // BTN_C = 0,
        // BTN_B = 0,

        // BTN_SAVE = 0, // START
        // BTN_EXIT = 0, // SELECT
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

#define PRESS_DURATION (unsigned long)1500 // Час, після спливання якого дія вважається утримуваною.

#define PRESS_LOCK (unsigned long)700
#define CLICK_LOCK (unsigned long)250
#define HOLD_LOCK (unsigned long)150
