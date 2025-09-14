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
            Keyboard *cln = new Keyboard(id);
            cln->_has_border = _has_border;
            cln->_x_pos = _x_pos;
            cln->_y_pos = _y_pos;
            cln->_width = _width;
            cln->_height = _height;
            cln->_back_color = _back_color;
            cln->_border_color = _border_color;
            cln->_corner_radius = _corner_radius;
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

    uint16_t Keyboard::getCurrBtnID() const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        uint16_t id = getFocusRow()->getCurrBtnID();
        xSemaphoreGive(_widg_mutex);
        return id;
    }

    String Keyboard::getCurrBtnTxt() const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        const KeyboardRow *row = getFocusRow();
        xSemaphoreGive(_widg_mutex);
        return row->getCurrBtnTxt();
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

        row = getFocusRow();
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
            ++_cur_focus_row_pos;
        else
            _cur_focus_row_pos = 0;

        row = getFocusRow();
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

    uint16_t Keyboard::getFocusXPos() const
    {
        return getFocusRow()->getCurFocusPos();
    }

    void Keyboard::setFocusPos(uint16_t x, uint16_t y)
    {
        if (_widgets.empty())
            return;

        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        _has_manual_settings = true;

        KeyboardRow *row = getFocusRow();
        row->removeFocus();

        if (y > _widgets.size() - 1)
            _cur_focus_row_pos = _widgets.size() - 1;
        else
            _cur_focus_row_pos = y;

        row = getFocusRow();
        row->setFocus(x);

        xSemaphoreGive(_widg_mutex);
    }

    KeyboardRow *Keyboard::getFocusRow() const
    {
        if (_widgets.empty())
        {
            log_e("Не додано жодної KeyboardRow");
            esp_restart();
        }

        KeyboardRow *row = _widgets[_cur_focus_row_pos]->castTo<KeyboardRow>();

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
                for (size_t i{0}; i < _widgets.size(); ++i)
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

            if (_first_drawing && !_has_manual_settings)
            {
                _first_drawing = false;

                if (!_widgets.empty())
                    getFocusRow()->setFocus(0);
            }

            uint16_t x{2};
            uint16_t y{2};

            for (size_t i{0}; i < _widgets.size(); ++i)
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