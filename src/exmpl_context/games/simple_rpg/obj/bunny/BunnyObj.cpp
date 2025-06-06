#include "BunnyObj.h"
#include "../ClassID.h"

#include "./sprite/up/move_u_i.h"
#include "./sprite/up/move_u_l.h"
#include "./sprite/up/move_u_r.h"
//
#include "./sprite/down/move_d_i.h"
#include "./sprite/down/move_d_l.h"
#include "./sprite/down/move_d_r.h"
//
#include "./sprite/left/move_l_i.h"
#include "./sprite/left/move_l_l.h"
#include "./sprite/left/move_l_r.h"
//
#include "./sprite/right/move_r_i.h"
#include "./sprite/right/move_r_l.h"
#include "./sprite/right/move_r_r.h"

namespace simple_rpg
{
    void BunnyObj::initAnimVectors()
    {
        move_up_anim.push_back(BUNNY_MOVE_UP_L);
        move_up_anim.push_back(BUNNY_MOVE_UP_R);
        //
        move_down_anim.push_back(BUNNY_MOVE_D_L);
        move_down_anim.push_back(BUNNY_MOVE_D_R);
        //
        move_right_anim.push_back(BUNNY_MOVE_R_L);
        move_right_anim.push_back(BUNNY_MOVE_R_R);
        //
        move_left_anim.push_back(BUNNY_MOVE_L_L);
        move_left_anim.push_back(BUNNY_MOVE_L_R);
    }

    void BunnyObj::disableAnimation()
    {
        if (!_sprite.has_animation)
            return;

        _sprite.has_animation = false;
        _sprite.has_img = true;

        switch (_prev_direction)
        {
        case DIRECTION_UP:
            _sprite.img_ptr = BUNNY_IDLE_U;
            break;
        case DIRECTION_DOWN:
            _sprite.img_ptr = BUNNY_IDLE_D;
            break;
        case DIRECTION_LEFT:
            _sprite.img_ptr = BUNNY_IDLE_L;
            break;
        case DIRECTION_RIGHT:
            _sprite.img_ptr = BUNNY_IDLE_R;
            break;
        }
    }

    void BunnyObj::init()
    {
        _type_ID = CLASS_BUNNY;
        _sprite.img_ptr = BUNNY_IDLE_D;
        _sprite.has_img = true;
        _sprite.width = 14;
        _sprite.height = 16;
        _sprite.frames_between_anim = 20;

        _sprite.pass_abillity_mask |= Tile::TYPE_GROUND;

        _sprite.rigid_offsets.top = 10;

        initSprite();
        initAnimVectors();
    }

    void BunnyObj::update()
    {
        // При виявленні персонажа в радіусі дії огляду, кролик починає тікати в будь-якому випадковому напрямку, але не в сторону персонажа.

        // Шукаємо персонажа в зоні огляду кроля
        std::list<IGameObject *> hero_list = getObjByClassInRadius(CLASS_HERO, 50);

        if (hero_list.empty())
        {
            disableAnimation();
        }
        else
        {
            // Отримуємо вектор руху
            int32_t dx = hero_list.front()->_x_global - _x_global;
            int32_t dy = hero_list.front()->_y_global - _y_global;

            // Визначаємо базовий напрямок
            int32_t move_x = (dx > 0) ? -PIX_PER_STEP : PIX_PER_STEP;
            int32_t move_y = (dy > 0) ? -PIX_PER_STEP : PIX_PER_STEP;

            // Додаємо випадковість до напрямку
            move_x += random(-2, 3) * PIX_PER_STEP;
            move_y += random(-2, 3) * PIX_PER_STEP;

            // Отримуємо наступні кординати переміщення
            move_x = _x_global + move_x;
            move_y = _y_global + move_y;

            if (move_x < 0)
                move_x = 0;

            if (move_y < 0)
                move_y = 0;

            if (!_terrain.canPass(_x_global, _y_global, move_x, move_y, _sprite)) // Застосовуємо зміщення
            {
                disableAnimation();
            }
            else
            {
                // Встановлюємо анімацію переміщення в потрібному напрямку.
                if (move_y < _y_global) // UP
                {
                    if (_prev_direction != DIRECTION_UP)
                    {
                        _prev_direction = DIRECTION_UP;
                        _sprite.animation_vec = &move_up_anim;
                        _sprite.has_animation = true;
                        _sprite.has_img = false;
                    }
                }
                else if (move_y > _y_global) // DOWN
                {
                    if (_prev_direction != DIRECTION_DOWN)
                    {
                        _prev_direction = DIRECTION_DOWN;
                        _sprite.animation_vec = &move_down_anim;
                        _sprite.has_animation = true;
                        _sprite.has_img = false;
                    }
                }
                else if (move_x < _x_global) // LEFT
                {
                    if (_prev_direction != DIRECTION_LEFT)
                    {
                        _prev_direction = DIRECTION_LEFT;
                        _sprite.animation_vec = &move_left_anim;
                        _sprite.has_animation = true;
                        _sprite.has_img = false;
                    }
                }
                else if (move_x > _x_global) // RIGHT
                {
                    if (_prev_direction != DIRECTION_RIGHT)
                    {
                        _prev_direction = DIRECTION_LEFT;
                        _sprite.animation_vec = &move_right_anim;
                        _sprite.has_animation = true;
                        _sprite.has_img = false;
                    }
                }

                _x_global = move_x;
                _y_global = move_y;
            }
        }
    }
}