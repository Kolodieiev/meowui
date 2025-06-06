#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"
#include "../box/BoxObj.h"

using namespace meow;

namespace sokoban
{
    class SokobanObj : public IGameObject
    {
    public:
        SokobanObj(ResManager &res,
                   WavManager &audio,
                   TerrainManager &terrain,
                   std::unordered_map<uint32_t, IGameObject *> &game_objs) : IGameObject(res, audio, terrain, game_objs) {}
        virtual ~SokobanObj() {}

        virtual void init() override;
        virtual void update() override;
        virtual void serialize(DataStream &ds) override;
        virtual void deserialize(DataStream &ds) override;
        virtual size_t getDataSize() const override;

        virtual void onDraw() override; // Необов'язковий метод

        void move(MovingDirection direction);

        // Додати вказівник на новий ящик на сцені. Комірник зможе перевіряти стан ящика по цьому вказівнику.
        void addBoxPtr(BoxObj *box_ptr);

    private:
        const uint16_t PIX_PER_STEP{32};

        std::vector<BoxObj *> _boxes;

        // Якщо можливо, переміститися самому в задані координати, та перемістити ящик
        void stepTo(uint16_t x, uint16_t y, uint16_t box_x_step, uint16_t box_y_step);
    };
}