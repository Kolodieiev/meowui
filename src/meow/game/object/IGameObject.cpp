#pragma GCC optimize("O3")
#include "IGameObject.h"
#include <cmath>

namespace meow
{
  uint32_t IGameObject::_global_obj_id_counter{0};

  IGameObject::IGameObject(GraphicsDriver &display,
                           ResManager &res,
                           WavManager &audio,
                           TerrainManager &terrain,
                           std::unordered_map<uint32_t, IGameObject *> &game_objs)
      : _display{display},
        _res_manager{res},
        _audio{audio},
        _obj_sprite{TFT_eSprite(display.getTFT())},
        _terrain{terrain},
        _game_objs{game_objs},
        _obj_id{++_global_obj_id_counter}
  {
    _obj_sprite.setSwapBytes(true);
    _obj_sprite.setColorDepth(16);
    _obj_sprite.setAttribute(PSRAM_ENABLE, true);
  }

#ifdef DOUBLE_BUFFERRING

  void IGameObject::initSprite()
  {
    if (!_sprite.has_img && !_sprite.has_animation)
      return;

    _obj_sprite.createSprite(_sprite.width, _sprite.height);

    if (!_obj_sprite.getPointer())
    {
      log_e("Недостатньо пам'яті для створення спрайту об'єкта");
      esp_restart();
    }
  }

  IGameObject::~IGameObject()
  {
    _obj_sprite.deleteSprite();
  }

  void IGameObject::onDraw()
  {
    if (_sprite.has_animation)
    {
      _obj_sprite.pushImage(0, 0, _sprite.width, _sprite.height, _sprite.animation_vec->at(_sprite.anim_pos));

      if (_sprite.angle == 0)
        _display.pushSprite(_obj_sprite, _x_local, _y_local, _sprite.transp_color);
      else
        _display.pushRotated(_obj_sprite, _x_local + _sprite.x_pivot, _y_local + _sprite.y_pivot, _sprite.angle, _sprite.transp_color);

      if (_sprite.anim_pos_counter != _sprite.frames_between_anim)
      {
        ++_sprite.anim_pos_counter;
      }
      else
      {
        _sprite.anim_pos_counter = 0;
        ++_sprite.anim_pos;
        if (_sprite.anim_pos == _sprite.animation_vec->size())
          _sprite.anim_pos = 0;
      }
    }
    else if (_sprite.has_img)
    {
      _obj_sprite.pushImage(0, 0, _sprite.width, _sprite.height, _sprite.img_ptr);

      if (_sprite.angle == 0)
        _display.pushSprite(_obj_sprite, _x_local, _y_local, _sprite.transp_color);
      else
        _display.pushRotated(_obj_sprite, _x_local + _sprite.x_pivot, _y_local + _sprite.y_pivot, _sprite.angle, _sprite.transp_color);
    }
  }

  uint16_t IGameObject::calcAngleToPoint(uint16_t x, uint16_t y)
  {
    int16_t azimut = atan2(y - _y_global - _sprite.y_pivot, x - _x_global - _sprite.x_pivot) * 180 / PI;
    if (azimut < 0)
      azimut += 360;
    return azimut + 90;
  }

  uint16_t IGameObject::calcDistToPoint(uint16_t x, uint16_t y)
  {
    uint16_t a = abs(_x_global + _sprite.x_pivot - x);
    uint16_t b = abs(_y_global + _sprite.y_pivot - y);

    return sqrt((a * a) + (b * b));
  }

  void IGameObject::stepToPoint(uint16_t x_to, uint16_t y_to, uint16_t step_w)
  {
    if (_x_global == x_to)
    {
      if (_y_global == y_to)
        return;
      else
      {
        if (_y_global > y_to)
        {
          _y_global -= step_w;
          if (_y_global < y_to)
            _y_global = y_to;
        }
        else
        {
          _y_global += step_w;
          if (_y_global > y_to)
            _y_global = y_to;
        }
      }
    }
    else if (_y_global == y_to)
    {
      if (_x_global > x_to)
      {
        _x_global -= step_w;
        if (_x_global < x_to)
          _x_global = x_to;
      }
      else
      {
        _x_global += step_w;
        if (_x_global > x_to)
          _x_global = x_to;
      }
    }
    else
    {
      uint16_t x_next{_x_global};

      if (x_next < x_to)
      {
        x_next += step_w;
        if (x_next > x_to)
        {
          _x_global = x_to;
          return;
        }
      }
      else
      {
        x_next -= step_w;
        if (x_next < x_to)
        {
          _x_global = x_to;
          return;
        }
      }

      _y_global = _y_global + (x_next - _x_global) * (float)(y_to - _y_global) / (x_to - _x_global);
      _x_global = x_next;
    }
  }

  std::list<IGameObject *> IGameObject::getObjectsAt(uint16_t x, uint16_t y, bool rigid_only)
  {
    std::list<IGameObject *> objs;

    for (auto it = _game_objs.begin(), last_it = _game_objs.end(); it != last_it; ++it)
    {
      if (it->second != this && it->second->hasIntersectWithPoint(x, y, rigid_only))
        objs.push_back(it->second);
    }

    return objs;
  }

  std::list<IGameObject *> IGameObject::getObjInCircle(uint16_t x_mid, uint16_t y_mid, uint16_t radius, bool rigid_only)
  {
    std::list<IGameObject *> objs;
    for (auto it = _game_objs.begin(), last_it = _game_objs.end(); it != last_it; ++it)
    {
      if (it->second != this && it->second->hasIntersectWithCircle(x_mid, y_mid, radius, rigid_only))
        objs.push_back(it->second);
    }

    return objs;
  }

  std::list<IGameObject *> IGameObject::getObjctsInRect(uint16_t x_start, uint16_t y_start, uint16_t width, uint16_t height, bool rigid_only)
  {
    std::list<IGameObject *> objs;
    for (auto it = _game_objs.begin(), last_it = _game_objs.end(); it != last_it; ++it)
    {
      if (it->second != this && it->second->hasIntersectWithRect(x_start, y_start, width, height, rigid_only))
        objs.push_back(it->second);
    }

    return objs;
  }

  bool IGameObject::hasIntersectWithPoint(uint16_t x, uint16_t y, bool rigid_only)
  {
    if (rigid_only)
    {
      if (!_sprite.is_rigid)
        return false;

      return (x >= _x_global + _sprite.rigid_offsets.left &&
              x <= _x_global + _sprite.width - _sprite.rigid_offsets.right - 1) &&
             (y >= _y_global + _sprite.rigid_offsets.top &&
              y <= _y_global + _sprite.height - _sprite.rigid_offsets.bottom - 1);
    }

    return (x >= _x_global &&
            x <= _x_global + _sprite.width - 1) &&
           (y >= _y_global &&
            y <= _y_global + _sprite.height - 1);
  }

  bool IGameObject::hasIntersectWithCircle(uint16_t x_mid, uint16_t y_mid, uint16_t radius, bool rigid_only)
  {
    float half_body_w;
    float half_body_h;

    float dist_x;
    float dist_y;

    if (rigid_only)
    {
      if (!_sprite.is_rigid)
        return false;

      half_body_w = (float)(_sprite.width - _sprite.rigid_offsets.left - _sprite.rigid_offsets.right) * 0.5f;
      half_body_h = (float)(_sprite.height - _sprite.rigid_offsets.top - _sprite.rigid_offsets.bottom) * 0.5f;

      dist_x = abs(x_mid - _x_global + _sprite.rigid_offsets.left - half_body_w);
      dist_y = abs(y_mid - _y_global + _sprite.rigid_offsets.top - half_body_h);
    }
    else
    {
      half_body_w = (float)_sprite.width * 0.5f;
      half_body_h = (float)_sprite.height * 0.5f;

      dist_x = abs(x_mid - _x_global - half_body_w);
      dist_y = abs(y_mid - _y_global - half_body_h);
    }

    if (dist_x > half_body_w + radius || dist_y > half_body_h + radius)
      return false;

    if (dist_x <= half_body_w || dist_y <= half_body_h)
      return true;

    float dx = dist_x - half_body_w;
    float dy = dist_y - half_body_h;

    return dx * dx + dy * dy <= radius * radius;
  }

  bool IGameObject::hasIntersectWithRect(uint16_t x_start, uint16_t y_start, uint16_t width, uint16_t height, bool rigid_only)
  {
    if (rigid_only)
    {
      if (!_sprite.is_rigid)
        return false;

      if (_x_global + _sprite.rigid_offsets.left > x_start + width ||
          x_start > _x_global + _sprite.width - _sprite.rigid_offsets.right - 1 ||
          _y_global + _sprite.rigid_offsets.top > y_start + height ||
          y_start > _y_global + _sprite.height - _sprite.rigid_offsets.bottom - 1)
        return false;
    }
    else
    {
      if (_x_global > x_start + width ||
          x_start > _x_global + _sprite.width - 1 ||
          _y_global > y_start + height ||
          y_start > _y_global + _sprite.height - 1)
        return false;
    }

    return true;
  }

  bool IGameObject::hasCollisionAt(uint16_t x_to, uint16_t y_to)
  {
    if (!_sprite.is_rigid)
      return false;

    x_to += _sprite.rigid_offsets.left;
    y_to += _sprite.rigid_offsets.top;
    uint16_t body_w = _sprite.width - _sprite.rigid_offsets.left - _sprite.rigid_offsets.right - 1;
    uint16_t body_h = _sprite.height - _sprite.rigid_offsets.top - _sprite.rigid_offsets.bottom - 1;

    IGameObject *obj;
    for (auto it = _game_objs.begin(), last_it = _game_objs.end(); it != last_it; ++it)
    {
      obj = it->second;

      if (obj != this && obj->hasIntersectWithRect(x_to, y_to, body_w, body_h, true))
        return true;
    }

    return false;
  }
#endif
}