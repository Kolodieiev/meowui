#pragma once
#include <Arduino.h>

namespace meow
{
    class Tile
    {
    public:
        // Типи поверхні плитки. Можете розширити власними типами, за потреби.
        enum TileType : uint8_t
        {
            TYPE_NONE = 0,
            TYPE_WALL = 1 << 0,
            TYPE_GROUND = 1 << 1,
            TYPE_WATER = 1 << 2,
            TYPE_AIR = 1 << 3,
            TYPE_FIRE = 1 << 4,
            TYPE_SNOW = 1 << 5,
            TYPE_MUD = 1 << 6
        };

        Tile(TileType type, const uint16_t *img_ptr) : _img_ptr{img_ptr}, _type{type} {}

        const uint16_t *_img_ptr;
        const TileType _type;
    };
}