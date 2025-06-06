#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

namespace simple_rpg
{
    class BunnyObj : public IGameObject
    {
    public:
        BunnyObj(ResManager &res,
                 WavManager &audio,
                 TerrainManager &terrain,
                 std::unordered_map<uint32_t, IGameObject *> &game_objs) : IGameObject(res, audio, terrain, game_objs) {}
        virtual ~BunnyObj() {}

        virtual void init() override;
        virtual void update() override;
        //
        virtual void serialize(DataStream &ds) override {}
        virtual void deserialize(DataStream &ds) override {}
        virtual size_t getDataSize() const override { return 0; }

    private:
        const uint16_t PIX_PER_STEP{5};

        std::vector<const uint16_t *> move_up_anim;
        std::vector<const uint16_t *> move_down_anim;
        std::vector<const uint16_t *> move_right_anim;
        std::vector<const uint16_t *> move_left_anim;

        MovingDirection _prev_direction{DIRECTION_NONE};

        void initAnimVectors();
        void disableAnimation();
    };
}