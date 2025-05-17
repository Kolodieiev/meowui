#pragma GCC optimize("O3")

#include "FixedMenu.h"

namespace meow
{
    FixedMenu::FixedMenu(uint16_t widget_ID) : Menu(widget_ID, TYPE_ID_FIX_MENU) {}

    bool FixedMenu::focusUp()
    {
        if (_widgets.empty())
            return false;

        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        IWidget *item = _widgets[_cur_focus_pos];
        item->removeFocus();
        uint16_t cycles_count = getCyclesCount();

        bool need_redraw = false;

        if (_cur_focus_pos > 0)
        {
            --_cur_focus_pos;

            if (_cur_focus_pos < _first_item_index)
            {
                --_first_item_index;
                need_redraw = true;
            }
        }
        else if (_is_loop_enbl)
        {
            if (_widgets.size() > cycles_count)
            {
                need_redraw = true;
                _first_item_index = _widgets.size() - cycles_count;
            }
            else
                _first_item_index = 0;

            _cur_focus_pos = _widgets.size() - 1;
        }

        item = _widgets[_cur_focus_pos];
        item->setFocus();

        if (need_redraw)
            drawItems(_first_item_index, cycles_count);

        xSemaphoreGive(_widg_mutex);
        return true;
    }

    bool FixedMenu::focusDown()
    {
        if (_widgets.empty())
            return false;

        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        IWidget *item = _widgets[_cur_focus_pos];
        item->removeFocus();
        uint16_t cycles_count = getCyclesCount();

        bool need_redraw = false;

        if (_cur_focus_pos < _widgets.size() - 1)
        {
            ++_cur_focus_pos;

            if (_cur_focus_pos > _first_item_index + cycles_count - 1)
            {
                need_redraw = true;
                ++_first_item_index;
            }
        }
        else if (_is_loop_enbl)
        {
            _cur_focus_pos = 0;
            _first_item_index = 0;

            need_redraw = _widgets.size() > cycles_count;
        }

        item = _widgets[_cur_focus_pos];
        item->setFocus();

        if (need_redraw)
            drawItems(_first_item_index, cycles_count);

        xSemaphoreGive(_widg_mutex);
        return true;
    }

    void FixedMenu::setCurrFocusPos(uint16_t focus_pos)
    {
        if (_widgets.size() < 2 || _cur_focus_pos == focus_pos || focus_pos >= _widgets.size())
            return;

        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        IWidget *item = _widgets[_cur_focus_pos];
        item->removeFocus();

        _cur_focus_pos = focus_pos;
        uint16_t cycles_count = getCyclesCount();

        uint16_t to_end = _widgets.size() - 1 - _cur_focus_pos;

        if (to_end >= cycles_count)
        {
            _first_item_index = _cur_focus_pos;
        }
        else
        {
            if (_widgets.size() <= cycles_count)
                _first_item_index = 0;
            else
                _first_item_index = _widgets.size() - cycles_count;
        }

        item = _widgets[_cur_focus_pos];
        item->setFocus();

        drawItems(_first_item_index, cycles_count);
        xSemaphoreGive(_widg_mutex);
    }

    FixedMenu *FixedMenu::clone(uint16_t id) const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        try
        {
            FixedMenu *clone = new FixedMenu(id);
            clone->_has_border = _has_border;
            clone->_x_pos = _x_pos;
            clone->_y_pos = _y_pos;
            clone->_width = _width;
            clone->_height = _height;
            clone->_back_color = _back_color;
            clone->_border_color = _border_color;
            clone->_corner_radius = _corner_radius;
            clone->_item_height = _item_height;
            clone->_item_width = _item_width;
            clone->_items_spacing = _items_spacing;
            clone->_is_loop_enbl = _is_loop_enbl;
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
}