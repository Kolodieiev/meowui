#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "meowui_setup/context_id.h"

#include "../../driver/input/Input.h"
#include "../../driver/graphics/GraphicsDriver.h"
#include "../widget/IWidgetContainer.h"
#include "../widget/text/Label.h"

namespace meow
{
#define D_WIDTH _display.width()
#define D_HEIGHT _display.height()

    class IContext
    {
    public:
        IContext(GraphicsDriver &display);
        virtual ~IContext() = 0;
        IContext(const IContext &rhs) = delete;
        IContext &operator=(const IContext &rhs) = delete;

        /**
         * @brief Службовий метод, необхідний для функціонування програми. Не викликай його з робочого коду.
         *
         */
        void _tick();

        /**
         * @brief Повертає ідентифікатор контексту, який було встановлено методом openContextByID.
         *
         * @return ContextID - унікальний ідентифікатор дисплею.
         */
        ContextID getNextContextID() const { return _next_context_ID; }

        /**
         * @brief Повертає значення прапору, який вказує на те, чи повинен бути звільнений цей контекст.
         *
         * @return true - якщо контекст повинен бути звільнений.
         * @return false - якщо контекст повинен бути активним.
         */
        bool isReleased() const { return _is_released; }

    protected:
        /**
         * @brief Об'єкт, що надає інформацію про поточний стан зареєстрованих кнопок,
         * та довзоляє керувати окремо кожною кнопкою.
         *
         */
        static Input _input;

        /**
         * @brief Об'єкт, що відповідає за вивід зображення на дисплей.
         *
         */
        GraphicsDriver &_display;

        /**
         * @brief Прапор, який дозволяє повністю вимкнути відрисовку GUI.
         * true - віджети будуть відмальовуватися. false - перерисовка віджетів буде пропущена.
         *
         */
        bool _gui_enabled{true};

        /**
         * @brief Викликається кожен кадр після оновлення стану кнопок, та перед формуванням буферу зображення.
         * Повинен бути обов'язково реалізований в кожному контексті.
         *
         */
        virtual void update() = 0;

        /**
         * @brief Викликається з максимальною частотою, яка доступна для поточного контексту, без прив'язки до GUI.
         * Повинен бути обов'язково реалізований в кожному контексті.
         *
         * @return true - Якщо контекст контролює ввід та малювання.
         * @return false - Інакше.
         */
        virtual bool loop() = 0;

        /**
         * @brief Встановлює віджет, який буде слугувати макетом GUI для поточного контексту. Віджет буде автоматично видалений разом з контекстом.
         *
         * @param layout Вказівник на віджет макету.
         */
        void setLayout(IWidgetContainer *layout);

        /**
         * @brief Повертає вказівник на поточний віджет макету контексту.
         *
         * @return IWidgetContainer*
         */
        IWidgetContainer *getLayout() const { return _layout; }

        /**
         * @brief Встановлює стан поточного контексту в такий, що повинен бути звільнений.
         * Та встановлює ідентифікатор контексту, в який повинен виконатися перехід.
         *
         * @param screen_ID
         */
        void openContextByID(ContextID screen_ID);

        /**
         * @brief Виводить коротке повідомлення-підказку в межах поточного контексту.
         * Повідомлення буде автоматично видалене, після спливання вказаного часу або в разі припиннення існування контексту, в якому воно було створене.
         *
         * @param msg_txt Текст повідомлення.
         * @param duration Тривалість відображення повідомлення.
         */
        void showToast(const char *msg_txt, unsigned long duration = 500);

        /**
         * @brief Повертає х-координату, на якій віджет буде встановлено по центру відносно екрану.
         *
         * @param widget Вказівник на віджет.
         * @return uint16_t
         */
        uint16_t getCenterX(IWidget *widget) const { return widget ? (D_WIDTH - widget->getWidth()) / 2 : 0; }

        /**
         * @brief Повертає y-координату, на якій віджет буде встановлено по центру відносно екрану.
         *
         * @param widget Вказівник на віджет.
         * @return uint16_t
         */
        uint16_t getCenterY(IWidget *widget) const { return widget ? (D_HEIGHT - widget->getHeight()) / 2 : 0; }

    private:
        SemaphoreHandle_t _layout_mutex;

        const uint8_t UI_UPDATE_DELAY = 20; // затримка між фреймами
        IWidgetContainer *_layout;
        //
        bool _is_released{false};
        ContextID _next_context_ID;
        //
        bool _has_toast{false};
        Label *_toast_label{nullptr};
        unsigned long _toast_lifetime;
        unsigned long _toast_birthtime;
        void removeToast();
    };

}