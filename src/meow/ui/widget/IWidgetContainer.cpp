#pragma GCC optimize("O3")

#include "IWidgetContainer.h"

namespace meow
{
    IWidgetContainer::IWidgetContainer(uint16_t widget_ID, GraphicsDriver &display) : IWidget(widget_ID, display)
    {
        _is_container = true;
        _widg_mutex = xSemaphoreCreateMutex();
    }

    IWidgetContainer::~IWidgetContainer()
    {
        deleteWidgets();
        vSemaphoreDelete(_widg_mutex);
    }

    void IWidgetContainer::addWidget(IWidget *widget_ptr)
    {
        if (!widget_ptr)
        {
            log_e("*IWidget не може бути NULL.");
            esp_restart();
        }

        uint16_t search_ID = widget_ptr->getID();

        if (search_ID == 0)
        {
            log_e("WidgetID повинен бути > 0.");
            esp_restart();
        }

        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        for (const auto &widget : _widgets)
            if (widget->getID() == search_ID)
            {
                log_e("WidgetID повинен бути унікальним.");
                esp_restart();
            }

        widget_ptr->setParent(this);
        _widgets.push_back(widget_ptr);
        _is_changed = true;

        xSemaphoreGive(_widg_mutex);
    }

    bool IWidgetContainer::deleteWidgetByID(uint16_t widget_ID)
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        for (auto i_b = _widgets.begin(), i_e = _widgets.end(); i_b < i_e; ++i_b)
        {
            if ((*i_b)->getID() == widget_ID)
            {
                delete *i_b;
                _widgets.erase(i_b);
                _is_changed = true;

                xSemaphoreGive(_widg_mutex);
                return true;
            }
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    IWidget *IWidgetContainer::findWidgetByID(uint16_t widget_ID) const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        for (const auto &widget_ptr : _widgets)
            if (widget_ptr->getID() == widget_ID)
            {
                xSemaphoreGive(_widg_mutex);
                return widget_ptr;
            }

        xSemaphoreGive(_widg_mutex);
        return nullptr;
    }

    IWidget *IWidgetContainer::getWidgetByIndx(uint16_t widget_indx) const
    {
        IWidget *result{nullptr};

        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        if (_widgets.size() > widget_indx)
            result = _widgets[widget_indx];
        xSemaphoreGive(_widg_mutex);

        return result;
    }

    IWidget *IWidgetContainer::getWidgetByCoords(uint16_t x, uint16_t y) const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        for (const auto &widget : _widgets)
        {
            if (widget->hasIntersectWithCoords(x, y))
            {
                if (widget->isContainer())
                {
                    IWidgetContainer *container = (IWidgetContainer *)widget;
                    xSemaphoreGive(_widg_mutex);
                    return container->getWidgetByCoords(x, y);
                }
                else
                {
                    xSemaphoreGive(_widg_mutex);
                    return widget;
                }
            }
        }

        xSemaphoreGive(_widg_mutex);
        return (IWidget *)this;
    }

    void IWidgetContainer::deleteWidgets()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        for (const auto &widget_ptr : _widgets)
            delete widget_ptr;

        _widgets.clear();

        _is_changed = true;

        xSemaphoreGive(_widg_mutex);
    }
}