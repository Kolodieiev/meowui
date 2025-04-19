#pragma GCC optimize("O3")

#include "IContext.h"
#include "../widget/layout/EmptyLayout.h"

namespace meow
{
    Input IContext::_input;

    void IContext::tick()
    {
        if (loop())
        {
            if ((millis() - _upd_time) < UI_UPDATE_DELAY)
            {
                vTaskDelay(2 / portTICK_PERIOD_MS);
            }
            else
            {
                _upd_time = millis();

                _input._update();

                update();

#ifdef GRAPHICS_ENABLED
                if (_gui_enabled)
                {
                    xSemaphoreTake(_layout_mutex, portMAX_DELAY);
                    _layout->onDraw();

                    if (_notification)
                    {
                        _notification->onDraw();
                    }

                    if (_toast_label)
                    {
                        if (millis() - _toast_birthtime > _toast_lifetime)
                            removeToast();
                        else
                            _toast_label->forcedDraw();
                    }
                    xSemaphoreGive(_layout_mutex);

#ifdef DOUBLE_BUFFERRING
                    _display._pushBuffer();
#endif // DOUBLE_BUFFERRING
                }
#endif // GRAPHICS_ENABLED
            }
        }
    }

    IContext::IContext(GraphicsDriver &display) : _display{display}
    {
        _layout_mutex = xSemaphoreCreateMutex();
        _layout = new EmptyLayout(1, _display);
        _layout->setBackColor(TFT_YELLOW);
        _layout->setWidth(_display.width());
        _layout->setHeight(_display.height());
    }

    IContext::~IContext()
    {
        delete _layout;
        _layout = nullptr;

        delete _toast_label;
        _toast_label = nullptr;
    }

    void IContext::setLayout(IWidgetContainer *layout)
    {
        if (!layout)
        {
            log_e("Спроба встановити NULL-layout.");
            esp_restart();
        }

        if (_layout == layout)
            return;

        xSemaphoreTake(_layout_mutex, portMAX_DELAY);

        delete _layout;
        _layout = layout;

        xSemaphoreGive(_layout_mutex);
    }

    void IContext::openContextByID(ContextID screen_ID)
    {
        _input.reset();
        _next_context_ID = screen_ID;
        _is_released = true;
    }

    void IContext::release()
    {
        openContextByID(static_cast<ContextID>(0));
    }

    void IContext::showToast(const char *msg_txt, unsigned long duration)
    {
        if (!msg_txt)
            return;

        if (duration < 500 || duration > 5000)
            duration = 500;

        _toast_birthtime = millis();
        _toast_lifetime = duration;

        if (_toast_label)
        {
            _toast_label->setText(msg_txt);
            _toast_label->setTicker(true);
            return;
        }

        _toast_label = new Label(1, _display);
        _toast_label->setText(msg_txt);
        _toast_label->setBackColor(TFT_WHITE);
        _toast_label->setTextColor(TFT_BLACK);
        _toast_label->setAlign(IWidget::ALIGN_CENTER);
        _toast_label->setGravity(IWidget::GRAVITY_CENTER);
        _toast_label->setTicker(true);
        _toast_label->setCornerRadius(10);
        _toast_label->setBorder(true);
        _toast_label->setBorderColor(TFT_ORANGE);
        _toast_label->setHeight(25);
        _toast_label->setTextOffset(4);

        if (D_WIDTH < 120)
            _toast_label->setWidth(D_WIDTH - 6);
        else
            _toast_label->setWidth(120);

        _toast_label->setPos(getCenterX(_toast_label), D_HEIGHT - _toast_label->getHeight() - 15);
    }

    void IContext::showNotification(Notification *notifaction)
    {
        _notification = notifaction;
    }

    void IContext::hideNotification()
    {
        _notification = nullptr;

        if (_layout)
            _layout->forcedDraw();
    }

    void IContext::removeToast()
    {
        delete _toast_label;
        _toast_label = nullptr;

        if (_layout)
            _layout->forcedDraw();
    }
}