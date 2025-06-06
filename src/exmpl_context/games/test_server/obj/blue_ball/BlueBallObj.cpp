#include "BlueBallObj.h"
#include "../ClassID.h"

#include "res/sprite_blue_ball.h"

namespace test_server
{
    void BlueBallObj::init()
    {
        _type_ID = ClassID::BLUE_BALL;    // Встановити ідентифікатор типу
        _sprite.img_ptr = SPRITE_BLUE_BALL; // Встановити зображення спрайта

        BallObj::init();
    }
}