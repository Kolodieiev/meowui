#include "CaveScene.h"
#include "../obj/TriggerID.h"
//
#include "../map/tiles/cave/tile_0.h"
#include "../map/tiles/cave/tile_1.h"
#include "../map/tiles/cave/tile_2.h"
#include "../map/tiles/cave/tile_3.h"
#include "../map/tiles/cave/tile_4.h"
#include "../map/tiles/cave/tile_5.h"
#include "../map/tiles/cave/tile_6.h"
#include "../map/tiles/cave/tile_7.h"
#include "../map/tiles/cave/tile_8.h"
//
#include "../map/template/cave_scene_tmpl.h"

using namespace cave_tiles;

namespace simple_rpg
{
    CaveScene::CaveScene(DataStream &stored_objs)
        : IBaseScene(stored_objs)
    {
        init();
        buildTerrain();
    }

    CaveScene::~CaveScene()
    {
    }

    void CaveScene::onTrigger(uint8_t trigg_id)
    {
        if (trigg_id == TRIGG_PORTAL_USED)
        {
            openSceneByID(ID_SCENE_OPEN_WORLD);
        }
    }

    void CaveScene::init()
    {
        _hero->setPos(30, 0);
        _portal->setPos(85, 64);
    }

    void CaveScene::buildTerrain()
    {
        _terrain.addTileDesc(0, Tile::TYPE_GROUND, IMG_TILE_0);
        _terrain.addTileDesc(1, Tile::TYPE_GROUND, IMG_TILE_1);
        _terrain.addTileDesc(2, Tile::TYPE_GROUND, IMG_TILE_2);
        _terrain.addTileDesc(3, Tile::TYPE_GROUND, IMG_TILE_3);
        _terrain.addTileDesc(4, Tile::TYPE_GROUND, IMG_TILE_4);
        _terrain.addTileDesc(5, Tile::TYPE_GROUND, IMG_TILE_5);
        _terrain.addTileDesc(6, Tile::TYPE_GROUND, IMG_TILE_6);
        _terrain.addTileDesc(7, Tile::TYPE_GROUND, IMG_TILE_7);
        _terrain.addTileDesc(8, Tile::TYPE_GROUND, IMG_TILE_8);

        uint16_t w_num = 10;
        uint16_t h_num = 10;
        uint16_t tile_side_len = 32;
        _terrain.build(w_num, h_num, tile_side_len, CAVE_SCENE_TMPL);
    }
}