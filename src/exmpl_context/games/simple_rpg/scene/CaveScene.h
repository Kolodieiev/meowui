#pragma once
#include <Arduino.h>
#include "./IBaseScene.h"

using namespace meow;

namespace simple_rpg
{
    class CaveScene : public IBaseScene
    {
    public:
        explicit CaveScene(DataStream &stored_objs);
        virtual ~CaveScene();

    protected:
        virtual void onTrigger(uint8_t trigg_id) override;

    private:
        void init();
        void buildTerrain();
    };
}