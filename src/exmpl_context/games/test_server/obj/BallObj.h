#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

namespace test_server
{
    class BallObj : public IGameObject
    {
    public:
        BallObj( ResManager &res,
            WavManager &audio,
            TerrainManager &terrain,
            std::unordered_map<uint32_t, IGameObject *> &game_objs) : IGameObject(res, audio, terrain, game_objs)
        {
        }

        virtual ~BallObj() {}

        virtual void init() override;
        virtual void update() override;
        virtual void serialize(DataStream &ds) override;
        virtual void deserialize(DataStream &ds) override;
        virtual size_t getDataSize() const override;
        virtual void onDraw() override; // Необов'язковий метод

        void move(MovingDirection direction);

        void setName(const char *name) { _name = name; }

    protected:
        const uint16_t PIX_PER_STEP{5};
        void stepTo(uint16_t x, uint16_t y);
    };

}