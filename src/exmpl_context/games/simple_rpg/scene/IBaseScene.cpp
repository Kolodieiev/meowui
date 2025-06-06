#include "IBaseScene.h"
#include "../obj/ClassID.h" // Ідентифікатори класів об'єктів

namespace simple_rpg
{
    IBaseScene::IBaseScene(DataStream &stored_objs)
        : IGameScene(stored_objs)
    {
        // Ініціалізуємо рандом. Його використовують зайці під час вибору напрямку руху.
        randomSeed(esp_random());

        // Відновлюємо об'єкти з попрередньої сцени в поточній
        deserialize(stored_objs);
        createHeroObj();
        createPortalObj();

        // Створюємо графічний інтерфейс користувача
        ui = new BaseUI();

        // Прикріплюємо створений UI до сцени
        _game_UI = ui;

        // Встановлюємо кількість кроликів, які було створено сумарно.
        // Дані лічильника перенесені через потік між сценами і десеріалізовані вище в коді.
        ui->updBunnyNum(_hero->getBunnyNum());

        // Тут можна ініціалізувати та додати до сцени ігрове меню, так само, як це відбувається для UI.
    }

    IBaseScene::~IBaseScene()
    {
        // Перед видаленням сцени зберігаємо дані, які потрібно перенести до іншої сцени

        size_t obj_data_size = getObjsSize(); // Отримуємо розмір даних, які потрібно перенести
        _stored_objs.resize(obj_data_size);   // Виділяємо місце під дані
        serialize(_stored_objs);              // Записуємо дані в потік
    }

    void IBaseScene::update()
    {
        // Обробка кнопки виходу
        if (_input.isPressed(BtnID::BTN_EXIT))
        {
            _input.lock(BtnID::BTN_EXIT, 300);
            _is_finished = true; // Піднімаємо прапор, який означає, що
            return;
        }

        // Якщо гра не на паузі, виконуємо звичайні для сцени речі
        if (!_is_paused)
        {
            // Оброблюємо утримання однієї з кнопок джойстика для переміщення персонажа
            // Зверніть увагу, що тут відсутнє блокування кнопок.
            // Це потрібно, щоб стан джойстика оброблювався кожен кадр
            if (_input.isHolded(BtnID::BTN_UP))
            {
                _hero->moveU();
            }
            else if (_input.isHolded(BtnID::BTN_DOWN))
            {
                _hero->moveD();
            }
            else if (_input.isHolded(BtnID::BTN_RIGHT))
            {
                _hero->moveR();
            }
            else if (_input.isHolded(BtnID::BTN_LEFT))
            {
                _hero->moveL();
            }
            else
            {
                _hero->stop(); // Передаємо персонажу відсутність руху, щоб він міг зупинити свою анімацію
            }

            if (_input.isReleased(BtnID::BTN_A))
            {
                // Оброблюємо натискання кнопки А для створення нового об'єкту зайця

                _input.lock(BtnID::BTN_A, 300);
                _hero->addBunny();

                // Оновлюємо відображення кількості зайців в UI.
                ui->updBunnyNum(_hero->getBunnyNum());
            }
            else if (_input.isReleased(BtnID::BTN_D))
            {
                // Ставимо гру на паузу якщо натискалась кнопка D

                _input.lock(BtnID::BTN_D, 300);
                _is_paused = true;
            }

            IGameScene::update(); // Необхідно обов'язково перевикликати метод кожен кадр у базовму класі. Інакше сцена не буде перемальовуватися.
        }
        else
        {
            // Інакше якщо гра на паузі оброблюємо кнопки як це потрібно для ігрового меню
            // Ігрове меню не є обов'язковим для реалізації.
            // Це лише приклад того, як можна оброблювати його стан.

            // Тут оброблюємо натискання кнопок управління меню.

            // У даному прикладі меню не створене, тому оброблюємо тільки одну кнопку, для зняття гри з паузи
            if (_input.isReleased(BtnID::BTN_B))
            {
                // Якщо натискалась кнопка B знімаємо гру з паузи

                _input.lock(BtnID::BTN_B, 300);
                _is_paused = false;
            }
        }
    }

    void IBaseScene::createHeroObj()
    {
        // Перевіряємо, чи було створено десеріалізатором об'єкт персонажа раніше
        if (!_hero)
        {
            // Якщо об'єкт не було створено, наприклад, під час першого завантаження гри, то
            // створюємо об'єкт персонажа
            _hero = createObject<HeroObj>();
            // Обов'язково викликаємо його ініціалізацію
            _hero->init();
            // Додаємо його на сцену
            _game_objs.emplace(_hero->getId(), _hero);
        }

        // Вказуємо камері, що цей об'єкт є головним на сцені
        _main_obj = _hero;
    }

    void IBaseScene::createPortalObj()
    {
        _portal = createObject<PortalObj>();
        _portal->init();
        //
        _game_objs.emplace(_portal->getId(), _portal);
    }

    void IBaseScene::deserialize(DataStream &stored_objs)
    {
        //  Означає що потік не містить даних об'єктів
        if (stored_objs.length() < 2)
            return;

        uint8_t class_id;
        stored_objs.read(&class_id, sizeof(class_id)); // Читаємо з потоку ідентифікатор класу об'єкта, якому належать дані

        switch (class_id)
        {
        case CLASS_HERO:
            createHeroObj();
            _hero->deserialize(stored_objs); // Десеріалізуємо поля з потоку
            break;

        default:
            log_e("Невідомий class_id: %u", class_id);
            esp_restart();
            break;
        }

        stored_objs.resize(1); // Скидаємо буфер після відновлення, щоб він дарма не займав місце у пам'яті
    }
}