#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>

#include "meowui_setup/input_setup.h"

#include "Pin.h"
#include "ITouchscreen.h"
#include <unordered_map>

namespace meow
{
    class Input
    {
    public:
        Input();

        /**
         * @brief Оновлює стан вводу. Не потрібно викликати метод самостійно
         *
         */
        void _update();

        /**
         * @brief Скидає стан вводу. Метод викликається контекстом перед його першим оновленням, щоб уникнути захоплення стану вводу з попереднього контексту.
         * Ручний виклик цього майже ніколи не потрібний.
         *
         */
        void reset();

        /**
         * @brief Вмикає фізичний пін та ініціалізує його в тому режимі, який було передано в конструктор під час створення об'єкта віртуального піна з цим номером.
         * Якщо віртуальний пін з таким номером відсутній, буде викликано виключення std::out_of_range.
         *
         * @param key_id Номер віртуального піна.
         */
        void enablePin(KeyID key_id);

        /**
         * @brief Вимикає пін, переводить його в режим високоімпедансного входу, та скидає стани віртуального піна з цим номером.
         * Це може бути корисним, якщо пін не використовується взагалі в поточному контексті або не використовуєтсья тривалий час.
         * Вимкнення піна на тривалий період, може трохи скоротити споживання струму мікроконтролером.
         * Якщо віртуальний пін з таким номером відсутній, буде викликано виключення std::out_of_range.
         *
         * @param key_id Номер віртуального піна.
         */
        void disablePin(KeyID key_id);

        /**
         * @brief Перевіряє чи знаходиться зараз віртуальний пін з таким номер в активному стані.
         * Тобто чи натиснута кнопка або чи фіксується дотик на цьому піні.
         * Якщо віртуальний пін з таким номером відсутній, буде викликано виключення std::out_of_range.
         *
         *
         * @param key_id Номер віртуального піна.
         * @return true - Якщо пін утримується.
         * @return false - Інакше.
         */
        bool isHolded(KeyID key_id) const;

        /**
         * @brief Перевіряє чи утримується пін більше n мілісекунд, що задано в налаштуваннях вводу.
         * Якщо віртуальний пін з таким номером відсутній, буде викликано виключення std::out_of_range.
         *
         * @param key_id Номер віртуального піна.
         * @return true - Якщо пін утримується більше n мілісекунд.
         * @return false - Інакше.
         */
        bool isPressed(KeyID key_id) const;

        /**
         * @brief Перевіряє чи було пін раніше активовано натисканням та відпущено.
         * Якщо віртуальний пін з таким номером відсутній, буде викликано виключення std::out_of_range.
         *
         * @param key_id Номер віртуального піна.
         * @return true - Якщо пін раніше було активовано та відпущено.
         * @return false - Інакше.
         */
        bool isReleased(KeyID key_id) const;

        /**
         * @brief Блокує віртуальний пін, щоб запобігти випадковим спрацюванням через брязкіт контактів,
         * або щоб задати час очікування до наступного спрацюванням цього піна. Під час виклику скидає стан віртуального піна
         * та блокує його оновлення, доки не сплине час блокування.
         * Якщо віртуальний пін з таким номером відсутній, буде викликано виключення std::out_of_range.
         *
         * @param key_id Номер віртуального піна.
         * @param lock_duration Час в мілісекундах, на який потрібно заблокувати віртуальний пін.
         */
        void lock(KeyID key_id, unsigned long lock_duration);

        /**
         * @brief Службовий метод. Може бути використаний виключно для відлагодження. Виводить в консоль режим, в якому знаходиться фізичний пін мікроконтролера.
         *
         * @param key_id Номер піна мікроконтролера.
         */
        void _printPinMode(KeyID key_id);

#ifdef TOUCHSCREEN_SUPPORT
        bool isHolded() const { return _touchscreen->isHolded(); }
        bool isPressed() const { return _touchscreen->isPressed(); }
        bool isReleased() const { return _touchscreen->isReleased(); }
        bool isSwiped() const { return _touchscreen->isSwiped(); }
        void lock(unsigned long lock_duration) { _touchscreen->lock(lock_duration); }

        ITouchscreen::Swipe getSwipe() { return _touchscreen->getSwipe(); }
        uint16_t getX() const { return _touchscreen->getX(); }
        uint16_t getY() const { return _touchscreen->getY(); }
#endif

    private:
        std::unordered_map<KeyID, Pin *> _buttons = BUTTONS;

#ifdef TOUCHSCREEN_SUPPORT
        ITouchscreen *_touchscreen;
#endif
    };

    extern Input _input;
}