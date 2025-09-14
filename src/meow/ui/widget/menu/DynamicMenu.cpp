#pragma GCC optimize("O3")

#include "DynamicMenu.h"

namespace meow
{
    DynamicMenu::DynamicMenu(uint16_t widget_ID) : Menu(widget_ID, TYPE_ID_DYN_MENU) {}

    bool DynamicMenu::focusUp()
    {
        if (_widgets.empty())
            return false;

        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (_cur_focus_pos > 0)
        {
            IWidget *item = _widgets[_cur_focus_pos];
            item->removeFocus();

            --_cur_focus_pos;

            uint16_t cycles_count = getCyclesCount();

            if (_cur_focus_pos < _first_item_index)
            {
                --_first_item_index;
                drawItems(_first_item_index, cycles_count);
            }

            item = _widgets[_cur_focus_pos];
            item->setFocus();

            xSemaphoreGive(_widg_mutex);
            return true;
        }
        else if (_prev_items_load_handler)
        {
            std::vector<MenuItem *> temp_vec;
            _prev_items_load_handler(temp_vec, getItemsNumOnScreen(), _widgets[_cur_focus_pos]->getID(), _prev_items_load_arg);

            if (!temp_vec.empty())
            {
                xSemaphoreGive(_widg_mutex);
                delWidgets();
                xSemaphoreTake(_widg_mutex, portMAX_DELAY);

                _widgets.reserve(temp_vec.size());
                for (size_t i{0}; i < temp_vec.size(); ++i)
                {
                    temp_vec[i]->setParent(this);
                    _widgets.push_back(temp_vec[i]);
                }

                drawItems(_first_item_index, getCyclesCount());
                _cur_focus_pos = _widgets.size() - 1;

                _widgets[_cur_focus_pos]->setFocus();

                xSemaphoreGive(_widg_mutex);
                return true;
            }
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    bool DynamicMenu::focusDown()
    {
        if (_widgets.empty())
            return false;

        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (_cur_focus_pos < _widgets.size() - 1)
        {
            IWidget *item = _widgets[_cur_focus_pos];
            item->removeFocus();

            ++_cur_focus_pos;

            uint16_t cycles_count = getCyclesCount();

            if (_cur_focus_pos > _first_item_index + cycles_count - 1)
            {
                ++_first_item_index;

                drawItems(_first_item_index, cycles_count);
            }

            item = _widgets[_cur_focus_pos];
            item->setFocus();

            xSemaphoreGive(_widg_mutex);
            return true;
        }
        else if (_next_items_load_handler)
        {
            std::vector<MenuItem *> temp_vec;
            _next_items_load_handler(temp_vec, getItemsNumOnScreen(), _widgets[_cur_focus_pos]->getID(), _next_items_load_arg);

            if (!temp_vec.empty())
            {
                xSemaphoreGive(_widg_mutex);
                delWidgets();
                xSemaphoreTake(_widg_mutex, portMAX_DELAY);

                _widgets.reserve(temp_vec.size());
                for (size_t i{0}; i < temp_vec.size(); ++i)
                {
                    temp_vec[i]->setParent(this);
                    _widgets.push_back(temp_vec[i]);
                }

                drawItems(_first_item_index, getCyclesCount());
                _cur_focus_pos = _first_item_index;

                _widgets[_cur_focus_pos]->setFocus();

                xSemaphoreGive(_widg_mutex);
                return true;
            }
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    DynamicMenu *DynamicMenu::clone(uint16_t id) const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        try
        {
            DynamicMenu *cln = new DynamicMenu(id);
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