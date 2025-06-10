#pragma GCC optimize("O3")

#include "EmptyLayout.h"

namespace meow
{
    EmptyLayout::EmptyLayout(uint16_t widget_ID, IWidget::TypeID type_ID) : IWidgetContainer(widget_ID,
                                                                                             type_ID == TYPE_ID_UNKNOWN ? TYPE_ID_EMPTY_LAYOUT : type_ID)
    {
    }

    EmptyLayout::~EmptyLayout() {}

    void EmptyLayout::onDraw()
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

            for (size_t i{0}; i < _widgets.size(); ++i)
                _widgets[i]->forcedDraw();
        }

        xSemaphoreGive(_widg_mutex);
    }

    EmptyLayout *EmptyLayout::clone(uint16_t id) const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        try
        {
            EmptyLayout *cln = new EmptyLayout(id);
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

            for (const auto &widget_ptr : _widgets)  // cppcheck-suppress constVariableReference
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