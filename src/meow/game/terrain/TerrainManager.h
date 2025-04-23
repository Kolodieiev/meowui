#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <cmath>
#include <unordered_map>
//
#include "../../driver/graphics/GraphicsDriver.h"
//
#include "Tile.h"

#include "../object/SpriteDescription.h"

namespace meow
{
    class TerrainManager
    {
    public:
        const uint16_t VIEW_W;      // Ширина view-порта
        const uint16_t VIEW_H;      // Висота view-порта
        const uint16_t HALF_VIEW_W; // Використовується для позиціонування об'єктів
        const uint16_t HALF_VIEW_H; // Використовується для позиціонування об'єктів
        //
        TerrainManager() : VIEW_W{_display.width()},
                           VIEW_H{_display.height()},
                           HALF_VIEW_W{static_cast<uint16_t>(_display.width() * 0.5f)},
                           HALF_VIEW_H{static_cast<uint16_t>(_display.height() * 0.5f)} {}

        ~TerrainManager();

        /**
         * @brief Повертає ширину ігрового рівня в пікселях.
         *
         * @return uint16_t
         */
        uint16_t getWidth() const { return _terrain_w; }

        /**
         * @brief  Повертає висоту ігрового рівня в пікселях.
         *
         * @return uint16_t
         */
        uint16_t getHeight() const { return _terrain_h; }

        /**
         * @brief Повертає X-координату верхнього лівого кута view-порта.
         *
         * @return uint16_t
         */
        uint16_t getViewX() const { return _view_x; }

        /**
         * @brief Повертає Y-координату верхнього лівого кута view-порта.
         *
         * @return uint16_t
         */
        uint16_t getViewY() const { return _view_y; }

        /**
         * @brief Малює частину ігрового рівня з указаних координат, якщо можливо.
         * Інакше малює частину ігрового рівня найближчу до цих координат.
         *
         */
        void onDraw();

        /**
         * @brief Встановлює вказівник на фонове зображення ігрового рівня.
         * Дані за вказівником не будуть видалені автоматично разом з ігровим рівнем.
         * Очікується, що розмір зображення буде не меншим за розмір дисплею.
         *
         * @param img_ptr Вказівник на фонове зображення.
         */
        void setBackImgPtr(const uint16_t *img_ptr) { _back_img = img_ptr; }

        /**
         * @brief Додає опис окремої плитки.
         * Дані за вказівником sprite_src не будуть видалені автоматично.
         *
         * @param sprite_id Ідентифікатор плитки.
         * @param type Тип прохідності плитки.
         * @param sprite_src Зображення плитки.
         */
        void addTileDesc(uint16_t sprite_id, Tile::TileType type, const uint16_t *sprite_src);

        /**
         * @brief Скидає опис плиток.
         * Необхідно викликати перед додаванням нових плиток, якщо потрібно перемалювати ігровий рівень в межах однії сцени.
         *
         */
        void clearTilesDesc() { freeTilesDescriptionData(); }

        /**
         * @brief Створює поверхню ігрового рівня на основі опису плиток та таблиці розміщення плиток.
         * Ігровий рівень повиннен бути не меншим за розмір дисплея, в іншому разі, під час малювання ігрового рівня, виникне помилка.
         * Опис плиток повинен бути доданий до виклику цього методу.
         *
         * @param tiles_w_num Кількість плиток в ширину.
         * @param tiles_h_num Кількість плиток у висоту.
         * @param tile_side_len Розмір сторони плитки. Плитки повинні мати квадратну форму.
         * @param tiles_pos_templ Вказівник на масив, що містить шаблон ігрової сцени.
         */
        void build(uint16_t tiles_w_num, uint16_t tiles_h_num, uint16_t tile_side_len, const uint16_t *tiles_pos_templ);

        /**
         * @brief Перевіряє, чи може об'єкт зі вказаним спрайтом бути переміщеним в указані координати ігрового рівня.
         *
         * @param x_from Х-координата, звідки повинне виконуватися переміщення.
         * @param y_from Y-координата, звідки повинне виконуватися переміщення.
         * @param x_to Х-координата, куди повинне виконуватися переміщення.
         * @param y_to Y-координата, куди повинне виконуватися переміщення.
         * @param sprite Спрайт ігрового об'єкта.
         * @return true - Якщо об'єкт може бути переміщено в указані координатах ігрового рівня. false - інакше.
         */
        bool canPass(uint16_t x_from, uint16_t y_from, uint16_t x_to, uint16_t y_to, const SpriteDescription &sprite);

        /**
         * @brief Повертає тип плитки у вказаних координатах.
         *
         * @param x Координата плитки.
         * @param y Координата плитки.
         * @return Tile::TileType тип плитки у вказаних координатах. Tile::TYPE_NONE - якщо плитка за вказаними координатами відсутня.
         */
        Tile::TileType getTileType(uint16_t x, uint16_t y);

        /**
         * @brief Встановлює позицію камери за вказаними координатами.
         *
         * @param x Координата камери.
         * @param y Координата камери.
         */
        void setCameraPos(uint16_t x, uint16_t y);

        /**
         * @brief Перевіряє чи потрапляє хоча б частина спрайту на зображення кадру.
         *
         * @param x_pos X - Координата спрайта.
         * @param y_pos Y - Координата спрайта.
         * @param sprite_w Ширина спрайта.
         * @param sprite_h Висота спрайта.
         * @return true - Якщо спрайт частково чи повністю потрапляє на зображення кадру. false - інакше.
         */
        bool isInView(uint16_t x_pos, uint16_t y_pos, uint16_t sprite_w, uint16_t sprite_h);

    private:
        uint16_t _view_x{0};                              // X верхнього лівого кута view-порта
        uint16_t _view_y{0};                              // Y верхнього лівого кута view-порта
        uint16_t _terrain_w{0};                           // Ширина ігрового рівня в пікселях
        uint16_t _terrain_h{0};                           // Висота ігрового рівня в пікселях
        Tile ***_terrain{nullptr};                        // Набір плиток ігрового рівня
        std::unordered_map<uint16_t, Tile *> _tile_descr; // Опис плиток
        const uint16_t *_back_img{nullptr};               // Фонове зображення
        uint8_t _tile_side_len{0};                        // Розмір сторони плитки
        uint16_t _tile_x_num{0};                          // Кількість плиток мапи по горизонталі
        uint16_t _tile_y_num{0};                          // Кількість плиток мапи по вертикалі

        void freeMem();
        void freeTilesDescriptionData();

        uint16_t coordToTilePos(uint16_t coord);
    };
}