#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

namespace simple_rpg
{
    class HeroObj : public IGameObject
    {
    public:
        HeroObj(ResManager &res,
                WavManager &audio,
                TerrainManager &terrain,
                std::unordered_map<uint32_t, IGameObject *> &game_objs) : IGameObject(res, audio, terrain, game_objs) {}
        virtual ~HeroObj() {}

        virtual void init() override;
        virtual void update() override;
        //
        virtual void serialize(DataStream &ds) override;
        virtual void deserialize(DataStream &ds) override;
        virtual size_t getDataSize() const override;
        //
        virtual void onDraw() override; // Необов'язковий метод

        void addBunny();
        uint16_t getBunnyNum() const { return _bunny_counter; }

        void moveU();
        void moveD();
        void moveL();
        void moveR();

        void stop();

    private:
        const uint16_t PIX_PER_STEP{5}; // Кількість пікселів на крок.

        // Вектори анімації руху
        std::vector<const uint16_t *> move_up_anim;
        std::vector<const uint16_t *> move_down_anim;
        std::vector<const uint16_t *> move_right_anim;
        std::vector<const uint16_t *> move_left_anim;
        //
        //
        uint16_t _bunny_counter{0}; // Поле, що містить кількість кроликів, створених персонажем
        //
        MovingDirection _prev_direction{DIRECTION_NONE};
        //
        void initAnimVectors();
        bool stepAt(uint16_t x, uint16_t y);

        void disableAnimation();
    };
}