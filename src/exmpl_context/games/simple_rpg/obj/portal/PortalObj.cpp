#include "PortalObj.h"
#include "../ClassID.h"
#include "../TriggerID.h"
//
#include "./sprite/frame_1.h"
#include "./sprite/frame_2.h"
#include "./sprite/frame_3.h"
#include "./sprite/frame_4.h"
#include "./sprite/frame_5.h"

namespace simple_rpg
{
    void PortalObj::init()
    {
        spin_anim.push_back(PORTAL_ANIM_FRAME_1);
        spin_anim.push_back(PORTAL_ANIM_FRAME_2);
        spin_anim.push_back(PORTAL_ANIM_FRAME_3);
        spin_anim.push_back(PORTAL_ANIM_FRAME_4);
        spin_anim.push_back(PORTAL_ANIM_FRAME_5);

        _type_ID = CLASS_PORTAL;
        _sprite.has_animation = true;       // Вказати, що об'єкт має анімацію
        _sprite.animation_vec = &spin_anim; // Задати вказівник на вектор анімації
        _sprite.transp_color = TFT_WHITE;   // Встановити колір, який буде ігноруватися під час малювання спрайту
        _sprite.frames_between_anim = 5;
        _sprite.width = 64;
        _sprite.height = 64;

        _trigger_ID = TRIGG_PORTAL_USED; // Встановлюємо ідентифікатор тригера цього об'єкта

        initSprite(); // Ініціалізувати спрайт об'єкта
    }

    void PortalObj::update()
    {
        // Об'єкт порталу кожен кадр шукає в списку об'єкт персонажа,
        // та перевіряє чи не знаходиться той у певній зоні перед порталом.
        // Якщо об'єкт персонажа виявлено перед порталом, портал піднімає прапор тригеру

        if (!getObjByClassInRect(CLASS_HERO, _x_global + 20, _y_global + _sprite.height, _sprite.width - 40, 1).empty())
        {
            _is_triggered = true;
        }
    }
}