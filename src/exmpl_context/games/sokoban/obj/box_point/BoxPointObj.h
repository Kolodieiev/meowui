#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

namespace sokoban
{
    class BoxPointObj : public IGameObject
    {
    public:
        BoxPointObj(ResManager &res,
                    WavManager &audio,
                    TerrainManager &terrain,
                    std::unordered_map<uint32_t, IGameObject *> &game_objs) : IGameObject(res, audio, terrain, game_objs) {}
        virtual ~BoxPointObj() {}

        virtual void init() override;
        virtual void update() override;
        virtual void serialize(DataStream &ds) override;
        virtual void deserialize(DataStream &ds) override;
        virtual size_t getDataSize() const override;
    };
}