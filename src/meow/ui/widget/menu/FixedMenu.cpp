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
            FixedMenu *cln = new FixedMenu(id);
            cln->_has_border = _has_border;
            cln->_x_pos = _x_pos;
            cln->_y_pos = _y_pos;
            cln->_width = _width;
            cln->_height = _height;
            cln->_back_color = _back_color;
            cln->_border_color = _border_color;
            cln->_corner_radius = _corner_radius;
            cln->_item_height = _item_height;
            cln->_item_width = _item_width;
            cln->_items_spacing = _items_spacing;
            cln->_is_loop_enbl = _is_loop_enbl;
            cln->_is_transparent = _is_transparent;
            cln->_visibility = _visibility;
            cln->_has_focus = _has_focus;
            cln->_old_border_state = _old_border_state;
            cln->_need_clear_border = _need_clear_border;
            cln->_need_change_border = _need_change_border;
            cln->_need_change_back = _need_change_back;
            cln->_focus_border_color = _focus_border_color;
            cln->_old_border_color = _old_border_color;
            cln->_focus_back_color = _focus_back_color;
            cln->_old_back_color = _old_back_color;
            cln->_parent = _parent;

            // cppcheck-suppress constVariableReference
            for (const auto &widget_ptr : _widgets)
                cln->addWidget(widget_ptr->clone(widget_ptr->getID()));

            xSemaphoreGive(_widg_mutex);
            return cln;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }
}