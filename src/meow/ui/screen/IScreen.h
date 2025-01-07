#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include "meowui_setup/screen_id.h"

#include "../../driver/input/Input.h"
#include "../../driver/graphics/GraphicsDriver.h"
#include "../widget/IWidgetContainer.h"

namespace meow
{
#define D_WIDTH _display.width()
#define D_HEIGHT _display.height()

    class IScreen
    {
    public:
        IScreen(GraphicsDriver &display);
        virtual ~IScreen() = 0;
        IScreen(const IScreen &rhs) = delete;
        IScreen &operator=(const IScreen &rhs) = delete;

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
        ScreenID getNextScreenID() const { return _next_screen_ID; }

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
        bool _screen_enabled{true};

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
         */
        virtual void loop() = 0;

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
        void openScreenByID(ScreenID screen_ID);

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

        const uint8_t UI_UPDATE_DELAY = 25; // затримка між фреймами. 40FPS
        IWidgetContainer *_layout;
        //
        bool _is_released{false};
        ScreenID _next_screen_ID;
        //
    };

}