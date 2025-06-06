#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

namespace simple_rpg
{
    class PortalObj : public IGameObject
    {
    public:
        PortalObj(ResManager &res,
                  WavManager &audio,
                  TerrainManager &terrain,
                  std::unordered_map<uint32_t, IGameObject *> &game_objs) : IGameObject(res, audio, terrain, game_objs) {}
        virtual ~PortalObj() {}

        virtual void init() override;
        virtual void update() override;
        //
        virtual void serialize(DataStream &ds) override {}   // Портал не зберігає інформацію для перенесення між сценами
        virtual void deserialize(DataStream &ds) override {} // Тому реалізація чисто віртальних методів порожня
        virtual size_t getDataSize() const override { return 0; }

    private:
        std::vector<const uint16_t *> spin_anim;
    };
}