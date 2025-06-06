#include "HeroObj.h"
#include "../ClassID.h"
// Підключити спрайти анімації
// рух вгору
#include "./sprite/move_u/move_u_i.h"
#include "./sprite/move_u/move_u_l.h"
#include "./sprite/move_u/move_u_r.h"
// вниз
#include "./sprite/move_d/move_d_i.h"
#include "./sprite/move_d/move_d_l.h"
#include "./sprite/move_d/move_d_r.h"
// вправо
#include "./sprite/move_r/move_r_i.h"
#include "./sprite/move_r/move_r_l.h"
#include "./sprite/move_r/move_r_r.h"
// вліво
#include "./sprite/move_l/move_l_i.h"
#include "./sprite/move_l/move_l_l.h"
#include "./sprite/move_l/move_l_r.h"
//
#include "../bunny/BunnyObj.h" // Клас об'єкта кролика

namespace simple_rpg
{
    void HeroObj::initAnimVectors()
    {
        // Тут наповнюються вектори анімації кадрами.
        // Кількість векторів і кількість кадрів в них визначає розробник.
        // Кількість кадрів анімації не впливає на продуктивність. Тільки на кількість зайнятого зображеннями місця в постійній пам'яті МК.

        // Анімація руху вгору
        move_up_anim.push_back(HERO_MOVE_UP_L);
        move_up_anim.push_back(HERO_MOVE_UP_R);
        // Рух вниз
        move_down_anim.push_back(HERO_MOVE_D_L);
        move_down_anim.push_back(HERO_MOVE_D_R);
        // Вправо
        move_right_anim.push_back(HERO_MOVE_R_L);
        move_right_anim.push_back(HERO_MOVE_R_R);
        // Вліво
        move_left_anim.push_back(HERO_MOVE_L_L);
        move_left_anim.push_back(HERO_MOVE_L_R);
    }

    void HeroObj::init()
    {
        // Метод, в якому пропонується виконувати ініціалізацію об'єкта.
        // Цей метод потрібно викликати самостійно після створення об'єкта в ігровій сцені.

        _type_ID = CLASS_HERO;           // Встановити ідентифікатор типу
        _sprite.img_ptr = HERO_IDLE_D;    // Встановити зображення спрайта
        _sprite.has_img = true;           // Вказати, що об'єкт має зображення для малювання.
        _sprite.width = 28;               // Ширина спрайта
        _sprite.height = 32;              // Висота спрайта
        _sprite.frames_between_anim = 10; // Скільки ігрових кадрів потрібно пропустити між кадрами анімамції

        _sprite.pass_abillity_mask |= Tile::TYPE_GROUND; // Маска типу прохідності ігрового об'єкта.
                                                         // Дозволяє обмежувати пересування об'єкта по певних видах плиток ігрової мапи

        _sprite.rigid_offsets.top = 20; // Задати зміщення твердого тіла зверху в 20пкс

        initSprite();      // Ініціалізувати спрайт об'єкта
        initAnimVectors(); // Формуємо ветори анімації
    }

    void HeroObj::update()
    {
        // Тут можна визначити логіку поведінки автономних об'єктів.
        // Цей метод викликається автоматично ігровим рушієм.
    }

    void HeroObj::serialize(DataStream &ds)
    {
        // Метод, який потрібен для запису важливих полів об'єкта в потік.
        // Поля для запису обирає розробник гри.

        // Якщо серіалізається використовується в грі з одним гравцем, тоді першим в потік обов'язково повинне записуватися поле
        // з ідентифікатором типу. Інаше першим записується ідентифікатор об'єкта, а за ним ідентифікатор типу.
        // Це не правило, але корисна порада.

        ds.write(&_type_ID, sizeof(_type_ID));
        ds.write(&_bunny_counter, sizeof(_bunny_counter));
    }

    void HeroObj::deserialize(DataStream &ds)
    {
        // Метод, для читання даних з потоку в поля об'єкта.
        // Зчитування відбувається в тому ж порядку, що й запис.

        // Записувати поле _type_ID не потрібно, воно встановлюється самим класом під час створення об'єкта.
        // Байти цього поля з потоку буде читати функція ігрової сцени, що відповідає за перенесення об'єктів.
        // ds.read(&_type_ID, sizeof(_type_ID));

        ds.read(&_bunny_counter, sizeof(_bunny_counter)); // А це відновлення "важливого" поля.
    }

    size_t HeroObj::getDataSize() const
    {
        // Метод повинен повернути справжнє значення розміру полів, які будуть записані в потік.
        return sizeof(_type_ID) +
               sizeof(_bunny_counter);
    }

    void HeroObj::onDraw()
    {
        // Метод для розширення можливостей малювання зображення об'єкта.
        // Може бути використаний, наприклад, для малювання індикатора міцності об'єкта.

        IGameObject::onDraw(); // Необхідно обов'язково викликати базовий метод для коректної відрисовки спрайту чи анімації.
    }

    void HeroObj::moveU()
    {
        // Тут можуть бути переповнення типу по глобальній координаті, але, в даному випадку, в цьому немає нічого страшного,
        // тому що ігровий рівень менший за (максимальне знаачення типу - розмір кроку).
        // Якщо у вашій грі це може мати вплив, додайте додаткові перевірки.

        if (!stepAt(_x_global, _y_global - PIX_PER_STEP))
        {
            disableAnimation(); // Якщо не можемо пройти в заданому напрямку через рельєф, вимикаємо анімацію руху
        }
        else if (_prev_direction != DIRECTION_UP) // Якщо напрямок руху не співпадає з попереднім
        {
            _prev_direction = DIRECTION_UP;
            _sprite.animation_vec = &move_up_anim; // Окрім переміщення об'єкта, змінюємо також його поточну анімацію.
            _sprite.has_animation = true;          // Вмикаємо анімацію
            _sprite.has_img = false;               // Вимикаємо відображення статичного зображення

            // Ще можна скинути лічильник кадрів анімації, якщо вектор довгий, щоб нова анімація починалася з 0 кадру.
            // _sprite.anim_pos = 0;
            // Але в даному випадку це не матиме значення, бо у нас вектор анімації руху має всього 2 кадри.
        }
    }

    void HeroObj::moveD()
    {
        if (!stepAt(_x_global, _y_global + PIX_PER_STEP))
        {
            disableAnimation();
        }
        else if (_prev_direction != DIRECTION_DOWN)
        {
            _prev_direction = DIRECTION_DOWN;
            _sprite.animation_vec = &move_down_anim;
            _sprite.has_animation = true;
            _sprite.has_img = false;
        }
    }

    void HeroObj::moveL()
    {
        if (!stepAt(_x_global - PIX_PER_STEP, _y_global))
        {
            disableAnimation();
        }
        else if (_prev_direction != DIRECTION_LEFT)
        {
            _prev_direction = DIRECTION_LEFT;
            _sprite.animation_vec = &move_left_anim;
            _sprite.has_animation = true;
            _sprite.has_img = false;
        }
    }

    void HeroObj::moveR()
    {
        if (!stepAt(_x_global + PIX_PER_STEP, _y_global))
        {
            disableAnimation();
        }
        else if (_prev_direction != DIRECTION_RIGHT)
        {
            _prev_direction = DIRECTION_RIGHT;
            _sprite.animation_vec = &move_right_anim;
            _sprite.has_animation = true;
            _sprite.has_img = false;
        }
    }

    void HeroObj::stop()
    {
        // Якщо в попередньому кадрі об'єкт персонажа не рухався, то не робимо нічого.
        if (_prev_direction == DIRECTION_NONE)
            return;

        // Вимикаємо анімацію руху
        disableAnimation();

        _prev_direction = DIRECTION_NONE;
    }

    bool HeroObj::stepAt(uint16_t x, uint16_t y)
    {
        // Перевіряємо, чи дозволяє рель'єф пройти в задані координати
        if (_terrain.canPass(_x_global, _y_global, x, y, _sprite))
        {
            // Якщо перевірка успішна - переміщуємо персонажа
            _x_global = x;
            _y_global = y;

            return true;
        }

        return false;
    }

    void HeroObj::disableAnimation()
    {
        // Якщо анімація була вимкнута раніше
        if (!_sprite.has_animation)
            return;

        // Вимикаємо анімацію
        _sprite.has_animation = false;
        // Вмикаємо відображення статичного зображення
        _sprite.has_img = true;

        // Перевіряємо попередній напрямок руху, та встановлюємо відповідне до напрямку руху зображення стану спокою.
        switch (_prev_direction)
        {
        case DIRECTION_UP:
            _sprite.img_ptr = HERO_IDLE_U;
            break;
        case DIRECTION_DOWN:
            _sprite.img_ptr = HERO_IDLE_D;
            break;
        case DIRECTION_LEFT:
            _sprite.img_ptr = HERO_IDLE_L;
            break;
        case DIRECTION_RIGHT:
            _sprite.img_ptr = HERO_IDLE_R;
            break;
        }
    }

    void HeroObj::addBunny()
    {
        try
        {
            // Створюємо об'єкт кролика в об'єкті персонажа
            BunnyObj *bunny = createObject<BunnyObj>();
            bunny->init();
            // Встановлюємо йому позицію персонажа.
            bunny->setPos(_x_global, _y_global);
            // Додаємо його на сцену та забуваємо про нього.
            // Сцена самостійно видалить об'єкти, що містяться в ній.
            _game_objs.emplace(bunny->getId(), bunny);
            // Збільшуємо лічильник кроликів, які були створені персонажем.
            ++_bunny_counter;
        }
        catch (const std::exception &e)
        {
            log_e("Помилка виділення пам'яті %s", e.what());
        }
    }
}