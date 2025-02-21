#pragma once
// #pragma GCC optimize("O3")
#include <Arduino.h>
#include <list>
#include <unordered_map>
#include "WavTrack.h"

namespace meow
{
    class WavManager
    {
    public:
        ~WavManager();

        bool init(uint8_t I2S_BCLK_PIN, uint8_t I2S_LRC_PIN, uint8_t I2S_DOUT_PIN);

        /*!
         * @brief
         *      Додати звукову доріжку до списку міксування. Ресурси зі списку міксування звільняються автоматично.
         * @param  track
         *      Вказівник на передзавантажену доріжку.
         * @return
         *      Ідентифікатор на доріжку в списку міксування.
         */
        uint16_t addToMix(WavTrack *track);

        /*!
         * @brief
         *     Видалити доріжку із списку міксування.
         * @param  id
         *     Ідентифікатор, який було присвоєно доріжці, під час додавання до міксу.
         */
        void removeFromMix(uint16_t id);

        /*!
         * @brief
         *     Стартувати задачу відтворення міксу.
         */
        void startMix();

        /*!
         * @brief
         *    Поставити на паузу/відновити вітворення міксу.
         */
        void pauseResume();

    private:
        bool _is_inited{false};
        bool _is_playing{false};

        static uint64_t _track_id;
        static std::unordered_map<uint16_t, WavTrack *> _mix;

        struct TaskParams
        {
            enum CMD : uint8_t
            {
                CMD_NONE = 0,
                CMD_PAUSE,
                CMD_STOP
            };

            CMD cmd{CMD_NONE};
        } _params;

        TaskHandle_t _task_handle = NULL;

        static void mixTask(void *params);
    };

}