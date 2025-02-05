#include "IGameScene.h"
#pragma GCC optimize("O3")
#include <algorithm>

namespace meow
{
    IGameScene::IGameScene(GraphicsDriver &display, Input &input, DataStream &stored_objs) : _display{display},
                                                                                             _input{input},
                                                                                             _terrain{TerrainManager(display)},
                                                                                             _stored_objs{stored_objs}
    {
        _obj_mutex = xSemaphoreCreateMutex();

        if (!_obj_mutex)
        {
            log_e("Не вдалося створити _obj_mutex");
            esp_restart();
        }
    }

    IGameScene::~IGameScene()
    {
        for (auto it = _game_objs.begin(), last_it = _game_objs.end(); it != last_it; ++it)
            delete it->second;

        delete _game_UI;
        delete _game_menu;
    }

    void IGameScene::update()
    {
        if (_is_paused)
            return;

        if (!_main_obj)
            return;

        takeLock();

        _terrain.setCameraPos(_main_obj->_x_global, _main_obj->_y_global);
        _terrain.onDraw();

        std::list<IGameObject *> view_obj;
        IGameObject *obj;

        for (auto it = _game_objs.begin(), last_it = _game_objs.end(); it != last_it; ++it)
        {
            obj = it->second;

            if (!obj->isDestroyed())
            {
                obj->update();

                if (obj->isTriggered())
                {
                    obj->resetTrigger();
                    onTrigger(obj->getTriggerID());
                }

                if (_terrain.isInView(obj->_x_global, obj->_y_global, obj->_sprite.width, obj->_sprite.height))
                {
                    if (obj != _main_obj)
                    {
                        obj->_x_local = obj->_x_global - _terrain.getViewX();
                        obj->_y_local = obj->_y_global - _terrain.getViewY();
                    }
                    else
                    {
                        if (_main_obj->_x_global < _terrain.HALF_VIEW_W)
                            _main_obj->_x_local = _main_obj->_x_global;
                        else if (_main_obj->_x_global < _terrain.getWidth() - _terrain.HALF_VIEW_W)
                            _main_obj->_x_local = _terrain.HALF_VIEW_W;
                        else
                            _main_obj->_x_local = _terrain.VIEW_W + _main_obj->_x_global - _terrain.getWidth();

                        if (_main_obj->_y_global < _terrain.HALF_VIEW_H)
                            _main_obj->_y_local = _main_obj->_y_global;
                        else if (_main_obj->_y_global < _terrain.getHeight() - _terrain.HALF_VIEW_H)
                            _main_obj->_y_local = _terrain.HALF_VIEW_H;
                        else
                            _main_obj->_y_local = _terrain.VIEW_H + _main_obj->_y_global - _terrain.getHeight();
                    }
                    view_obj.push_back(obj);
                }
            }
            else
            {
                delete it->second;
                _game_objs.erase(it);
            }
        }

        view_obj.sort([](IGameObject *a, IGameObject *b)
                      { if(a->getLayer() < b->getLayer())
                        return true;
                    else
                        return a->_y_global + a->_sprite.height < b->_y_global + b->_sprite.height; });

        for (auto it = view_obj.begin(), last_it = view_obj.end(); it != last_it; ++it)
            (*it)->onDraw();

        giveLock();

        if (_game_UI)
            _game_UI->onDraw();
    }

    void IGameScene::openSceneByID(uint16_t scene_ID)
    {
        _input.reset();
        _next_scene_ID = scene_ID;
        _is_released = true;
    }

    size_t IGameScene::getObjsSize()
    {
        size_t sum{0};
        takeLock();
        for (auto it = _game_objs.begin(), last_it = _game_objs.end(); it != last_it; ++it)
            sum += it->second->getDataSize();
        giveLock();
        return sum;
    }

    void IGameScene::serialize(DataStream &ds)
    {
        takeLock();
        for (auto it = _game_objs.begin(), last_it = _game_objs.end(); it != last_it; ++it)
            it->second->serialize(ds);
        giveLock();
    }
}