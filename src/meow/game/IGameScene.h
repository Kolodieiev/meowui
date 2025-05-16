#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
//
#include <unordered_map>
#include <vector>
//
#include "meow/driver/graphics/GraphicsDriver.h"
#include "meow/driver/input/Input.h"
#include "meow/lib/audio/wav/WavManager.h"
#include "meow/manager/resources/ResManager.h"

//
#include "./IGameUI.h"
#include "./IGameMenu.h"
#include "./terrain/TerrainManager.h"
#include "./object/IGameObject.h"

namespace meow
{
    class IGameScene
    {
    public:
        IGameScene(DataStream &stored_objs);
        virtual ~IGameScene() = 0;

        /**
         * @brief Забезпечує оновлення стану гри.
         * Викликається контекстом кожен кадр.
         *
         */
        virtual void update() = 0;

        IGameScene(const IGameScene &rhs) = delete;
        IGameScene &operator=(const IGameScene &rhs) = delete;

        /**
         * @brief Повертає стан прапору, який вказує на те, чи повинна бути завершена гра.
         * Зазвичай, викликається керуючим контекстом.
         *
         * @return true - Якщо гра повинна бути завершена. false - Інакше.
         */
        bool isFinished() const { return _is_finished; }

        /**
         * @brief Повертає стан прапору, який вказує на те, чи повинен бути зміненений поточний ігровий рівень.
         * Зазвичай, викликається керуючим контекстом.
         *
         * @return true - Якщо ігровий рівень повинен бути змінений. false - Інакше.
         */
        bool isReleased() const { return _is_released; }

        /**
         * @brief Повертає ідентифікатор ігрового рівня, який повинен бути запущений наступним.
         * Зазвичай, викликається керуючим контекстом.
         *
         * @return uint8_t
         */
        uint8_t getNextSceneID() const { return _next_scene_ID; }

    protected:
        SemaphoreHandle_t _obj_mutex;                           // Мютекс для синхронізації доступу до об'єктів
        bool _is_paused{false};                                 // Прапор встановлення сцени на паузу
        bool _is_released{false};                               // Прапор, який вказує, що поточна сцена готова звільнити своє місце для наступної сцени
        bool _is_finished{false};                               // Прапор, який повідомляє керуючому контексту, що гру завершено
        uint8_t _next_scene_ID{0};                              // Ідентифікатор наступної сцени
        IGameUI *_game_UI{nullptr};                             // Шар ігрового UI. Тут можуть виводитися графічні елементи інтерфейса
        IGameMenu *_game_menu{nullptr};                         // Шар ігрового меню, якщо в ньому є необхідність
        IGameObject *_main_obj{nullptr};                        // Об'єкт, за яким завжди слідує камера
        TerrainManager _terrain;                                // Самий нижній шар сцени
        std::unordered_map<uint32_t, IGameObject *> _game_objs; // Список усіх ігрових об'єктів на сцені, які повинні взаємодіяти один з одним
        WavManager _audio;                                      // Звуковий менеджер
        ResManager _res_manager;                                // Менеджер ресурсів
        DataStream &_stored_objs;                               // Контейнер для перенесення відбитків об'єктів до наступної сцени

        /**
         * @brief Блокує мютекс доступу до ігрових об'єктів.
         *
         */
        void takeLock() { xSemaphoreTake(_obj_mutex, portMAX_DELAY); }

        /**
         * @brief Відпускає мютекс доступу до ігрових об'єктів.
         *
         */
        void giveLock() { xSemaphoreGive(_obj_mutex); }

        /**
         * @brief Піднімає прапор, який вказує, що поточний ігровий рівень повинен бути змінений.
         * Встановлює ідентифікатро ігрового рівня, який повинен бути створений наступним.
         *
         * @param scene_ID Ідентифікатор ігрового рівня, що повинен бути створений наступним.
         */
        void openSceneByID(uint16_t scene_ID);

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

        /**
         * @brief Повертає загальний розмір даних ігрових об'єктів, які можуть бути серіалізовані.
         *
         * @return size_t
         */
        size_t getObjsSize();

        /**
         * @brief Записує усі об'єкти на сцені в DataStream
         *
         * @param ds Об'єкт DataStream, куди будуть серіалізовані об'єкти сцени.
         */
        void serialize(DataStream &ds);

        /**
         * @brief Якщо не перевантажено, слугує заглушкою для тригерів об'єктів.
         * Інакше буде викликано у об'єкта спадкоємця.
         *
         * @param id Ідентифікатор тригера.
         */
        virtual void onTrigger(uint8_t trigg_id)
        {
            log_i("Викликано тригер: %d", trigg_id);
        }
    };
}
