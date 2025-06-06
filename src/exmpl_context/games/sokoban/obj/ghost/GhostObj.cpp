#include "GhostObj.h"

namespace sokoban
{
    void GhostObj::init()
    {
        _sprite.is_rigid = false; // Вимикаємо тверде тіло для нашого привида, щоб він міг проходити крізь стіни
    }

    void GhostObj::update()
    {
    }

    void GhostObj::serialize(DataStream &ds)
    {
    }

    void GhostObj::deserialize(DataStream &ds)
    {
    }

    size_t GhostObj::getDataSize() const
    {
        return 0;
    }

    void GhostObj::move(MovingDirection direction)
    {
        if (direction == DIRECTION_NONE)
            return;

        if (direction == DIRECTION_UP)
        {
            // Даний об'єкт завжди може проходити по будь-якій плитці мапи, так як він не має твердого тіла.
            // Результат буде false тільки у випадку, якщо об'єкт спробує вийти за межі ігрового рівня.
            if (_terrain.canPass(_x_global, _y_global, _x_global, _y_global - PIX_PER_STEP, _sprite))
                _y_global -= PIX_PER_STEP;
        }
        else if (direction == DIRECTION_DOWN)
        {
            if (_terrain.canPass(_x_global, _y_global, _x_global, _y_global + PIX_PER_STEP, _sprite))
                _y_global += PIX_PER_STEP;
        }
        else if (direction == DIRECTION_LEFT)
        {
            if (_terrain.canPass(_x_global, _y_global, _x_global - PIX_PER_STEP, _y_global, _sprite))
                _x_global -= PIX_PER_STEP;
        }
        else if (direction == DIRECTION_RIGHT)
        {
            if (_terrain.canPass(_x_global, _y_global, _x_global + PIX_PER_STEP, _y_global, _sprite))
                _x_global += PIX_PER_STEP;
        }
    }
}