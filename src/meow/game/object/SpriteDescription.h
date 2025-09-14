#pragma once
#include <Arduino.h>
#include <vector>
#include "../../driver/graphics/GraphicsDriver.h"

namespace meow
{
    struct BodyOffsets
    {
        uint16_t top{0};    // Зміщення від верхнього краю.
        uint16_t bottom{0}; // Зміщення від нижнього краю.
        uint16_t left{0};   // Зміщення від лівого краю.
        uint16_t right{0};  // Зміщення від правого краю.
    };

    struct SpriteDescription
    {
        BodyOffsets rigid_offsets{}; // Опис зміщення жорсткого тіла об'єкта від країв його спрайту.

        const uint16_t *img_ptr{nullptr};                      // Вказівник на массив із зображенням, якщо відсутня анімація
        std::vector<const uint16_t *> *animation_vec{nullptr}; // Вектор анімації

        uint16_t width{1};             // Ширина спрайту
        uint16_t height{1};            // Висота спрайту
        uint16_t transp_color{0xF81F}; // Колір прозорості спрайту
        uint16_t x_pivot{1};           // Х-координата логічного центру спрайта
        uint16_t y_pivot{1};           // Y-координата логічного центра спрайта
        int16_t angle{0};              // Кут повороту спрайта

        uint8_t frames_between_anim{0}; // Кількість пропущених кадрів між кадрами анімації
        uint8_t frames_counter{0};      // Лічильник кадрів
        uint8_t anim_pos{0};            // Номер поточного кадру анімації
        uint8_t pass_abillity_mask{0};  // Маска, яка вказує, по якому типу поверхні може переміщуватися об'єкт.

        bool is_rigid{true};       // Чи має об'єкт тверде тіло.
        bool has_img{false};       // Чи має об'єкт зображення для спрайта
        bool has_animation{false}; // Чи має об'єкт анімацію спрайта

        // Скидає лічильник кадрів, та лічильник поточної анімації
        void reset()
        {
            frames_counter = 0;
            anim_pos = 0;
        }
    };
}