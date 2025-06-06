#include "BallObj.h"
#include "TriggerID.h"
#include "ClassID.h"

namespace test_server
{
    //
    void BallObj::init()
    {
        _sprite.has_img = true; // Вказати, що об'єкт може малювати свій спрайт
        _sprite.width = 32;     // Ширина спрайта
        _sprite.height = 32;    // Висота спрайта

        _sprite.pass_abillity_mask |= Tile::TYPE_GROUND; // Маска типу прохідності ігрового об'єкта.
        initSprite();                                    // Ініціалізувати спрайт об'єкта
    }

    void BallObj::update()
    {
    }

    void BallObj::serialize(DataStream &ds)
    {
        ds.write(&_obj_ID, sizeof(_obj_ID));
        ds.write(&_type_ID, sizeof(_type_ID));
        ds.write(&_x_global, sizeof(_x_global));
        ds.write(&_y_global, sizeof(_y_global));
        ds.write(&_is_triggered, sizeof(_is_triggered));
        ds.write(&_is_destroyed, sizeof(_is_destroyed));
    }

    void BallObj::deserialize(DataStream &ds)
    {
        ds.read(const_cast<uint32_t*>(&_obj_ID), sizeof(_obj_ID)); // Зняття константності для запису ID
        ds.read(&_type_ID, sizeof(_type_ID));
        ds.read(&_x_global, sizeof(_x_global));
        ds.read(&_y_global, sizeof(_y_global));
        ds.read(&_is_triggered, sizeof(_is_triggered));
        ds.read(&_is_destroyed, sizeof(_is_destroyed));
    }

    size_t BallObj::getDataSize() const
    {
        return sizeof(_obj_ID) +
               sizeof(_type_ID) +
               sizeof(_x_global) +
               sizeof(_y_global) +
               sizeof(_is_triggered) +
               sizeof(_is_destroyed);
    }

    void BallObj::onDraw()
    {
        // Необовязковий метод
        // Якщо перевизначено, тут можна відрисувати все, що НЕ стосується спрайта об'єкта.
        // Наприклад, полоску XP над ним.

        IGameObject::onDraw();
    }

    void BallObj::move(MovingDirection direction)
    {
        // Дельта T не реалізовано
        if (direction == DIRECTION_NONE)
            return;

        if (direction == DIRECTION_UP)
            stepTo(_x_global, _y_global - PIX_PER_STEP);
        else if (direction == DIRECTION_DOWN)
            stepTo(_x_global, _y_global + PIX_PER_STEP);
        else if (direction == DIRECTION_LEFT)
            stepTo(_x_global - PIX_PER_STEP, _y_global);
        else if (direction == DIRECTION_RIGHT)
            stepTo(_x_global + PIX_PER_STEP, _y_global);
    }

    void BallObj::stepTo(uint16_t x, uint16_t y)
    {
        if (_terrain.canPass(_x_global, _y_global, x, y, _sprite))
        {
            _x_global = x; // Якщо перевірка успішна - рухаємо об'єкт
            _y_global = y;
        }
    }
}