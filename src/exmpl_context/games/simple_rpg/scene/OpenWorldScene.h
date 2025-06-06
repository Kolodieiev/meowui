#pragma once
#include <Arduino.h>
#include "./IBaseScene.h"

using namespace meow;

namespace simple_rpg
{
    class OpenWorldScene : public IBaseScene
    {
    public:
        OpenWorldScene(DataStream &stored_objs);
        virtual ~OpenWorldScene();

    protected:
        // Метод, який буде викликано ігровим рушієм у разі, якщо хоча б один із об'єктів на сцені підніме прапор спрацювання тригера.
        // В метод буде передано ідентифікатор тригера.
        virtual void onTrigger(uint8_t trigg_id) override;

    private:
        // Метод для початкової ініціалізації сцени
        void init();
        // Метод, в якому відбувається зв'язування ідентифікаторів плиток із зобрраженнями цих плиток та збирання ігрового рівня
        void buildTerrain();

        // Метод для створення кролика
        void createBunnyObj();
    };
}
