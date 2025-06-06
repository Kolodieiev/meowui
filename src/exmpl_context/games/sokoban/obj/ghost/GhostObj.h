#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

namespace sokoban
{
    class GhostObj : public IGameObject
    {
    public:
        GhostObj(ResManager &res,
                 WavManager &audio,
                 TerrainManager &terrain,
                 std::unordered_map<uint32_t, IGameObject *> &game_objs) : IGameObject(res, audio, terrain, game_objs) {}
        virtual ~GhostObj() {}
        virtual void init() override;
        virtual void update() override;
        virtual void serialize(DataStream &ds) override;
        virtual void deserialize(DataStream &ds) override;
        virtual size_t getDataSize() const override;

        void move(MovingDirection direction);

    private:
        const uint16_t PIX_PER_STEP{20}; // Кількість пікселів пройдених за кадр
    };
}