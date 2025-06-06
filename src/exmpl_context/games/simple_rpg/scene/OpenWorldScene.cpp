#include "OpenWorldScene.h"
#include "../obj/TriggerID.h" // Заголовковий файл, що містить ідентифікатори тригерів

// Підключити зображення плиток
#include "../map/tiles/open_world/tile_0.h"
#include "../map/tiles/open_world/tile_1.h"
#include "../map/tiles/open_world/tile_2.h"
#include "../map/tiles/open_world/tile_3.h"
#include "../map/tiles/open_world/tile_4.h"
#include "../map/tiles/open_world/tile_5.h"
#include "../map/tiles/open_world/tile_6.h"
#include "../map/tiles/open_world/tile_7.h"
#include "../map/tiles/open_world/tile_8.h"
#include "../map/tiles/open_world/tile_9.h"
#include "../map/tiles/open_world/tile_10.h"
#include "../map/tiles/open_world/tile_11.h"
#include "../map/tiles/open_world/tile_11.h"
#include "../map/tiles/open_world/tile_12.h"
#include "../map/tiles/open_world/tile_13.h"
#include "../map/tiles/open_world/tile_14.h"
#include "../map/tiles/open_world/tile_15.h"
#include "../map/tiles/open_world/tile_16.h"
//
#include "../map/template/main_scene_tmpl.h" // Підключити шаблон ігрового рівня

using namespace open_world_tiles;

namespace simple_rpg
{
    OpenWorldScene::OpenWorldScene(DataStream &stored_objs)
        : IBaseScene(stored_objs)
    {
        init();
        buildTerrain();
    }

    OpenWorldScene::~OpenWorldScene()
    {
    }

    void OpenWorldScene::onTrigger(uint8_t trigg_id)
    {
        if (trigg_id == TRIGG_PORTAL_USED)
        {
            openSceneByID(ID_SCENE_CAVE);
        }
    }

    void OpenWorldScene::init()
    {
        // Створюємо первинні об'єкти притаманні тільки цій сцені, і які не створюються в базовому класі
        createBunnyObj();

        // Якщо потрібно, тут можна задати нові координати відновлених об'єктів, тощо.
        _hero->setPos(30, 0);
        _portal->setPos(85, 172);
    }

    void OpenWorldScene::buildTerrain()
    {
        // Створити опис кожної плитки мапи.
        _terrain.addTileDesc(0, Tile::TYPE_WATER, IMG_TILE_0);
        _terrain.addTileDesc(1, Tile::TYPE_WATER, IMG_TILE_1);
        _terrain.addTileDesc(2, Tile::TYPE_WATER, IMG_TILE_2);
        _terrain.addTileDesc(3, Tile::TYPE_WATER, IMG_TILE_3);
        _terrain.addTileDesc(4, Tile::TYPE_WATER, IMG_TILE_4);
        _terrain.addTileDesc(5, Tile::TYPE_WATER, IMG_TILE_5);
        _terrain.addTileDesc(6, Tile::TYPE_WATER, IMG_TILE_6);
        _terrain.addTileDesc(7, Tile::TYPE_WATER, IMG_TILE_7);
        _terrain.addTileDesc(8, Tile::TYPE_GROUND, IMG_TILE_8);
        _terrain.addTileDesc(9, Tile::TYPE_GROUND, IMG_TILE_9);
        _terrain.addTileDesc(10, Tile::TYPE_GROUND, IMG_TILE_10);
        _terrain.addTileDesc(11, Tile::TYPE_WATER, IMG_TILE_11);
        _terrain.addTileDesc(12, Tile::TYPE_WATER, IMG_TILE_12);
        _terrain.addTileDesc(13, Tile::TYPE_WATER, IMG_TILE_13);
        _terrain.addTileDesc(14, Tile::TYPE_WATER, IMG_TILE_14);
        _terrain.addTileDesc(15, Tile::TYPE_WATER, IMG_TILE_15);
        _terrain.addTileDesc(16, Tile::TYPE_WATER, IMG_TILE_16);

        // Зібрати ігровий рівень основі даних шаблону та опису плиток
        uint16_t w_num = 30;         // Кількість плиток в ширину
        uint16_t h_num = 20;         // Кількість плиток по висоті
        uint16_t tile_side_len = 16; // Розмір сторони плитки. Плитки повинні мати квадратну форму.
        _terrain.build(w_num, h_num, tile_side_len, MAIN_SCENE_TMPL);
    }

    void OpenWorldScene::createBunnyObj()
    {
    }
} // namespace simple_rpg
