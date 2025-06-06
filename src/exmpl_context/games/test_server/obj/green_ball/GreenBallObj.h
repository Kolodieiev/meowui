#pragma once
#include <Arduino.h>
#include "../BallObj.h"

using namespace meow;

namespace test_server
{
    class GreenBallObj : public BallObj
    {
    public:
        GreenBallObj(ResManager &res,
                     WavManager &audio,
                     TerrainManager &terrain,
                     std::unordered_map<uint32_t, IGameObject *> &game_objs) : BallObj(res, audio, terrain, game_objs) {}
        virtual ~GreenBallObj() {}

        virtual void init() override;

    private:
    };
}