#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "../IWidget.h"
#include <SD.h>

namespace meow
{
    class Image final : public IWidget
    {
        // TODO додати анімацію
    public:
        Image(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~Image();

        /**
         * @brief Викликає процедуру малювання віджета на дисплей.
         * Якщо віджет не було змінено, він автоматично пропустить перемальовування.
         *
         */
        virtual void onDraw() override;

        /**
         * @brief Повертає вказівник на глибоку копію віджета.
         *
         * @param id Ідентифікатор, який буде присвоєно новому віджету.
         * @return Image*
         */
        virtual Image *clone(uint16_t id) const override;

        /**
         * @brief Повертає ідентифікатор типу.
         * Використовується в системі приведення типу.
         *
         * @return constexpr TypeID
         */
        static constexpr TypeID staticType() { return TypeID::TYPE_ID_IMAGE; }

        /**
         * @brief Встановлює колір, з яким пікселі не будуть відображатися.
         *
         * @param color
         */
        void setTransparentColor(uint16_t color)
        {
            _has_transp_color = true;
            _transparent_color = color;
            _is_changed = true;
        }

        /**
         * @brief Видаляє прозорість у зображення.
         *
         */
        void clearTransparency()
        {
            _has_transp_color = false;
            _is_changed = true;
        }

        /**
         * @brief Встановлює вказівник на буффер із зображенням.
         * Буфер не буде видалено разом із віджетом.
         *
         * @param image_ptr Вказівник на буфер із зображенням.
         */
        void setSrc(const uint16_t *image_ptr);

#ifdef DOUBLE_BUFFERRING
        /**
         * @brief Ініціалізує буфер спрайту зображення.
         *
         * @param width Ширина спрайту.
         * @param height Висота спрайту.
         */
        void init(uint16_t width, uint16_t height);
#endif

    private:
        bool _has_transp_color{false};
        uint16_t _transparent_color{TFT_TRANSPARENT};

        const uint16_t *_img_ptr{nullptr};

#ifdef DOUBLE_BUFFERRING
        TFT_eSprite _img_buf = TFT_eSprite(_display.getTFT());
#endif
    };

}