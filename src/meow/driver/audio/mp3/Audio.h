/*
 * Audio.h
 *
 *  Created on: Oct 28,2018
 *
 *  Version 3.0.10d
 *  Updated on: May 27.2024
 *      Author: Wolle (schreibfaul1)
 */

/*
 * Modified by Kolodieiev
 */

#pragma once
#pragma GCC optimize("O3")
#include <vector>
#include <Arduino.h>
#include "../../../manager/files/FileManager.h"
#include <libb64/cencode.h>

#if ESP_IDF_VERSION_MAJOR == 5
#include <driver/i2s_std.h>
#else
#include <driver/i2s.h>
#endif

#ifndef I2S_GPIO_UNUSED
#define I2S_GPIO_UNUSED -1 // = I2S_PIN_NO_CHANGE in IDF < 5
#endif
using namespace std;

class AudioBuffer
{

public:
    AudioBuffer(size_t maxBlockSize = 0);      // constructor
    ~AudioBuffer();                            // frees the buffer
    size_t init();                             // set default values
    bool isInitialized() { return m_f_init; }; //
    void setBufsize(int ram, int psram);       //
    int32_t getBufsize();                      //
    void changeMaxBlockSize(uint16_t mbs);     // is default 1600 for mp3 and aac, set 16384 for FLAC
    uint16_t getMaxBlockSize();                // returns maxBlockSize
    size_t freeSpace();                        // number of free bytes to overwrite
    size_t writeSpace();                       // space fom writepointer to bufferend
    size_t bufferFilled();                     // returns the number of filled bytes
    size_t getMaxAvailableBytes();             // max readable bytes in one block
    void bytesWritten(size_t bw);              // update writepointer
    void bytesWasRead(size_t br);              // update readpointer
    uint8_t *getWritePtr();                    // returns the current writepointer
    uint8_t *getReadPtr();                     // returns the current readpointer
    uint32_t getWritePos();                    // write position relative to the beginning
    uint32_t getReadPos();                     // read position relative to the beginning
    void resetBuffer();                        // restore defaults

protected:
    size_t m_buffSizePSRAM = UINT16_MAX * 10; // most webstreams limit the advance to 100...300Kbytes
    size_t m_buffSizeRAM = 1600 * 10;
    size_t m_buffSize = 0;
    size_t m_freeSpace = 0;
    size_t m_writeSpace = 0;
    size_t m_dataLength = 0;
    size_t m_resBuffSizeRAM = 2048;       // reserved buffspace, >= one wav  frame
    size_t m_resBuffSizePSRAM = 4096 * 4; // reserved buffspace, >= one flac frame
    size_t m_maxBlockSize = 1600;
    uint8_t *m_buffer = NULL;
    uint8_t *m_writePtr = NULL;
    uint8_t *m_readPtr = NULL;
    uint8_t *m_endPtr = NULL;
    bool m_f_start = true;
    bool m_f_init = false;
};
//----------------------------------------------------------------------------------------------------------------------

class Audio : private AudioBuffer
{

    AudioBuffer InBuff; // instance of input buffer

public:
    Audio(bool internalDAC = false, uint8_t channelEnabled = 3, uint8_t i2sPort = I2S_NUM_0); // #99
    ~Audio();
    void reconfigI2S();
    void setBufsize(int rambuf_sz, int psrambuf_sz);
    bool connecttoFS(meow::FileManager &f_mngr, const char *path, int32_t m_fileStartPos = -1);
    bool setFileLoop(bool input); // TEST loop
    bool setAudioPlayPosition(uint16_t sec);
    bool setFilePos(uint32_t pos);
    bool audioFileSeek(const float speed);
    bool setTimeOffset(int sec);
    bool setPinout(uint8_t BCLK, uint8_t LRC, uint8_t DOUT, int8_t MCLK = I2S_GPIO_UNUSED);
    bool pauseResume();
    bool isRunning() { return m_f_running; }
    void loop();
    uint32_t stopSong();
    void forceMono(bool m);
    void setBalance(int8_t bal = 0);
    void setVolumeSteps(uint8_t steps);
    void setVolume(uint8_t vol, uint8_t curve = 0);
    uint8_t getVolume();
    uint8_t maxVolume();

    uint32_t getAudioDataStartPos();
    uint32_t getFileSize();
    uint32_t getFilePos();
    uint32_t getSampleRate();
    uint8_t getBitsPerSample();
    uint8_t getChannels();
    uint32_t getBitRate(bool avg = false);
    uint32_t getAudioFileDuration();
    uint32_t getAudioCurrentTime();
    uint32_t getTotalPlayingTime();
    uint16_t getVUlevel();

    uint32_t inBufferFilled(); // returns the number of stored bytes in the inputbuffer
    uint32_t inBufferFree();   // returns the number of free bytes in the inputbuffer
    uint32_t inBufferSize();   // returns the size of the inputbuffer in bytes
    void setTone(int8_t gainLowPass, int8_t gainBandPass, int8_t gainHighPass);
    void setI2SCommFMT_LSB(bool commFMT);

private:
    size_t id3Size{0};
    size_t totalId3Size{0}; // if we have more header, id3_1_size + id3_2_size + ....
    size_t remainingHeaderBytes{0};
    size_t universal_tmp{0};
    uint8_t ID3version{0};
    int ehsz{0};
    char tag[5];
    char frameid[5];
    size_t framesize{0};
    bool compressed{false};
    uint8_t numID3Header{0};
    //
    uint16_t count{0};
    //
    uint32_t ctime{0};
    bool f_stream{false};
    bool f_fileDataComplete{false};
    uint32_t byteCounter{0}; // count received data
    //
    bool f_setDecodeParamsOnce{true};
    //
    uint8_t sampleArray[2][4][8] = {0};
    uint8_t cnt0{0}, cnt1{0}, cnt2{0}, cnt3{0}, cnt4{0};
    bool f_vu{false};
    //
    int16_t iir_out[2];
    int16_t iir_out1[2];
    int16_t iir_out2[2];
    //

#ifndef ESP_ARDUINO_VERSION_VAL
#define ESP_ARDUINO_VERSION_MAJOR 0
#define ESP_ARDUINO_VERSION_MINOR 0
#define ESP_ARDUINO_VERSION_PATCH 0
#endif

    void UTF8toASCII(char *str);
    void setDefaults(); // free buffers and set defaults
    void initInBuff();
    void processLocalFile();
    void playAudioData();
    int findNextSync(uint8_t *data, size_t len);
    int sendBytes(uint8_t *data, size_t len);
    void setDecoderItems();
    void computeAudioTime(uint16_t bytesDecoderIn);
    size_t readAudioHeader(uint32_t bytes);
    int read_ID3_Header(uint8_t *data, size_t len);
    bool setSampleRate(uint32_t hz);
    bool setBitsPerSample(int bits);
    bool setChannels(int channels);
    void setBitrate(int br);
    void playChunk(bool i2s_only);
    void computeVUlevel(int16_t sample[2]);
    void computeLimit();
    void Gain(int16_t *sample);
    bool initializeDecoder();
    esp_err_t I2Sstart(uint8_t i2s_num);
    esp_err_t I2Sstop(uint8_t i2s_num);
    int16_t *IIR_filterChain0(int16_t iir_in[2], bool clear = false);
    int16_t *IIR_filterChain1(int16_t iir_in[2], bool clear = false);
    int16_t *IIR_filterChain2(int16_t iir_in[2], bool clear = false);
    void IIR_calculateCoefficients(int8_t G1, int8_t G2, int8_t G3);
    bool readID3V1Tag();
    int32_t mp3_correctResumeFilePos(uint32_t resumeFilePos);

    //++++ implement several function with respect to the index of string ++++

    bool startsWith(const char *base, const char *str)
    {
        // fb
        char c;
        while ((c = *str++) != '\0')
            if (c != *base++)
                return false;
        return true;
    }

    int lastIndexOf(const char *haystack, const char *needle)
    {
        // fb
        int nlen = strlen(needle);
        if (nlen == 0)
            return -1;
        const char *p = haystack - nlen + strlen(haystack);
        while (p >= haystack)
        {
            int i = 0;
            while (needle[i] == p[i])
                if (++i == nlen)
                    return p - haystack;
            p--;
        }
        return -1;
    }

    int specialIndexOf(uint8_t *base, const char *str, int baselen, bool exact = false)
    {
        int result = 0; // seek for str in buffer or in header up to baselen, not nullterninated
        if (strlen(str) > baselen)
            return -1; // if exact == true seekstr in buffer must have "\0" at the end
        for (int i = 0; i < baselen - strlen(str); ++i)
        {
            result = i;
            for (int j = 0; j < strlen(str) + exact; ++j)
            {
                if (*(base + i + j) != *(str + j))
                {
                    result = -1;
                    break;
                }
            }
            if (result >= 0)
                break;
        }
        return result;
    }

    // some other functions
    size_t bigEndian(uint8_t *base, uint8_t numBytes, uint8_t shiftLeft = 8)
    {
        uint64_t result = 0;
        if (numBytes < 1 || numBytes > 8)
            return 0;
        for (int i = 0; i < numBytes; ++i)
        {
            result += *(base + i) << (numBytes - i - 1) * shiftLeft;
        }
        if (result > SIZE_MAX)
        {
            log_e("range overflow");
            result = 0;
        } // overflow
        return (size_t)result;
    }

    void vector_clear_and_shrink(vector<char *> &vec)
    {
        uint size = vec.size();
        for (int i = 0; i < size; ++i)
        {
            if (vec[i])
            {
                free(vec[i]);
                vec[i] = NULL;
            }
        }
        vec.clear();
        vec.shrink_to_fit();
    }

private:
    enum SampleIndex : uint8_t
    {
        LEFTCHANNEL = 0,
        RIGHTCHANNEL = 1
    };

    enum FilterType : uint8_t
    {
        LOWSHELF = 0,
        PEAKEQ = 1,
        HIFGSHELF = 2
    };

    typedef struct _filter
    {
        float a0;
        float a1;
        float a2;
        float b1;
        float b2;
    } filter_t;

    typedef struct _pis_array
    {
        int number;
        int pids[4];
    } pid_array;

    FILE *audiofile{nullptr};
    size_t _audio_size{0};
    meow::FileManager *_f_mgr;
    SemaphoreHandle_t mutex_audio;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#if ESP_IDF_VERSION_MAJOR == 5
    i2s_chan_handle_t m_i2s_tx_handle = {};
    i2s_chan_config_t m_i2s_chan_cfg = {}; // stores I2S channel values
    i2s_std_config_t m_i2s_std_cfg = {};   // stores I2S driver values
#else
    i2s_config_t m_i2s_config = {};
    i2s_pin_config_t m_pin_config = {};
#endif
#pragma GCC diagnostic pop

    std::vector<uint32_t> m_hashQueue;

    const size_t m_frameSizeMP3 = 1600;
    const size_t m_outbuffSize = 4096 * 2;

    uint16_t m_ibuffSize = 0; // will set in constructor (depending on PSRAM)
    filter_t m_filter[3];     // digital filters
    uint32_t m_sampleRate = 16000;
    uint32_t m_bitRate = 0;       // current bitrate given fom decoder
    uint32_t m_avr_bitrate = 0;   // average bitrate, median computed by VBR
    int m_controlCounter = 0;     // Status within readID3data() and readWaveHeader()
    int8_t m_balance = 0;         // -16 (mute left) ... +16 (mute right)
    uint16_t m_vol = 21;          // volume
    uint8_t m_vol_steps = 21;     // default
    double m_limit_left = 0;      // limiter 0 ... 1, left channel
    double m_limit_right = 0;     // limiter 0 ... 1, right channel
    uint8_t m_curve = 0;          // volume characteristic
    uint8_t m_bitsPerSample = 16; // bitsPerSample
    uint8_t m_channels = 2;
    uint8_t m_i2s_num = I2S_NUM_0; // I2S_NUM_0 or I2S_NUM_1
    uint8_t m_filterType[2];       // lowpass, highpass
    uint8_t m_vuLeft = 0;          // average value of samples, left channel
    uint8_t m_vuRight = 0;         // average value of samples, right channel
    int16_t *m_outBuff = NULL;     // Interleaved L/R
    int16_t m_validSamples = {0};  // #144
    int16_t m_curSample{0};
    int16_t m_decodeError = 0;       // Stores the return value of the decoder
    uint32_t m_contentlength = 0;    // Stores the length if the stream comes from fileserver
    uint32_t m_PlayingStartTime = 0; // Stores the milliseconds after the start of the audio
    int32_t m_resumeFilePos = -1;    // the return value from stopSong(), (-1) is idle
    int32_t m_fileStartPos = -1;     // may be set in connecttoFS()
    uint32_t m_haveNewFilePos = 0;   // user changed the file position
    bool m_f_exthdr = false;         // ID3 extended header
    bool m_f_running = false;
    bool m_f_firstCall = false;        // InitSequence for processWebstream and processLokalFile
    bool m_f_firstCurTimeCall = false; // InitSequence for computeAudioTime
    bool m_f_playing = false;          // valid mp3 stream recognized
    bool m_f_loop = false;             // Set if audio file should loop
    bool m_f_forceMono = false;        // if true stereo -> mono
    bool m_f_internalDAC = false;      // false: output vis I2S, true output via internal DAC
    uint8_t m_f_channelEnabled = 3;    // internal DAC, both channels
    uint32_t m_audioFileDuration = 0;
    float m_audioCurrentTime = 0;
    uint32_t m_audioDataStart = 0;  // in bytes
    size_t m_audioDataSize = 0;     //
    float m_filterBuff[3][2][2][2]; // IIR filters memory for Audio DSP
    float m_corr = 1.0;             // correction factor for level adjustment
    size_t m_i2s_bytesWritten = 0;  // set in i2s_write() but not used
    size_t m_fileSize = 0;          // size of the file
    uint16_t m_filterFrequency[2];
    int8_t m_gain0 = 0; // cut or boost filters (EQ)
    int8_t m_gain1 = 0;
    int8_t m_gain2 = 0;

    //

    uint64_t _sumBytesIn = 0;
    uint64_t _sumBytesOut = 0;
    uint64_t _sumBitRate = 0;
    uint32_t _frame_counter = 0;
    uint32_t _timeStamp = 0;
    uint32_t _deltaBytesIn = 0;
    bool _need_calc_br = true;
    bool _need_calc_offset = false;
};

//----------------------------------------------------------------------------------------------------------------------
