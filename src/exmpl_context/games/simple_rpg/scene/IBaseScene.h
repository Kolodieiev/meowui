#pragma once
#include <Arduino.h>
#include "meow/game/IGameScene.h"
#include "../SceneID.h"              // Заголовковий файл, що містить ідентифіктаори сцен
#include "../obj/hero/HeroObj.h"     // Клас персонажа
#include "../obj/portal/PortalObj.h" // Клас порталу
#include "../ui/BaseUI.h"            // Клас кастомного UI

using namespace meow;

namespace simple_rpg
{
    class IBaseScene : public IGameScene
    {
    public:
        IBaseScene(DataStream &stored_objs);
        virtual ~IBaseScene() override;
        virtual void update() override;

    protected:
        // Об'єкти за наступними вказівниками обов'язкові для кожного рівня
        // Тому їх додано в базовий клас, щоб не дублювати код
        HeroObj *_hero{nullptr};
        PortalObj *_portal{nullptr};
        BaseUI *ui;

    private:
        // Метод для створення персонажа
        void createHeroObj();
        // Метод для створення порталу, що перемикає сцени
        void createPortalObj();
        void deserialize(DataStream &stored_objs);
        //
    };
}