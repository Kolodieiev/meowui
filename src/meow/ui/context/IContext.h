#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "meowui_setup/context_id.h"

#include "../../driver/input/Input.h"
#include "../../driver/graphics/GraphicsDriver.h"
#include "../widget/IWidgetContainer.h"
#include "../widget/text/Label.h"
#include "../widget/notification/Notification.h"

namespace meow
{
#define D_WIDTH _display.width()
#define D_HEIGHT _display.height()

    class IContext
    {
    public:
        IContext();
        virtual ~IContext() = 0;
        IContext(const IContext &rhs) = delete;
        IContext &operator=(const IContext &rhs) = delete;

        /**
         * @brief Метод, що викликається для контексту кожен доступний тік.
         * Виклик методу мусить виконувати об'єкт, що керує цим контекстом.
         */
        void tick();

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
         * Також встановлює ідентифікатор контексту, в який повинен виконатися перехід.
         *
         * @param screen_ID
         */
        void openContextByID(ContextID screen_ID);

        /**
         * @brief Встановлює стан поточного контексту в такий, що повинен бути звільнений.
         *
         */
        void release();

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

        /**
         * @brief Відображає віджет Notification для поточного макету.
         *
         * @param notification Вказівник на віджет.
         */
        void showNotification(Notification *notification);

        /**
         * @brief Прибирає віджет Notification з відображення у макеті.
         * Пам'ять, яку займає об'єкт віджета не буде звільнено.
         *
         */
        void hideNotification();

    private:
        SemaphoreHandle_t _layout_mutex;
        IWidgetContainer *_layout;
        //
        bool _is_released{false};
        ContextID _next_context_ID;
        //
        unsigned long _upd_time{0};
        //
        Label *_toast_label{nullptr};
        unsigned long _toast_lifetime;
        unsigned long _toast_birthtime;
        void removeToast();
        //
        Notification *_notification{nullptr};
    };

}