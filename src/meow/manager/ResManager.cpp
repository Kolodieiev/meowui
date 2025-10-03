#include "./ResManager.h"

#include "meow/util/audio/WavUtil.h"
#include "meow/util/img/BmpUtil.h"

namespace meow
{
    uint16_t ResManager::loadBmpRes(const char *path_to_bmp)
    {
        if (!path_to_bmp)
            return 0;

        ImgData data = BmpUtil::loadBmp(path_to_bmp);

        if (data.width == 0)
        {
            log_e("Помилка завантаження: %s", path_to_bmp);
            return 0;
        }

        _bmp_res.insert(std::pair<uint16_t, ImgData>(_img_res_id_i, data));
        return _img_res_id_i++;
    }

    uint16_t ResManager::loadWavRes(const char *path_to_wav)
    {
        if (!path_to_wav)
            return 0;

        WavUtil util;
        AudioData data = util.loadWav(path_to_wav);

        if (data.size == 0)
        {
            log_e("Помилка завантаження: %s", path_to_wav);
            return 0;
        }

        _wav_res.insert(std::pair<uint16_t, AudioData>(_audio_res_id_i, data));
        return _audio_res_id_i++;
    }

    ImgData ResManager::getBmpRes(uint16_t res_id)
    {
        auto it = _bmp_res.find(res_id);
        if (it == _bmp_res.end())
            return _empty_img;

        return it->second;
    }

    AudioData ResManager::getWavRes(uint16_t res_id)
    {
        auto it = _wav_res.find(res_id);
        if (it == _wav_res.end())
            return _empty_audio;

        return it->second;
    }

    void ResManager::deleteBmpRes(uint16_t res_id)
    {
        auto it = _bmp_res.find(res_id);

        if (it != _bmp_res.end())
        {
            free(it->second.data_ptr);
            _bmp_res.erase(it);
        }
    }

    void ResManager::deleteWavRes(uint16_t res_id)
    {
        auto it = _wav_res.find(res_id);

        if (it != _wav_res.end())
        {
            free(it->second.data_ptr);
            _wav_res.erase(it);
        }
    }

    void ResManager::clearBmpRes()
    {
        for (auto it = _bmp_res.begin(), last_it = _bmp_res.end(); it != last_it; ++it)
            free(it->second.data_ptr);

        _bmp_res.clear();
    }

    void ResManager::clearWavRes()
    {
        for (auto it = _wav_res.begin(), last_it = _wav_res.end(); it != last_it; ++it)
            free(it->second.data_ptr);

        _wav_res.clear();
    }

    ResManager _res;
}
