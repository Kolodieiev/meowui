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
        static void mixTask(void *params);

    private:
        std::unordered_map<uint16_t, WavTrack *> _mix;
        uint64_t _track_id{0};
        TaskHandle_t _task_handle{nullptr};

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

        bool _is_playing{false};
    };

}