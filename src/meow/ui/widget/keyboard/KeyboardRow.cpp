#pragma GCC optimize("O3")

#include "KeyboardRow.h"
#include "../text/Label.h"

namespace meow
{
    KeyboardRow::KeyboardRow(uint16_t widget_ID, GraphicsDriver &display) : IWidgetContainer(widget_ID, display, TYPE_ID_KB_ROW) {}

    KeyboardRow *KeyboardRow::clone(uint16_t id) const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        try
        {
            KeyboardRow *clone = new KeyboardRow(id, _display);
            clone->_has_border = _has_border;
            clone->_x_pos = _x_pos;
            clone->_y_pos = _y_pos;
            clone->_width = _width;
            clone->_height = _height;
            clone->_back_color = _back_color;
            clone->_border_color = _border_color;
            clone->_corner_radius = _corner_radius;
            clone->_btn_height = _btn_height;
            clone->_btn_width = _btn_width;
            clone->_is_transparent = _is_transparent;
            clone->_visibility = _visibility;
            clone->_has_focus = _has_focus;
            clone->_old_border_state = _old_border_state;
            clone->_need_clear_border = _need_clear_border;
            clone->_need_change_border = _need_change_border;
            clone->_need_change_back = _need_change_back;
            clone->_focus_border_color = _focus_border_color;
            clone->_old_border_color = _old_border_color;
            clone->_focus_back_color = _focus_back_color;
            clone->_old_back_color = _old_back_color;
            clone->_parent = _parent;

            for (const auto &widget_ptr : _widgets)
            {
                IWidget *item = widget_ptr->clone(widget_ptr->getID());
                clone->addWidget(item);
            }

            xSemaphoreGive(_widg_mutex);
            return clone;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    uint16_t KeyboardRow::getCurrentBtnID() const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        uint16_t id = getFocusBtn()->getID();
        xSemaphoreGive(_widg_mutex);
        return id;
    }

    String KeyboardRow::getCurrentBtnTxt() const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        Label *lbl = reinterpret_cast<Label *>(getFocusBtn());
        xSemaphoreGive(_widg_mutex);
        return lbl->getText();
    }

    bool KeyboardRow::focusUp()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (_cur_focus_pos > 0)
        {
            IWidget *btn = getFocusBtn();
            btn->removeFocus();
            --_cur_focus_pos;
            btn = getFocusBtn();
            btn->setFocus();

            xSemaphoreGive(_widg_mutex);
            return true;
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    bool KeyboardRow::focusDown()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (!_widgets.empty() && _cur_focus_pos < _widgets.size() - 1)
        {
            IWidget *btn = getFocusBtn();
            btn->removeFocus();
            ++_cur_focus_pos;
            btn = getFocusBtn();
            btn->setFocus();

            xSemaphoreGive(_widg_mutex);
            return true;
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    void KeyboardRow::setFocus(uint16_t pos)
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        IWidget *btn = getFocusBtn();
        btn->removeFocus();

        if (pos > _widgets.size() - 1)
            _cur_focus_pos = _widgets.size() - 1;
        else
            _cur_focus_pos = pos;

        btn = getFocusBtn();
        btn->setFocus();

        xSemaphoreGive(_widg_mutex);
    }

    void KeyboardRow::removeFocus()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        getFocusBtn()->removeFocus();
        _cur_focus_pos = 0;

        xSemaphoreGive(_widg_mutex);
    }

    IWidget *KeyboardRow::getFocusBtn() const
    {
        if (_widgets.empty())
        {
            log_e("Не додано жодної кнопки");
            esp_restart();
        }

        IWidget *item = _widgets[_cur_focus_pos];

        if (!item)
        {
            log_e("Кнопку не знайдено");
            esp_restart();
        }

        return item;
    }

    void KeyboardRow::onDraw()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (!_is_changed)
        {
            if (_visibility != INVISIBLE && _is_enabled)
                for (uint16_t i{0}; i < _widgets.size(); ++i)
                    _widgets[i]->onDraw();
        }
        else
        {
            _is_changed = false;

            if (_visibility == INVISIBLE)
            {
                if (!_is_transparent)
                    hide();
                xSemaphoreGive(_widg_mutex);
                return;
            }

            if (!_is_transparent)
                clear();

            if (_widgets.empty())
            {
                xSemaphoreGive(_widg_mutex);
                return;
            }

            uint16_t step{0};

            if (_btn_width * _widgets.size() < _width)
                step = (float)(_width - _btn_width * _widgets.size()) / (_widgets.size() + 1);

            uint16_t x = step;
            uint16_t y = (float)(_height - _btn_height) / 2;

            for (uint16_t i{0}; i < _widgets.size(); ++i)
            {
                _widgets[i]->setPos(x, y);
                _widgets[i]->setWidth(_btn_width);
                _widgets[i]->setHeight(_btn_height);
                _widgets[i]->onDraw();

                x += _btn_width + step;
            }
        }

        xSemaphoreGive(_widg_mutex);
    }
}