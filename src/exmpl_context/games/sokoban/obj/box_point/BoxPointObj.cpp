#include "BoxPointObj.h"
//
#include "../ClassID.h"
//
#include "./res/sprite_box_point.h"

namespace sokoban
{
    void BoxPointObj::init()
    {
        _type_ID = ClassID::CLASS_BOX_POINT;
        _sprite.img_ptr = SPRITE_BOX_POIN;
        _sprite.has_img = true;
        _sprite.width = 32;
        _sprite.height = 32;

        _sprite.is_rigid = false;

        initSprite();
    }

    void BoxPointObj::update()
    {
    }

    void BoxPointObj::serialize(DataStream &ds)
    {
    }

    void BoxPointObj::deserialize(DataStream &ds)
    {
    }

    size_t BoxPointObj::getDataSize() const
    {
        return 0;
    }
}