#include "GreenBallObj.h"
#include "../ClassID.h"

#include "res/sprite_green_ball.h"

namespace test_server
{
    void GreenBallObj::init()
    {
        _type_ID = ClassID::GREEN_BALL;     // Встановити ідентифікатор типу
        _sprite.img_ptr = SPRITE_GREEN_BALL; // Встановити зображення спрайта

        BallObj::init();
    }
}