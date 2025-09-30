#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <unordered_map>
//
#include "../../driver/graphics/GraphicsDriver.h"
#include "../../lib/audio/wav/WavManager.h"
#include "meow/manager/ResManager.h"
//
#include "../DataStream.h"
//
#include "../terrain/TerrainManager.h"
//
#include "SpriteDescription.h"

#include "../../lib/audio/wav/WavTrack.h"

namespace meow
{
    class IGameScene;

    class IGameObject
    {
    public:
        enum MovingDirection : uint8_t
        {
            DIRECTION_NONE,
            DIRECTION_UP,
            DIRECTION_DOWN,
            DIRECTION_LEFT,
            DIRECTION_RIGHT
        };

        IGameObject(const IGameObject &rhs) = delete;
        IGameObject &operator=(const IGameObject &rhs) = delete;

        IGameObject(ResManager &res,
                    WavManager &audio,
                    TerrainManager &terrain,
                    std::unordered_map<uint32_t, IGameObject *> &game_objs);
        virtual ~IGameObject() = 0;

        /**
         * @brief Повертає ідентифікатор об'єкта.
         *
         * @return uint32_t
         */
        uint32_t getId() const { return _obj_ID; }

        /**
         * @brief Повертає ідентифікатор типу об'єкта.
         * Необхідний для заміни instanceof.
         *
         * @return int16_t
         */
        int16_t getTypeID() const { return _type_ID; }

        /**
         * @brief Повертає вказівник на рядок з ім'ям об'єкта, якщо було задано. Інакше на порожій рядок.
         *
         * @return const char*
         */
        const char *getName() const { return _name.c_str(); }

        /**
         * @brief Встановлює глобальну позицію об'єкта на ігровій сцені.
         *
         * @param x_pos Координата.
         * @param y_pos Координата.
         */
        void setPos(uint16_t x_pos, uint16_t y_pos);

        /**
         * @brief Повертає стан прапору, який вказує чи було об'єкт знищено в попередньому кадрі.
         * Зазвичай використовується сценою для очищення мертвих об'єктів з ігрового рівня.
         * Може бути використаний для запобігання взаємодії з потенційно мертвим об'єктом.
         *
         * @return true - Якщо об'єкт потенційно мертвий. false - Інакше.
         */
        bool isDestroyed() const { return _is_destroyed; }

        /**
         * @brief Повертає реальний розмір даних об'єкта, які будуть серіалізовані.
         *
         * @return size_t
         */
        virtual size_t getDataSize() const = 0;

        /**
         * @brief Серіалізує об'єкт в DataStream
         *
         * @param ds Об'єкт DataStream, куди буде серіалізовано дані.
         */
        virtual void serialize(DataStream &ds) = 0;

        /**
         * @brief Десеріаізує об'єкт із DataStream
         *
         * @param ds Об'єкт DataStream, з якого будуть прочитані дані у відповідні поля об'єкта.
         */
        virtual void deserialize(DataStream &ds) = 0;

    protected:
        /**
         * @brief Метод, в якому рекомендується ралізовувати ініціалізацію об'єкта.
         *
         */
        virtual void init() = 0;

        /**
         * @brief Забезпечує оновлення стану об'єкта. Викликається автоматично ігровим рівнем.
         *
         */
        virtual void update() = 0;

        /**
         * @brief Перемальовує об'єкт кожен кадр, якщо у нього задано зображення або анімацію.
         *
         */
        virtual void onDraw();

        /**
         * @brief Створює спрайт об'єкта.
         * Повинен бути викликаний тільки після заповнення полів об'єкта _sprite, що описує спрайт об'єкта.
         *
         */
        void initSprite();

        /**
         * @brief Повертає список усіх об'єктів на сцені з заданим ідентифікатором класу.
         *
         * @param type_ID Ідентифікатор класу.
         * @return std::list<IGameObject *>
         */
        std::list<IGameObject *> getObjByClass(uint8_t type_ID);

        /**
         * @brief Повертає список усіх об'єктів, що знаходяться в заданій точці і мають відповідний ідентифікатор класу.
         *
         * @param type_ID Ідентифікатор класу.
         * @param x Координата точки.
         * @param y Координата точки.
         * @return std::list<IGameObject *>
         */
        std::list<IGameObject *> getObjByClassAt(uint8_t type_ID, uint16_t x, uint16_t y);

        /**
         * @brief Повертає список усіх об'єктів, що знаходяться в заданому прямокутнику і мають відповідний ідентифікатор класу.
         *
         * @param type_ID Ідентифікатор класу.
         * @param x_start Координата верхнього лівого кута прямокутника.
         * @param y_start Координата верхнього лівого кута прямокутника.
         * @param rect_width Ширина прямокутника.
         * @param rect_height Висота прямокутника.
         * @return std::list<IGameObject *>
         */
        std::list<IGameObject *> getObjByClassInRect(uint8_t type_ID, uint16_t x_start, uint16_t y_start, uint16_t rect_width, uint16_t rect_height);

        /**
         * @brief Повертає список усіх об'єктів, що знаходяться в колі із заданим радіусом навколо об'єкта і мають відповідний ідентифікатор класу.
         *
         * @param type_ID Ідентифікатор класу.
         * @param radius Радіус пошуку
         * @return std::list<IGameObject *>
         */
        std::list<IGameObject *> getObjByClassInRadius(uint8_t type_ID, uint16_t radius);

        /**
         * @brief Повертає список об'єктів, відповідно до фільтра, які перетинаються з заданою точкою.
         *
         * @param x Координата точки.
         * @param y Координата точки.
         * @param rigid_only Фільтр, який вказучє чи потрібно відбирати тільки об'єкти, які мають тверде тіло.
         * @return std::list<IGameObject *>
         */
        std::list<IGameObject *> getObjAt(uint16_t x, uint16_t y, bool rigid_only = false);

        /**
         * @brief Повертає об'єкти ігрового рівня, відповідно до фільтра, що знаходяться в колі з заданим радіусом.
         *
         * @param x_mid Центральна координата кола.
         * @param y_mid Центральна координата кола.
         * @param radius Радіус кола.
         * @param rigid_only Фільтр, який вказучє чи потрібно відбирати тільки об'єкти, які мають тверде тіло.
         * @return std::list<IGameObject *>
         */
        std::list<IGameObject *> getObjInCircle(uint16_t x_mid, uint16_t y_mid, uint16_t radius, bool rigid_only = false);

        /**
         * @brief Повертає об'єкти ігрового рівня, відповідно до фільтра, що знаходяться в заданому прямокутнику.
         *
         * @param x_start Координата верхнього лівого кута прямокутника.
         * @param y_start Координата верхнього лівого кута прямокутника.
         * @param rect_width Ширина прямокутника.
         * @param rect_height Висота прямокутника.
         * @param rigid_only Фільтр, який вказучє чи потрібно відбирати тільки об'єкти, які мають тверде тіло.
         * @return std::list<IGameObject *>
         */
        std::list<IGameObject *> getObjInRect(uint16_t x_start, uint16_t y_start, uint16_t rect_width, uint16_t rect_height, bool rigid_only = false);

        /**
         * @brief Перевіряє чи пересікається об'єкт з заданою точкою.
         *
         * @param x Координата точки.
         * @param y Координата точки.
         * @param rigid_only Задає тип перевірки. Якщо true буде перевірятися тільки пересічення з жорстким тілом. Інакше зі спрайтом.
         * @return true - Якщо пересікається.
         * @return false - Інакше.
         */
        bool hasIntersectWithPoint(uint16_t x, uint16_t y, bool rigid_only = false) const;

        /**
         * @brief Перевіряє чи пересікається об'єкт з заданим колом.
         *
         * @param x_mid Центральна координата кола.
         * @param y_mid Центральна координата кола.
         * @param radius Радіус кола.
         * @param rigid_only Задає тип перевірки. Якщо true буде перевірятися тільки пересічення з жорстким тілом. Інакше зі спрайтом.
         * @return true - Якщо пересікається.
         * @return false - Інакше.
         */
        bool hasIntersectWithCircle(uint16_t x_mid, uint16_t y_mid, uint16_t radius, bool rigid_only = false) const;

        /**
         * @brief Перевіряє чи пересікається об'єкт з заданим прямокутником.
         *
         * @param x_start Координата верхнього лівого кута прямокутника.
         * @param y_start Координата верхнього лівого кута прямокутника.
         * @param rect_width Ширина прямокутника.
         * @param rect_height Висота прямокутника.
         * @param rigid_only Задає тип перевірки. Якщо true буде перевірятися тільки пересічення з жорстким тілом. Інакше зі спрайтом.
         * @return true - Якщо пересікається.
         * @return false - Інакше.
         */
        bool hasIntersectWithRect(uint16_t x_start, uint16_t y_start, uint16_t rect_width, uint16_t rect_height, bool rigid_only = false) const;

        /**
         * @brief Перевіряє чи матиме тверде тіло об'єкта пересічення з будь-яким іншим твердим тілом під час кроку в задані координати.
         *
         * @param x_to Координата.
         * @param y_to Координата.
         * @return true - Якщо об'єкт матиме пересічення.
         * @return false - Інакше.
         */
        bool hasCollisionAt(uint16_t x_to, uint16_t y_to);

        /**
         * @brief Розраховує кут від pivot об'єкта, до вказаної точки.
         *
         * @param x Координата точки.
         * @param y Координата точки.
         * @return uint16_t
         */
        uint16_t calcAngleToPoint(uint16_t x, uint16_t y);

        /**
         * @brief Розраховує відстань від pivot об'єкта до вказаної точки
         *
         * @param x Координата точки.
         * @param y Координата точки.
         * @return uint16_t
         */
        uint16_t calcDistToPoint(uint16_t x, uint16_t y);

        /**
         * @brief Переміщує об'єкт на вказану кількість пікселів в сторону указаної точки.
         *
         * @param x_to Координата точки.
         * @param y_to Координата точки.
         * @param step_w Розмір кроку в пікселях.
         */
        void stepToPoint(uint16_t x_to, uint16_t y_to, uint16_t step_w);

        /**
         * @brief Створює об'єкт вказаного типу.
         *
         * @tparam T Тип об'єкта, який потрібно створити.
         * @return T* Вказівний на створений об'єкт.
         */
        template <typename T>
        T *createObject()
        {
            try
            {
                return new T(_res_manager, _audio, _terrain, _game_objs);
            }
            catch (const std::bad_alloc &e)
            {
                log_e("%s", e.what());
                esp_restart();
            }
        }

    private:
        friend class IGameScene;
        static uint32_t _global_obj_id_counter; // Глобальний лічильник ідентифікаторів об'єктів.

    private:
        TFT_eSprite _obj_sprite; 

    protected:
        ResManager &_res_manager;                                // Менеджер ресурсів 
        TerrainManager &_terrain;                                // Поверхня ігрового рівня 
        WavManager &_audio;                                      // Менеджер аудіо 
        std::unordered_map<uint32_t, IGameObject *> &_game_objs; // Список ігрових об'єктів на сцені
        SpriteDescription _sprite{};                             // Об'єкт структури, яка описує спрайт об'єкта та його стани

        String _name; // Ім'я об'єкта, може не використовуватися

    private:
        int32_t _x_local{0}; // Координата X відносно дисплея
        int32_t _y_local{0}; // Координата Y відносно дисплея

    protected:
        const uint32_t _obj_ID; // Ідентифікатор об'єкта. Може не використовуватися в локальній грі.

    public:
        uint16_t _x_global{0}; // Координата Х відносно ігрового рівня
        uint16_t _y_global{0}; // Координата Y відносно ігрового рівня

    protected:
        uint8_t _type_ID{0};    // Ідентифікатор типу об'єкта
        uint8_t _trigger_ID{0}; // Ідентифіктор тригера. Може не використовуватися, якщо об'єкт не тригериться.
        uint8_t _layer{0};      // Шар сортування об'єкта по осі Z. Чим більше значення, тим вище шар

        bool _is_triggered{false}; // Прапор спрацювання тригера об'єкта.  Може не використовуватися, якщо об'єкт не тригериться.
        bool _is_destroyed{false}; // Прапор знищення об'єкта іншими об'єктами або сценою
    };

}
