#include "SokobanScene.h"

#include "../obj/TriggerID.h"
#include "../SceneID.h"

// Підключити шаблон мапи рівня
#include "../map/template/map_scene_0_2.h"
#include "../map/template/map_scene_3.h"
#include "../map/template/map_scene_4.h"
#include "../map/template/map_scene_5.h"
#include "../map/template/map_scene_6.h"
#include "../map/template/map_scene_7.h"
#include "../map/template/map_scene_8.h"
#include "../map/template/map_scene_10.h"
#include "../map/template/map_scene_11.h"
#include "../map/template/map_scene_12.h"
#include "../map/template/map_scene_13.h"
#include "../map/template/map_scene_14.h"
#include "../map/template/map_scene_15.h"
#include "../map/template/map_scene_16.h"
#include "../map/template/map_scene_17.h"
#include "../map/template/map_scene_18.h"
#include "../map/template/map_scene_19.h"
#include "../map/template/map_scene_20.h"
#include "../map/template/map_scene_21.h"
#include "../map/template/map_scene_22.h"
#include "../map/template/map_scene_23.h"
#include "../map/template/map_scene_24.h"
#include "../map/template/map_scene_25.h"
#include "../map/template/map_scene_26.h"
#include "../map/template/map_scene_27.h"
#include "../map/template/map_scene_28.h"
#include "../map/template/map_scene_29.h"
#include "../map/template/map_scene_30.h"
#include "../map/template/map_scene_31.h"
#include "../map/template/map_scene_32.h"
#include "../map/template/map_scene_33.h"
#include "../map/template/map_scene_34.h"
#include "../map/template/map_scene_35.h"
#include "../map/template/map_scene_36.h"
#include "../map/template/map_scene_37.h"
#include "../map/template/map_scene_38.h"
#include "../map/template/map_scene_39.h"
#include "../map/template/map_scene_40.h"
#include "../map/template/map_scene_41.h"
#include "../map/template/map_scene_42.h"
#include "../map/template/map_scene_43.h"
#include "../map/template/map_scene_44.h"
#include "../map/template/map_scene_45.h"
#include "../map/template/map_scene_46.h"
#include "../map/template/map_scene_47.h"
#include "../map/template/map_scene_48.h"
#include "../map/template/map_scene_49.h"
#include "../map/template/map_scene_50.h"
#include "../map/template/map_scene_51.h"
#include "../map/template/map_scene_52.h"
#include "../map/template/map_scene_53.h"
#include "../map/template/map_scene_54.h"
#include "../map/template/map_scene_55.h"
#include "../map/template/map_scene_56.h"
#include "../map/template/map_scene_57.h"
#include "../map/template/map_scene_58.h"
#include "../map/template/map_scene_59.h"
#include "../map/template/map_scene_60.h"
#include "../map/template/map_scene_61.h"
#include "../map/template/map_scene_62.h"
#include "../map/template/map_scene_63.h"
#include "../map/template/map_scene_64.h"
#include "../map/template/map_scene_65.h"
#include "../map/template/map_scene_66.h"
#include "../map/template/map_scene_67.h"
#include "../map/template/map_scene_68.h"
#include "../map/template/map_scene_69.h"
#include "../map/template/map_scene_70.h"

// Ідентифікатори ресурсів
#include "../ResID.h"

// Додати зображення плиток
#include "../../common_res/tile_img/tile_0.h"
#include "../../common_res/tile_img/tile_1.h"
//
#include "../ui/SceneUI.h"
// Ігрові об'єкти
#include "../obj/box/BoxObj.h"
#include "../obj/box_point/BoxPointObj.h"
//
namespace sokoban
{
    const uint16_t(*LVL_TEMPLATE[]) = {
        MAP_SCENE_0_2,
        MAP_SCENE_0_2,
        MAP_SCENE_0_2,
        MAP_SCENE_3,
        MAP_SCENE_4,
        MAP_SCENE_5,
        MAP_SCENE_6,
        MAP_SCENE_7,
        MAP_SCENE_8,
        MAP_SCENE_10,
        MAP_SCENE_11,
        MAP_SCENE_12,
        MAP_SCENE_13,
        MAP_SCENE_14,
        MAP_SCENE_15,
        MAP_SCENE_16,
        MAP_SCENE_17,
        MAP_SCENE_18,
        MAP_SCENE_19,
        MAP_SCENE_20,
        MAP_SCENE_21,
        MAP_SCENE_22,
        MAP_SCENE_23,
        MAP_SCENE_24,
        MAP_SCENE_25,
        MAP_SCENE_26,
        MAP_SCENE_27,
        MAP_SCENE_28,
        MAP_SCENE_29,
        MAP_SCENE_30,
        MAP_SCENE_31,
        MAP_SCENE_32,
        MAP_SCENE_33,
        MAP_SCENE_34,
        MAP_SCENE_35,
        MAP_SCENE_36,
        MAP_SCENE_37,
        MAP_SCENE_38,
        MAP_SCENE_39,
        MAP_SCENE_40,
        MAP_SCENE_41,
        MAP_SCENE_42,
        MAP_SCENE_43,
        MAP_SCENE_44,
        MAP_SCENE_45,
        MAP_SCENE_46,
        MAP_SCENE_47,
        MAP_SCENE_48,
        MAP_SCENE_49,
        MAP_SCENE_50,
        MAP_SCENE_51,
        MAP_SCENE_52,
        MAP_SCENE_53,
        MAP_SCENE_54,
        MAP_SCENE_55,
        MAP_SCENE_56,
        MAP_SCENE_57,
        MAP_SCENE_58,
        MAP_SCENE_59,
        MAP_SCENE_60,
        MAP_SCENE_61,
        MAP_SCENE_62,
        MAP_SCENE_63,
        MAP_SCENE_64,
        MAP_SCENE_65,
        MAP_SCENE_66,
        MAP_SCENE_67,
        MAP_SCENE_68,
        MAP_SCENE_69,
        MAP_SCENE_70};

    const uint8_t LVL_SIZE[][2] = // Розмір мапи
        {
            {8, 10},  // 0
            {8, 10},  // 1
            {8, 10},  // 2
            {8, 10},  // 3
            {8, 10},  // 4
            {10, 10}, // 5
            {9, 10},  // 6
            {11, 10}, // 7
            {9, 10},  // 8
            {19, 14}, // 10
            {22, 11}, // 11
            {14, 10}, // 12
            {17, 10}, // 13
            {22, 13}, // 14
            {17, 13}, // 15
            {12, 11}, // 16
            {13, 12}, // 17
            {16, 17}, // 18
            {17, 18}, // 19
            {21, 20}, // 20
            {19, 15}, // 21
            {13, 16}, // 22
            {20, 13}, // 23
            {17, 13}, // 24
            {17, 17}, // 25
            {14, 15}, // 26
            {18, 16}, // 27
            {22, 13}, // 28
            {28, 20}, // 29
            {20, 20}, // 30
            {16, 14}, // 31
            {22, 20}, // 32
            {25, 14}, // 33
            {21, 19}, // 34
            {23, 17}, // 35
            {15, 15}, // 36
            {23, 13}, // 37
            {15, 17}, // 38
            {24, 11}, // 39
            {14, 20}, // 40
            {15, 12}, // 41
            {18, 16}, // 42
            {13, 15}, // 43
            {12, 15}, // 44
            {20, 16}, // 45
            {18, 19}, // 46
            {21, 15}, // 47
            {14, 15}, // 48
            {23, 18}, // 49
            {11, 11}, // 50
            {20, 15}, // 51
            {13, 18}, // 52
            {17, 16}, // 53
            {25, 19}, // 54
            {19, 11}, // 55
            {22, 17}, // 56
            {19, 15}, // 57
            {16, 15}, // 58
            {19, 16}, // 59
            {21, 16}, // 60
            {16, 14}, // 61
            {21, 14}, // 62
            {13, 19}, // 63
            {23, 20}, // 64
            {22, 15}, // 65
            {14, 16}, // 66
            {18, 11}, // 67
            {27, 20}, // 68
            {29, 20}, // 69
            {26, 16}  // 70
    };

    const uint16_t SOKOBAN_POS[][2] = // Позиція сокобана
        {
            {2 * 32, 6 * 32},   // 0
            {2 * 32, 6 * 32},   // 1
            {2 * 32, 6 * 32},   // 2
            {2 * 32, 2 * 32},   // 3
            {3 * 32, 2 * 32},   // 4
            {7 * 32, 7 * 32},   // 5
            {1 * 32, 1 * 32},   // 6
            {7 * 32, 7 * 32},   // 7
            {3 * 32, 6 * 32},   // 8
            {8 * 32, 10 * 32},  // 10
            {12 * 32, 8 * 32},  // 11
            {7 * 32, 4 * 32},   // 12
            {14 * 32, 1 * 32},  // 13
            {8 * 32, 10 * 32},  // 14
            {14 * 32, 7 * 32},  // 15
            {9 * 32, 1 * 32},   // 16
            {5 * 32, 2 * 32},   // 17
            {1 * 32, 6 * 32},   // 18
            {1 * 32, 10 * 32},  // 19
            {2 * 32, 5 * 32},   // 20
            {7 * 32, 3 * 32},   // 21
            {6 * 32, 13 * 32},  // 22
            {7 * 32, 4 * 32},   // 23
            {7 * 32, 4 * 32},   // 24
            {6 * 32, 6 * 32},   // 25
            {3 * 32, 5 * 32},   // 26
            {10 * 32, 2 * 32},  // 27
            {15 * 32, 2 * 32},  // 28
            {12 * 32, 1 * 32},  // 29
            {6 * 32, 4 * 32},   // 30
            {2 * 32, 10 * 32},  // 31
            {11 * 32, 4 * 32},  // 32
            {5 * 32, 7 * 32},   // 33
            {5 * 32, 15 * 32},  // 34
            {17 * 32, 9 * 32},  // 35
            {4 * 32, 4 * 32},   // 36
            {10 * 32, 11 * 32}, // 37
            {6 * 32, 1 * 32},   // 38
            {19 * 32, 9 * 32},  // 39
            {8 * 32, 6 * 32},   // 40
            {13 * 32, 9 * 32},  // 41
            {8 * 32, 2 * 32},   // 42
            {1 * 32, 4 * 32},   // 43
            {10 * 32, 10 * 32}, // 44
            {10 * 32, 1 * 32},  // 45
            {7 * 32, 8 * 32},   // 46
            {9 * 32, 13 * 32},  // 47
            {10 * 32, 3 * 32},  // 48
            {11 * 32, 5 * 32},  // 49
            {8 * 32, 1 * 32},   // 50
            {17 * 32, 8 * 32},  // 51
            {2 * 32, 1 * 32},   // 52
            {15 * 32, 3 * 32},  // 53
            {13 * 32, 7 * 32},  // 54
            {9 * 32, 7 * 32},   // 55
            {11 * 32, 14 * 32}, // 56
            {9 * 32, 3 * 32},   // 57
            {7 * 32, 11 * 32},  // 58
            {2 * 32, 7 * 32},   // 59
            {5 * 32, 9 * 32},   // 60
            {5 * 32, 9 * 32},   // 61
            {5 * 32, 12 * 32},  // 62
            {4 * 32, 7 * 32},   // 63
            {4 * 32, 11 * 32},  // 64
            {5 * 32, 8 * 32},   // 65
            {11 * 32, 7 * 32},  // 66
            {7 * 32, 5 * 32},   // 67
            {21 * 32, 14 * 32}, // 68
            {13 * 32, 13 * 32}, // 69
            {6 * 32, 8 * 32}    // 70
    };

    const uint8_t BOX_NUM[] = // Кількість ящиків на рівень
        {
            1,  // 0
            2,  // 1
            3,  // 2
            7,  // 3
            5,  // 4
            6,  // 5
            3,  // 6
            6,  // 7
            5,  // 8
            20, // 10
            6,  // 11
            10, // 12
            11, // 13
            20, // 14
            12, // 15
            10, // 16
            11, // 17
            18, // 18
            15, // 19
            34, // 20
            14, // 21
            16, //  22
            18, //  23
            15, //  24
            14, //  25
            14, //  26
            18, //  27
            13, //  28
            20, // 29
            27, // 30
            6,  // 31
            35, // 32
            18, // 33
            23, // 34
            21, // 35
            13, // 36
            20, // 37
            20, // 38
            16, // 39
            18, // 40
            13, // 41
            20, // 42
            15, // 43
            15, // 44
            17, // 45
            21, // 46
            20, // 47
            14, // 48
            25, // 49
            8,  // 50
            16, // 51
            24, // 52
            16, // 53
            16, // 54
            9,  // 55
            19, // 56
            9,  // 57
            11, // 58
            13, // 59
            24, // 60
            17, // 61
            13, // 62
            11, // 63
            22, // 64
            24, // 65
            14, // 66
            16, // 67
            19, // 68
            30, // 69
            24  // 70
    };

    const uint16_t (*LVL_BOX[])[2] =
        {
            BOX_POS_0,
            BOX_POS_1,
            BOX_POS_2,
            BOX_POS_3,
            BOX_POS_4,
            BOX_POS_5,
            BOX_POS_6,
            BOX_POS_7,
            BOX_POS_8,
            BOX_POS_10,
            BOX_POS_11,
            BOX_POS_12,
            BOX_POS_13,
            BOX_POS_14,
            BOX_POS_15,
            BOX_POS_16,
            BOX_POS_17,
            BOX_POS_18,
            BOX_POS_19,
            BOX_POS_20,
            BOX_POS_21,
            BOX_POS_22,
            BOX_POS_23,
            BOX_POS_24,
            BOX_POS_25,
            BOX_POS_26,
            BOX_POS_27,
            BOX_POS_28,
            BOX_POS_29,
            BOX_POS_30,
            BOX_POS_31,
            BOX_POS_32,
            BOX_POS_33,
            BOX_POS_34,
            BOX_POS_35,
            BOX_POS_36,
            BOX_POS_37,
            BOX_POS_38,
            BOX_POS_39,
            BOX_POS_40,
            BOX_POS_41,
            BOX_POS_42,
            BOX_POS_43,
            BOX_POS_44,
            BOX_POS_45,
            BOX_POS_46,
            BOX_POS_47,
            BOX_POS_48,
            BOX_POS_49,
            BOX_POS_50,
            BOX_POS_51,
            BOX_POS_52,
            BOX_POS_53,
            BOX_POS_54,
            BOX_POS_55,
            BOX_POS_56,
            BOX_POS_57,
            BOX_POS_58,
            BOX_POS_59,
            BOX_POS_60,
            BOX_POS_61,
            BOX_POS_62,
            BOX_POS_63,
            BOX_POS_64,
            BOX_POS_65,
            BOX_POS_66,
            BOX_POS_67,
            BOX_POS_68,
            BOX_POS_69,
            BOX_POS_70};

    const uint16_t (*LVL_POINT[])[2] =
        {
            POINT_POS_0,
            POINT_POS_1,
            POINT_POS_2,
            POINT_POS_3,
            POINT_POS_4,
            POINT_POS_5,
            POINT_POS_6,
            POINT_POS_7,
            POINT_POS_8,
            POINT_POS_10,
            POINT_POS_11,
            POINT_POS_12,
            POINT_POS_13,
            POINT_POS_14,
            POINT_POS_15,
            POINT_POS_16,
            POINT_POS_17,
            POINT_POS_18,
            POINT_POS_19,
            POINT_POS_20,
            POINT_POS_21,
            POINT_POS_22,
            POINT_POS_23,
            POINT_POS_24,
            POINT_POS_25,
            POINT_POS_26,
            POINT_POS_27,
            POINT_POS_28,
            POINT_POS_29,
            POINT_POS_30,
            POINT_POS_31,
            POINT_POS_32,
            POINT_POS_33,
            POINT_POS_34,
            POINT_POS_35,
            POINT_POS_36,
            POINT_POS_37,
            POINT_POS_38,
            POINT_POS_39,
            POINT_POS_40,
            POINT_POS_41,
            POINT_POS_42,
            POINT_POS_43,
            POINT_POS_44,
            POINT_POS_45,
            POINT_POS_46,
            POINT_POS_47,
            POINT_POS_48,
            POINT_POS_49,
            POINT_POS_50,
            POINT_POS_51,
            POINT_POS_52,
            POINT_POS_53,
            POINT_POS_54,
            POINT_POS_55,
            POINT_POS_56,
            POINT_POS_57,
            POINT_POS_58,
            POINT_POS_59,
            POINT_POS_60,
            POINT_POS_61,
            POINT_POS_62,
            POINT_POS_63,
            POINT_POS_64,
            POINT_POS_65,
            POINT_POS_66,
            POINT_POS_67,
            POINT_POS_68,
            POINT_POS_69,
            POINT_POS_70};

    SokobanScene::SokobanScene(DataStream &stored_objs, bool is_loaded, uint8_t lvl)
        : IGameScene(stored_objs)
    {
        _level = --lvl;

        buildTerrain();

        createGhost();

        createSokoban();

        createBoxes();

        createBoxPoints();

        loadFX();

        // _audio.init();     // Ініціалізувати драйвер аудіо
        // _audio.startMix(); // Запустити міксування доріжок

        _game_UI = new SceneUI(); // Встановити шар UI. Тут можуть відрисовуватися різноманітні інформаційні панелі
    }

    SokobanScene::~SokobanScene()
    {
        delete _ghost; // Цей об'єкт необхідно видаляти самостійно, тому що він не додається до ігрового світу
    }

    void SokobanScene::update()
    {
        IGameObject::MovingDirection direction = IGameObject::DIRECTION_NONE;

        if (_input.isPressed(BtnID::BTN_SAVE))
        {
            _input.lock(BtnID::BTN_SAVE, 300);
            openSceneByID(++_level);
            return;
        }
        else if (_input.isPressed(BtnID::BTN_EXIT))
        {
            _input.lock(BtnID::BTN_EXIT, 300);
            _is_finished = true;
            return;
        }
        else if (_input.isReleased(BtnID::BTN_C))
        {
            _input.lock(BtnID::BTN_C, 300);
            _is_ghost_selected = !_is_ghost_selected;
            _input.reset();
        }

        if (_is_ghost_selected)
        {
            _main_obj = _ghost; // Якщо обрано привида встановлюємо його вказівник головним у сцені

            if (_input.isHolded(BtnID::BTN_UP)) // Оброблюємо тільки утримання кнопок
            {
                _input.lock(BtnID::BTN_UP, 30);
                direction = IGameObject::DIRECTION_UP;
            }
            else if (_input.isHolded(BtnID::BTN_DOWN))
            {
                _input.lock(BtnID::BTN_DOWN, 30);
                direction = IGameObject::DIRECTION_DOWN;
            }
            else if (_input.isHolded(BtnID::BTN_RIGHT))
            {
                _input.lock(BtnID::BTN_RIGHT, 30);
                direction = IGameObject::DIRECTION_RIGHT;
            }
            else if (_input.isHolded(BtnID::BTN_LEFT))
            {
                _input.lock(BtnID::BTN_LEFT, 30);
                direction = IGameObject::DIRECTION_LEFT;
            }

            _ghost->move(direction);
        }
        else
        {
            _main_obj = _sokoban; // Встановлюємо головним об'єктом комірника

            if (_input.isReleased(BtnID::BTN_UP)) // Оброблюємо тільки відпусканя кнопок
            {
                _input.lock(BtnID::BTN_UP, 100); // Блокуємо кнопку, щоб запобігти випадковим зайвим натисканням
                direction = IGameObject::DIRECTION_UP;
            }
            else if (_input.isReleased(BtnID::BTN_DOWN))
            {
                _input.lock(BtnID::BTN_DOWN, 100);
                direction = IGameObject::DIRECTION_DOWN;
            }
            else if (_input.isReleased(BtnID::BTN_RIGHT))
            {
                _input.lock(BtnID::BTN_RIGHT, 100);
                direction = IGameObject::DIRECTION_RIGHT;
            }
            else if (_input.isReleased(BtnID::BTN_LEFT))
            {
                _input.lock(BtnID::BTN_LEFT, 100);
                direction = IGameObject::DIRECTION_LEFT;
            }

            _sokoban->move(direction);
        }

        IGameScene::update(); // Необхідно обов'язково перевикликати метод кожен кадр у базовму класі. Інакше сцена не буде перемальовуватися.
    }

    void SokobanScene::onTrigger(uint8_t id)
    {
        if (id == TriggerID::TRIGGER_NEXT_SCENE)
        {
            if (_level + 2 < 70)
                openSceneByID(_level += 2);
            else
                _is_finished = true;
        }
    }

    void SokobanScene::buildTerrain()
    {
        // Створити опис кожної плитки мапи.
        _terrain.addTileDesc(0, Tile::TYPE_GROUND, IMG_TILE_0);
        _terrain.addTileDesc(1, Tile::TYPE_WALL, IMG_TILE_1);

        uint8_t tiles_x_num{LVL_SIZE[_level][0]}; // Кількість плиток мапи по ширині
        uint8_t tiles_y_num{LVL_SIZE[_level][1]}; // Кількість плиток мапи по висоті
        uint8_t tile_width{32};                   // Ширина або висота плитки. Плитки повинні бути квадратними

        _terrain.build(tiles_x_num, tiles_y_num, tile_width, LVL_TEMPLATE[_level]); // Побудувати мапу на основі даних шаблону
    }

    void SokobanScene::createGhost()
    {
        _ghost = createObject<GhostObj>(); // Створити об'єкт привида
        _ghost->init();                    // Обов'язковий виклик для ініціалізації об'єкта
        _main_obj = _ghost;                // Встановити головним об'єктом сцени привида. Саме за цим головним об'єктом слідкує viewport
    }

    void SokobanScene::createSokoban()
    {
        _sokoban = createObject<SokobanObj>();
        _sokoban->init();
        _game_objs.emplace(_sokoban->getId(), _sokoban);
        _sokoban->_x_global = SOKOBAN_POS[_level][0];
        _sokoban->_y_global = SOKOBAN_POS[_level][1];
    }

    void SokobanScene::createBoxes()
    {
        for (uint8_t i{0}; i < BOX_NUM[_level]; ++i)
        {
            BoxObj *box = createObject<BoxObj>(); // Створити об'єкт ящика
            box->init();
            box->_x_global = LVL_BOX[_level][i][0];
            box->_y_global = LVL_BOX[_level][i][1];

            _sokoban->addBoxPtr(box);              // Додати вказівник на об'єкт до комірника.
            _game_objs.emplace(box->getId(), box); // Додати об'єкт до ігрового світу
        }
    }

    void SokobanScene::createBoxPoints()
    {
        for (uint8_t i{0}; i < BOX_NUM[_level]; ++i)
        {
            BoxPointObj *point = createObject<BoxPointObj>(); // Створити об'єкт точки
            point->init();
            point->_x_global = LVL_POINT[_level][i][0];
            point->_y_global = LVL_POINT[_level][i][1];

            _game_objs.emplace(point->getId(), point);
        }
    }

    void SokobanScene::loadFX()
    {
        // std::unordered_map<uint16_t, const char *> fx_descr;
        // // Підготувати список звукових файлів, які необхідно завантажити
        // fx_descr.insert(std::pair<uint16_t, const char *>(FX_ID_STEP_1, FX_PATH_STEP_1));
        // fx_descr.insert(std::pair<uint16_t, const char *>(FX_ID_STEP_2, FX_PATH_STEP_2));
        // fx_descr.insert(std::pair<uint16_t, const char *>(FX_ID_STEP_3, FX_PATH_STEP_3));
        // fx_descr.insert(std::pair<uint16_t, const char *>(FX_ID_STEP_4, FX_PATH_STEP_4));
        // fx_descr.insert(std::pair<uint16_t, const char *>(FX_ID_STEP_5, FX_PATH_STEP_5));

        // // Завантажити звукові дані до менеджера ресурсів
        // _res_manager.loadWavs(fx_descr);
    }
}