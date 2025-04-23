#pragma GCC optimize("O3")

#include "Keyboard.h"

namespace meow
{
    Keyboard::Keyboard(uint16_t widget_ID) : IWidgetContainer(widget_ID, TYPE_ID_KEYBOARD) {}

    Keyboard *Keyboard::clone(uint16_t id) const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        try
        {
            Keyboard *clone = new Keyboard(id);
            clone->_has_border = _has_border;
            clone->_x_pos = _x_pos;
            clone->_y_pos = _y_pos;
            clone->_width = _width;
            clone->_height = _height;
            clone->_back_color = _back_color;
            clone->_border_color = _border_color;
            clone->_corner_radius = _corner_radius;
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

    uint16_t Keyboard::getCurrentBtnID() const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        uint16_t id = getFocusRow()->getCurrentBtnID();
        xSemaphoreGive(_widg_mutex);
        return id;
    }

    String Keyboard::getCurrentBtnTxt() const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        KeyboardRow *row = reinterpret_cast<KeyboardRow *>(getFocusRow());
        xSemaphoreGive(_widg_mutex);
        return row->getCurrentBtnTxt();
    }

    void Keyboard::focusUp()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        KeyboardRow *row = getFocusRow();

        uint16_t focusPos = row->getCurFocusPos();

        row->removeFocus();

        if (_cur_focus_row_pos > 0)
            _cur_focus_row_pos--;
        else
            _cur_focus_row_pos = _widgets.size() - 1;

        row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);
        row->setFocus(focusPos);

        xSemaphoreGive(_widg_mutex);
    }

    void Keyboard::focusDown()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        KeyboardRow *row = getFocusRow();

        uint16_t focusPos = row->getCurFocusPos();

        row->removeFocus();

        if (_cur_focus_row_pos < _widgets.size() - 1)
            _cur_focus_row_pos++;
        else
            _cur_focus_row_pos = 0;

        row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);
        row->setFocus(focusPos);

        xSemaphoreGive(_widg_mutex);
    }

    void Keyboard::focusLeft()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        KeyboardRow *row = getFocusRow();

        if (!row->focusUp())
        {
            row->removeFocus();
            row->setFocus(row->getSize() - 1);
        }

        xSemaphoreGive(_widg_mutex);
    }

    void Keyboard::focusRight()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        KeyboardRow *row = getFocusRow();

        if (!row->focusDown())
        {
            row->removeFocus();
            row->setFocus(0);
        }

        xSemaphoreGive(_widg_mutex);
    }

    KeyboardRow *Keyboard::getFocusRow() const
    {
        if (_widgets.empty())
        {
            log_e("Не додано жодної KeyboardRow");
            esp_restart();
        }

        KeyboardRow *row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);

        if (!row)
        {
            log_e("KeyboardRow не знайдено");
            esp_restart();
        }

        return row;
    }

    void Keyboard::onDraw()
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

            if (_first_drawing)
            {
                _first_drawing = false;

                if (!_widgets.empty())
                    getFocusRow()->setFocus(0);
            }

            uint16_t x{2};
            uint16_t y{2};

            for (uint16_t i{0}; i < _widgets.size(); ++i)
            {
                _widgets[i]->setPos(x, y);
                _widgets[i]->setWidth(_width - 4);
                _widgets[i]->onDraw();
                y += _widgets[i]->getHeight();
            }
        }

        xSemaphoreGive(_widg_mutex);
    }
}