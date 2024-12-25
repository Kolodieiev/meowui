#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
//
#include <unordered_map>
//
#include "../driver/graphics/GraphicsDriver.h"
#include "../driver/input/Input.h"
#include "./ResManager.h"
#include "../driver/audio/wav/WavManager.h"
//
#include "./IGameUI.h"
#include "./IGameMenu.h"
#include "./gmap/GameMap.h"
#include "./object/IGameObject.h"
#include "./IdGen.h"

namespace meow
{
    class IGameScene
    {
    public:
        IGameScene(GraphicsDriver &display, Input &input, DataStream &stored_objs);
        virtual ~IGameScene() = 0;

        // Метод, що викликається керуючим екраном кожний кадр
        virtual void update() = 0;

        IGameScene(const IGameScene &rhs) = delete;
        IGameScene &operator=(const IGameScene &rhs) = delete;
        //
        // Службовий метод, необхідний екрану для перевірки стану гри
        bool isFinished() const { return _is_finished; }
        // Службовий метод, необхідний екрану для перевірки стану сцени
        bool isReleased() const { return _is_released; }
        // Службовий метод, який повідомляє екрану ідентифікатор наступої сцени, яка повинна бути відкрита після поточної
        uint8_t getNextSceneID() const { return _next_scene_ID; }

    protected:
        // Мютекс для синхронізації доступу до об'єктів
        SemaphoreHandle_t _obj_mutex;
        // Взяти блокування доступу до об'єктів
        void takeLock() { xSemaphoreTake(_obj_mutex, portMAX_DELAY); }
        // Повернути блокування доступу до об'єктів
        void giveLock() { xSemaphoreGive(_obj_mutex); }

        // Прапор встановлення сцени на паузу
        bool _is_paused{false};
        // Прапор, який вказує, що поточна сцена готова звільнити своє місце для наступної сцени
        bool _is_released{false};
        // Прапор, який повідомляє керуючому екрану, що гру завершено
        bool _is_finished{false};
        //
        uint8_t _next_scene_ID{0};
        //
        IGameUI *_game_UI{nullptr};
        IGameMenu *_game_menu{nullptr};
        // Об'єкт, за яким слідує камера
        IGameObject *_main_obj{nullptr};
        // Самий нижній шар сцени
        GameMap _game_map;
        // Список усіх ігрових об'єктів на сцені, які повинні взаємодіяти один з одним
        std::unordered_map<uint32_t, IGameObject *> _game_objs;
        // Драйвер графіки
        GraphicsDriver &_display;
        // Ввід
        Input &_input;
        // Звуковий менеджер
        WavManager _audio;
        // Менеджер ресурсів
        ResManager _res_manager;

        // Контейнер для перенесення відбитків об'єктів до наступної сцени.
        DataStream &_stored_objs;

        // Знищити поточну сцену і відкрити наступну із вказаним ідентифікатором
        void openSceneByID(uint16_t scene_ID)
        {
            _next_scene_ID = scene_ID;
            _is_released = true;
        }

        // Метод-шаблон для створення ігрових об'єктів
        template <typename T>
        T *createObject()
        {
            try
            {
                return new T(_display, _res_manager, _audio, _game_map, _game_objs);
            }
            catch (const std::bad_alloc &e)
            {
                log_e("%s", e.what());
                esp_restart();
            }
        }

        // Повертає загальний розмір корисних даних усіх об'єктів
        size_t getObjsSize();
        // Записує усі об'єкти на сцені в DataStream
        void serialize(DataStream &ds);
        // Метод-обробник тригерів сцени
        virtual void onTrigger(uint8_t id)
        {
            log_i("Викликано тригер: %d", id);
        }
    };

}