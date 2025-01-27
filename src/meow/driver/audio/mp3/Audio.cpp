/*
 * Audio.cpp
 *
 *  Created on: Oct 26.2018
 *
 *  Version 3.0.10d
 *  Updated on: May 27.2024
 *      Author: Wolle (schreibfaul1)
 *
 */

// TODO прибрати статику

#pragma GCC optimize("O3")

#include "Audio.h"
#include "mp3_decoder/mp3_decoder.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AudioBuffer::AudioBuffer(size_t maxBlockSize)
{
    // if maxBlockSize isn't set use defaultspace (1600 bytes) is enough for aac and mp3 player
    if (maxBlockSize)
        m_resBuffSizeRAM = maxBlockSize;
    if (maxBlockSize)
        m_maxBlockSize = maxBlockSize;
}

AudioBuffer::~AudioBuffer()
{
    if (m_buffer)
        free(m_buffer);
    m_buffer = NULL;
}

void AudioBuffer::setBufsize(int ram, int psram)
{
    if (ram > -1) // -1 == default / no change
        m_buffSizeRAM = ram;
    if (psram > -1)
        m_buffSizePSRAM = psram;
}

int32_t AudioBuffer::getBufsize() { return m_buffSize; }

size_t AudioBuffer::init()
{
    if (m_buffer)
        free(m_buffer);
    m_buffer = NULL;
    if (psramInit() && m_buffSizePSRAM > 0)
    {
        // PSRAM found, AudioBuffer will be allocated in PSRAM
        m_buffSize = m_buffSizePSRAM;
        m_buffer = (uint8_t *)ps_calloc(m_buffSize, sizeof(uint8_t));
        m_buffSize = m_buffSizePSRAM - m_resBuffSizePSRAM;
    }
    if (m_buffer == NULL)
    {
        // PSRAM not found, not configured or not enough available
        m_buffer = (uint8_t *)heap_caps_calloc(m_buffSizeRAM, sizeof(uint8_t), MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
        m_buffSize = m_buffSizeRAM - m_resBuffSizeRAM;
    }
    if (!m_buffer)
        return 0;
    m_f_init = true;
    resetBuffer();
    return m_buffSize;
}

void AudioBuffer::changeMaxBlockSize(uint16_t mbs)
{
    m_maxBlockSize = mbs;
    return;
}

uint16_t AudioBuffer::getMaxBlockSize() { return m_maxBlockSize; }

size_t AudioBuffer::freeSpace()
{
    if (m_readPtr >= m_writePtr)
    {
        m_freeSpace = (m_readPtr - m_writePtr);
    }
    else
    {
        m_freeSpace = (m_endPtr - m_writePtr) + (m_readPtr - m_buffer);
    }
    if (m_f_start)
        m_freeSpace = m_buffSize;
    return m_freeSpace - 1;
}

size_t AudioBuffer::writeSpace()
{
    if (m_readPtr >= m_writePtr)
    {
        m_writeSpace = (m_readPtr - m_writePtr - 1); // readPtr must not be overtaken
    }
    else
    {
        if (getReadPos() == 0)
            m_writeSpace = (m_endPtr - m_writePtr - 1);
        else
            m_writeSpace = (m_endPtr - m_writePtr);
    }
    if (m_f_start)
        m_writeSpace = m_buffSize - 1;
    return m_writeSpace;
}

size_t AudioBuffer::bufferFilled()
{
    if (m_writePtr >= m_readPtr)
    {
        m_dataLength = (m_writePtr - m_readPtr);
    }
    else
    {
        m_dataLength = (m_endPtr - m_readPtr) + (m_writePtr - m_buffer);
    }
    return m_dataLength;
}

size_t AudioBuffer::getMaxAvailableBytes()
{
    if (m_writePtr >= m_readPtr)
    {
        m_dataLength = (m_writePtr - m_readPtr - 1);
    }
    else
    {
        m_dataLength = (m_endPtr - m_readPtr);
    }
    return m_dataLength;
}

void AudioBuffer::bytesWritten(size_t bw)
{
    m_writePtr += bw;
    if (m_writePtr == m_endPtr)
    {
        m_writePtr = m_buffer;
    }
    if (bw && m_f_start)
        m_f_start = false;
}

void AudioBuffer::bytesWasRead(size_t br)
{
    m_readPtr += br;
    if (m_readPtr >= m_endPtr)
    {
        size_t tmp = m_readPtr - m_endPtr;
        m_readPtr = m_buffer + tmp;
    }
}

uint8_t *AudioBuffer::getWritePtr() { return m_writePtr; }

uint8_t *AudioBuffer::getReadPtr()
{
    size_t len = m_endPtr - m_readPtr;
    if (len < m_maxBlockSize)
    {                                                     // be sure the last frame is completed
        memcpy(m_endPtr, m_buffer, m_maxBlockSize - len); // cpy from m_buffer to m_endPtr with len
    }
    return m_readPtr;
}

void AudioBuffer::resetBuffer()
{
    m_writePtr = m_buffer;
    m_readPtr = m_buffer;
    m_endPtr = m_buffer + m_buffSize;
    m_f_start = true;
    // memset(m_buffer, 0, m_buffSize); //Clear Inputbuffer
}

uint32_t AudioBuffer::getWritePos() { return m_writePtr - m_buffer; }

uint32_t AudioBuffer::getReadPos() { return m_readPtr - m_buffer; }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// clang-format off
Audio::Audio(bool internalDAC /* = false */, uint8_t channelEnabled /* = I2S_SLOT_MODE_STEREO */, uint8_t i2sPort) {

    mutex_audio = xSemaphoreCreateMutex();

#define __malloc_heap_psram(size) \
    heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL)

    if(psramInit()) m_ibuffSize = 4096; else m_ibuffSize = 512 + 64;
    m_outBuff = (int16_t*)__malloc_heap_psram(m_outbuffSize);

    if(!m_outBuff) 
        log_e("oom");

    m_f_channelEnabled = channelEnabled;
    m_f_internalDAC = internalDAC;
    m_i2s_num = i2sPort;  // i2s port number

    // -------- I2S configuration -------------------------------------------------------------------------------------------
#if ESP_IDF_VERSION_MAJOR == 5
    m_i2s_chan_cfg.id            = (i2s_port_t)m_i2s_num;  // I2S_NUM_AUTO, I2S_NUM_0, I2S_NUM_1
    m_i2s_chan_cfg.role          = I2S_ROLE_MASTER;        // I2S controller master role, bclk and lrc signal will be set to output
    m_i2s_chan_cfg.dma_desc_num  = 16;                     // number of DMA buffer
    m_i2s_chan_cfg.dma_frame_num = 512;                    // I2S frame number in one DMA buffer.
    m_i2s_chan_cfg.auto_clear    = true;                   // i2s will always send zero automatically if no data to send
    i2s_new_channel(&m_i2s_chan_cfg, &m_i2s_tx_handle, NULL);

    m_i2s_std_cfg.slot_cfg                = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO); // Set to enable bit shift in Philips mode
    m_i2s_std_cfg.gpio_cfg.bclk           = I2S_GPIO_UNUSED;           // BCLK, Assignment in setPinout()
    m_i2s_std_cfg.gpio_cfg.din            = I2S_GPIO_UNUSED;           // not used
    m_i2s_std_cfg.gpio_cfg.dout           = I2S_GPIO_UNUSED;           // DOUT, Assignment in setPinout()
    m_i2s_std_cfg.gpio_cfg.mclk           = I2S_GPIO_UNUSED;           // MCLK, Assignment in setPinout()
    m_i2s_std_cfg.gpio_cfg.ws             = I2S_GPIO_UNUSED;           // LRC,  Assignment in setPinout()
    m_i2s_std_cfg.gpio_cfg.invert_flags.mclk_inv = false;
    m_i2s_std_cfg.gpio_cfg.invert_flags.bclk_inv = false;
    m_i2s_std_cfg.gpio_cfg.invert_flags.ws_inv   = false;
    m_i2s_std_cfg.clk_cfg.sample_rate_hz = 44100;
    m_i2s_std_cfg.clk_cfg.clk_src        = I2S_CLK_SRC_DEFAULT;        // Select PLL_F160M as the default source clock
    m_i2s_std_cfg.clk_cfg.mclk_multiple  = I2S_MCLK_MULTIPLE_128;      // mclk = sample_rate * 256
    i2s_channel_init_std_mode(m_i2s_tx_handle, &m_i2s_std_cfg);
    I2Sstart(0);
    m_sampleRate = 44100;
#else
    m_i2s_config.sample_rate          = 44100;
    m_i2s_config.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
    m_i2s_config.channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT;
    m_i2s_config.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1; // interrupt priority
    m_i2s_config.dma_buf_count        = 16;
    m_i2s_config.dma_buf_len          = 512;
    m_i2s_config.use_apll             = false; // must be disabled in V2.0.1-RC1
    m_i2s_config.tx_desc_auto_clear   = true;   // new in V1.0.1
    m_i2s_config.fixed_mclk           = true;
    m_i2s_config.mclk_multiple        = I2S_MCLK_MULTIPLE_128;

    if (internalDAC)  {
        #ifdef CONFIG_IDF_TARGET_ESP32  // ESP32S3 has no DAC
        printf("internal DAC");
        m_i2s_config.mode             = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN );
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_MSB); // vers >= 2.0.5
        i2s_driver_install((i2s_port_t)m_i2s_num, &m_i2s_config, 0, NULL);
        i2s_set_dac_mode((i2s_dac_mode_t)m_f_channelEnabled);
        if(m_f_channelEnabled != I2S_DAC_CHANNEL_BOTH_EN) {
            m_f_forceMono = true;
        }
        #endif
    }
    else {
        m_i2s_config.mode             = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S); // Arduino vers. > 2.0.0
        i2s_driver_install((i2s_port_t)m_i2s_num, &m_i2s_config, 0, NULL);
        m_f_forceMono = false;
    }
    i2s_zero_dma_buffer((i2s_port_t) m_i2s_num);

#endif // ESP_IDF_VERSION_MAJOR == 5
    for(int i = 0; i < 3; ++i) {
        m_filter[i].a0 = 1;
        m_filter[i].a1 = 0;
        m_filter[i].a2 = 0;
        m_filter[i].b1 = 0;
        m_filter[i].b2 = 0;
    }
    computeLimit();  // first init, vol = 21, vol_steps = 21
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Audio::~Audio() {
    // I2Sstop(m_i2s_num);
    // InBuff.~AudioBuffer(); #215 the AudioBuffer is automatically destroyed by the destructor
    setDefaults();
#if ESP_IDF_VERSION_MAJOR == 5
    i2s_del_channel(m_i2s_tx_handle);
#else
    i2s_driver_uninstall((i2s_port_t)m_i2s_num); // #215 free I2S buffer
#endif
    if(m_outBuff)     {free(m_outBuff);      m_outBuff      = NULL; }

    vSemaphoreDelete(mutex_audio);
}
// clang-format on
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setBufsize(int rambuf_sz, int psrambuf_sz)
{
    if (InBuff.isInitialized())
    {
        log_e("Audio::setBufsize must not be called after audio is initialized");
        return;
    }
    InBuff.setBufsize(rambuf_sz, psrambuf_sz);
};

void Audio::initInBuff()
{
    if (!InBuff.isInitialized())
    {
        size_t size = InBuff.init();
    }
    changeMaxBlockSize(1600); // default size mp3 or aac
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
esp_err_t Audio::I2Sstart(uint8_t i2s_num)
{
#if ESP_IDF_VERSION_MAJOR == 5
    return i2s_channel_enable(m_i2s_tx_handle);
#else
    // It is not necessary to call this function after i2s_driver_install() (it is started automatically),
    // however it is necessary to call it after i2s_stop()
    return i2s_start((i2s_port_t)i2s_num);
#endif
}

esp_err_t Audio::I2Sstop(uint8_t i2s_num)
{
#if ESP_IDF_VERSION_MAJOR == 5
    return i2s_channel_disable(m_i2s_tx_handle);
#else
    return i2s_stop((i2s_port_t)i2s_num);
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setDefaults()
{
    stopSong();
    initInBuff(); // initialize InputBuffer if not already done
    InBuff.resetBuffer();
    MP3Decoder_FreeBuffers();

    m_hashQueue.clear();
    m_hashQueue.shrink_to_fit(); // uint32_t vector

    m_f_playing = false;
    m_f_firstCall = true;        // InitSequence for processWebstream and processLocalFile
    m_f_firstCurTimeCall = true; // InitSequence for computeAudioTime
    m_f_running = false;
    m_f_loop = false;   // Set if audio file should loop
    m_f_exthdr = false; // ID3 extended header

    m_audioCurrentTime = 0; // Reset playtimer
    m_audioFileDuration = 0;
    m_audioDataStart = 0;
    m_audioDataSize = 0;
    m_avr_bitrate = 0;   // the same as m_bitrate if CBR, median if VBR
    m_bitRate = 0;       // Bitrate still unknown
                         // byteCounter = 0;     // count received data
    m_contentlength = 0; // If Content-Length is known, count it
    m_curSample = 0;
    m_controlCounter = 0; // Status within readID3data() and readWaveHeader()
    m_channels = 2;       // assume stereo #209
    m_fileSize = 0;
    m_haveNewFilePos = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setFileLoop(bool input)
{
    m_f_loop = input;
    return input;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// clang-format off
void Audio::UTF8toASCII(char* str) {

    const uint8_t ascii[60] = {
    //129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148  // UTF8(C3)
    //                Ä    Å    Æ    Ç         É                                       Ñ                  // CHAR
      000, 000, 000, 142, 143, 146, 128, 000, 144, 000, 000, 000, 000, 000, 000, 000, 165, 000, 000, 000, // ASCII
    //149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168
    //      Ö                             Ü              ß    à                   ä    å    æ         è
      000, 153, 000, 000, 000, 000, 000, 154, 000, 000, 225, 133, 000, 000, 000, 132, 134, 145, 000, 138,
    //169, 170, 171, 172. 173. 174. 175, 176, 177, 179, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188
    //      ê    ë    ì         î    ï         ñ    ò         ô         ö              ù         û    ü
      000, 136, 137, 141, 000, 140, 139, 000, 164, 149, 000, 147, 000, 148, 000, 000, 151, 000, 150, 129};

    uint16_t i = 0, j = 0, s = 0;
    bool     f_C3_seen = false;

    while(str[i] != 0) {    // convert UTF8 to ASCII
        if(str[i] == 195) { // C3
            ++i;
            f_C3_seen = true;
            continue;
        }
        str[j] = str[i];
        if(str[j] > 128 && str[j] < 189 && f_C3_seen == true) {
            s = ascii[str[j] - 129];
            if(s != 0) str[j] = s; // found a related ASCII sign
            f_C3_seen = false;
        }
        ++i;
        ++j;
    }
    str[j] = 0;
}
// clang-format on
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::connecttoFS(meow::FileManager &f_mngr, const char *path, int32_t fileStartPos)
{
    if (fileStartPos < 10)
        fileStartPos = 0;

    if (!path)
    { // guard
        log_e("The path ore file name is empty");
        return false;
    }

    xSemaphoreTakeRecursive(mutex_audio, portMAX_DELAY); // #3

    m_fileStartPos = fileStartPos;

    setDefaults(); // free buffers an set defaults

    char *audioPath = (char *)__malloc_heap_psram(strlen(path) + 2);
    if (!audioPath)
    {
        log_e("Out of memory");
        xSemaphoreGiveRecursive(mutex_audio);
        return false;
    }
    if (path[0] == '/')
    {
        strcpy(audioPath, path);
    }
    else
    {
        audioPath[0] = '/';
        strcpy(audioPath + 1, path);
    }

    if (!f_mngr.fileExist(audioPath))
    {
        UTF8toASCII(audioPath);

        if (!f_mngr.fileExist(audioPath))
        {
            log_e("File doesn't exist");
            xSemaphoreGiveRecursive(mutex_audio);
            free(audioPath);
            return false;
        }
    }

    audiofile = f_mngr.getFileDescriptor(audioPath, "r");

    if (!audiofile)
    {
        log_e("Failed to open file for reading");
        free(audioPath);
        xSemaphoreGiveRecursive(mutex_audio);
        return false;
    }

    m_fileSize = f_mngr.getFileSize(audioPath);
    _audio_size = m_fileSize;

    free(audioPath);

    _f_mgr = &f_mngr;

    bool ret = initializeDecoder();
    if (ret)
        m_f_running = true;
    else
        f_mngr.closeFile(audiofile);
    xSemaphoreGiveRecursive(mutex_audio);
    return ret;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t Audio::readAudioHeader(uint32_t bytes)
{
    size_t bytesRead = 0;

    int res = read_ID3_Header(InBuff.getReadPtr(), bytes);

    if (res >= 0)
        bytesRead = res;
    else
    { // error, skip header
        m_controlCounter = 100;
    }

    if (!isRunning())
    {
        log_e("Processing stopped due to invalid audio header");
        return 0;
    }
    return bytesRead;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Audio::read_ID3_Header(uint8_t *data, size_t len)
{
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 0)
    { /* read ID3 tag and ID3 header size */

        ID3version = 0;
        m_contentlength = getFileSize();

        m_controlCounter++;
        remainingHeaderBytes = 0;
        ehsz = 0;
        if (specialIndexOf(data, "ID3", 4) != 0)
        { // ID3 not found
            m_audioDataSize = m_contentlength;
            return -1; // error, no ID3 signature found
        }
        ID3version = *(data + 3);
        switch (ID3version)
        {
        case 2:
            m_f_exthdr = false;
            break;
        case 3:
        case 4:
            m_f_exthdr = (*(data + 5) & 0x40); // bit6 extended header
            break;
        };
        id3Size = bigEndian(data + 6, 4, 7); //  ID3v2 size  4 * %0xxxxxxx (shift left seven times!!)
        id3Size += 10;

        if (ID3version == 2)
        {
            m_controlCounter = 10;
        }
        remainingHeaderBytes = id3Size;
        remainingHeaderBytes -= 10;

        return 10;
    }
    else if (m_controlCounter == 1)
    { // compute extended header size if exists
        m_controlCounter++;
        if (m_f_exthdr)
        {
            ehsz = bigEndian(data, 4);
            remainingHeaderBytes -= 4;
            ehsz -= 4;
            return 4;
        }
        else
            return 0;
    }
    else if (m_controlCounter == 2)
    { // skip extended header if exists
        if (ehsz > len)
        {
            ehsz -= len;
            remainingHeaderBytes -= len;
            return len;
        } // Throw it away
        else
        {
            ++m_controlCounter;
            remainingHeaderBytes -= ehsz;
            return ehsz;
        } // Throw it away
    }
    else if (m_controlCounter == 3)
    { // read a ID3 frame, get the tag
        if (remainingHeaderBytes == 0)
        {
            m_controlCounter = 99;
            return 0;
        }
        ++m_controlCounter;
        frameid[0] = *(data + 0);
        frameid[1] = *(data + 1);
        frameid[2] = *(data + 2);
        frameid[3] = *(data + 3);
        frameid[4] = 0;
        for (uint8_t i = 0; i < 4; ++i)
            tag[i] = frameid[i]; // tag = frameid

        remainingHeaderBytes -= 4;
        if (frameid[0] == 0 && frameid[1] == 0 && frameid[2] == 0 && frameid[3] == 0)
        {
            // We're in padding
            m_controlCounter = 98; // all ID3 metadata processed
        }
        return 4;
    }
    else if (m_controlCounter == 4)
    { // get the frame size
        m_controlCounter = 6;

        if (ID3version == 4)
            framesize = bigEndian(data, 4, 7); // << 7
        else
            framesize = bigEndian(data, 4); // << 8

        remainingHeaderBytes -= 4;
        uint8_t flag = *(data + 4); // skip 1st flag
        (void)flag;
        --remainingHeaderBytes;
        compressed = (*(data + 5)) & 0x80; // Frame is compressed using [#ZLIB zlib] with 4 bytes for 'decompressed
                                           // size' appended to the frame header.
        --remainingHeaderBytes;
        if (compressed)
        {
            remainingHeaderBytes -= 4;
            return 6 + 4;
        }
        return 6;
    }
    else if (m_controlCounter == 5)
    { // If the frame is larger than 512 bytes, skip the rest
        if (framesize > len)
        {
            framesize -= len;
            remainingHeaderBytes -= len;
            return len;
        }
        else
        {
            m_controlCounter = 3; // check next frame
            remainingHeaderBytes -= framesize;
            return framesize;
        }
    }
    else if (m_controlCounter == 6)
    {                                       // Read the value
        m_controlCounter = 5;               // only read 256 bytes
        uint8_t encodingByte = *(data + 0); // ID3v2 Text-Encoding-Byte
        // $00 – ISO-8859-1 (LATIN-1, Identical to ASCII for values smaller than 0x80).
        // $01 – UCS-2 encoded Unicode with BOM (Byte Order Mark), in ID3v2.2 and ID3v2.3.
        // $02 – UTF-16BE encoded Unicode without BOM (Byte Order Mark) , in ID3v2.4.
        // $03 – UTF-8 encoded Unicode, in ID3v2.4.

        if (startsWith(tag, "APIC") || startsWith(tag, "SYLT") || startsWith(tag, "TXXX") || startsWith(tag, "USLT"))
            return 0;

        if (framesize == 0)
            return 0;
        size_t fs = framesize;
        if (fs > 1024)
            fs = 1024;
        uint16_t dataLength = fs - 1;
        framesize -= fs;
        remainingHeaderBytes -= fs;

        return fs;
    }
    else if (m_controlCounter == 10)
    {
        if (universal_tmp > 0)
        {
            if (universal_tmp > len)
            {
                universal_tmp -= len;
                return len;
            } // Throw it away
            else
            {
                uint32_t t = universal_tmp;
                universal_tmp = 0;
                return t;
            } // Throw it away
        }

        frameid[0] = *(data + 0);
        frameid[1] = *(data + 1);
        frameid[2] = *(data + 2);
        frameid[3] = 0;
        for (uint8_t i = 0; i < 4; ++i)
            tag[i] = frameid[i]; // tag = frameid
        remainingHeaderBytes -= 3;
        size_t dataLen = bigEndian(data + 3, 3);
        universal_tmp = dataLen;
        remainingHeaderBytes -= 3;

        remainingHeaderBytes -= universal_tmp;
        universal_tmp -= dataLen;

        if (dataLen == 0)
            m_controlCounter = 98;
        if (remainingHeaderBytes == 0)
            m_controlCounter = 98;

        return 3 + 3 + dataLen;
    }
    else if (m_controlCounter == 98)
    { // skip all ID3 metadata (mostly spaces)
        if (remainingHeaderBytes > len)
        {
            remainingHeaderBytes -= len;
            return len;
        } // Throw it away
        else
        {
            m_controlCounter = 99;
            return remainingHeaderBytes;
        } // Throw it away
    }
    else if (m_controlCounter == 99)
    { //  exist another ID3tag?
        m_audioDataStart += id3Size;
        //    vTaskDelay(30);
        if ((*(data + 0) == 'I') && (*(data + 1) == 'D') && (*(data + 2) == '3'))
        {
            m_controlCounter = 0;
            ++numID3Header;
            totalId3Size += id3Size;
            return 0;
        }
        else
        {
            m_controlCounter = 100; // ok
            m_audioDataSize = m_contentlength - m_audioDataStart;

            numID3Header = 0;
            totalId3Size = 0;

            return 0;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::stopSong()
{
    uint32_t pos = 0;
    if (m_f_running)
    {
        m_f_running = false;
        pos = getFilePos() - inBufferFilled();
    }

    // added this before putting 'm_f_localfile = false' in stopSong(); shoulf never occur....
    _f_mgr->closeFile(audiofile);

    memset(m_outBuff, 0, m_outbuffSize);           // Clear OutputBuffer
    memset(m_filterBuff, 0, sizeof(m_filterBuff)); // Clear FilterBuffer
    m_validSamples = 0;
    m_audioCurrentTime = 0;
    m_audioFileDuration = 0;
    return pos;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::pauseResume()
{
    xSemaphoreTakeRecursive(mutex_audio, portMAX_DELAY);
    bool retVal = false;

    m_f_running = !m_f_running;
    retVal = true;
    if (!m_f_running)
    {
        memset(m_outBuff, 0, m_outbuffSize); // Clear OutputBuffer
        m_validSamples = 0;
    }

    xSemaphoreGiveRecursive(mutex_audio);
    return retVal;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::playChunk(bool i2s_only)
{
    int16_t validSamples = 0;
    size_t i2s_bytesConsumed = 0;
    int16_t *sample[2] = {0};
    int16_t *s2;
    int sampleSize = 4; // 2 bytes per sample (int16_t) * 2 channels
    esp_err_t err = ESP_OK;
    int i = 0;

    if (count > 0)
        goto i2swrite;

    if (getChannels() == 1)
    {
        for (int i = m_validSamples - 1; i >= 0; --i)
        {
            int16_t sample = m_outBuff[i];
            m_outBuff[2 * i] = sample;
            m_outBuff[2 * i + 1] = sample;
        }
        //    m_validSamples *= 2;
    }

    validSamples = m_validSamples;

    while (validSamples)
    {
        *sample = m_outBuff + i;
        computeVUlevel(*sample);

        //---------- Filterchain, can commented out if not used-------------
        {
            if (m_corr > 1)
            {
                s2 = *sample;
                s2[LEFTCHANNEL] /= m_corr;
                s2[RIGHTCHANNEL] /= m_corr;
            }
            IIR_filterChain0(*sample);
            IIR_filterChain1(*sample);
            IIR_filterChain2(*sample);
        }
        //------------------------------------------------------------------
        if (m_f_forceMono && m_channels == 2)
        {
            int32_t xy = ((*sample)[RIGHTCHANNEL] + (*sample)[LEFTCHANNEL]) / 2;
            (*sample)[RIGHTCHANNEL] = (int16_t)xy;
            (*sample)[LEFTCHANNEL] = (int16_t)xy;
        }
        Gain(*sample);

        if (m_f_internalDAC)
        {
            s2 = *sample;
            s2[LEFTCHANNEL] += 0x8000;
            s2[RIGHTCHANNEL] += 0x8000;
        }
        i += 2;
        validSamples -= 1;
    }

i2swrite:

    validSamples = m_validSamples;

    err = i2s_write((i2s_port_t)m_i2s_num, (int16_t *)m_outBuff + count, validSamples * sampleSize, &i2s_bytesConsumed, 10);

    if (!(err == ESP_OK || err == ESP_ERR_TIMEOUT))
        goto exit;
    m_validSamples -= i2s_bytesConsumed / sampleSize;
    count += i2s_bytesConsumed / 2;
    if (m_validSamples < 0)
    {
        m_validSamples = 0;
    }
    if (m_validSamples == 0)
    {
        count = 0;
    }

    return;
exit:
    if (err == ESP_OK)
        return;
    else if (err == ESP_ERR_INVALID_ARG)
        log_e("NULL pointer or this handle is not tx handle");
    else if (err == ESP_ERR_TIMEOUT)
        log_e("Writing timeout, no writing event received from ISR within ticks_to_wait");
    else if (err == ESP_ERR_INVALID_STATE)
        log_e("I2S is not ready to write");
    else
        log_e("i2s err %i", err);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::loop()
{
    if (!m_f_running)
        return;

    xSemaphoreTakeRecursive(mutex_audio, portMAX_DELAY);
    if (m_validSamples)
        playChunk(true);
    else
        processLocalFile();
    xSemaphoreGiveRecursive(mutex_audio);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::processLocalFile()
{
    if (!audiofile)
        return;

    const uint32_t timeout = 2500;                          // ms
    const uint32_t maxFrameSize = InBuff.getMaxBlockSize(); // every mp3/aac frame is not bigger
    uint32_t availableBytes = 0;

    if (m_f_firstCall)
    { // runs only one time per connection, prepare for start
        m_f_firstCall = false;
        f_stream = false;
        f_fileDataComplete = false;
        byteCounter = 0;
        ctime = millis();
        if (m_resumeFilePos == 0)
            m_resumeFilePos = -1; // parkposition
        return;
    }

    availableBytes = 256 * 1024; // set some large value

    availableBytes = min(availableBytes, (uint32_t)InBuff.writeSpace());
    availableBytes = min(availableBytes, _audio_size - byteCounter);
    if (m_contentlength)
    {
        size_t file_pos = _f_mgr->getPos(audiofile);
        if (m_contentlength > file_pos)
            availableBytes = min(availableBytes, m_contentlength - file_pos);
    }
    if (m_audioDataSize)
    {
        availableBytes = min(availableBytes, m_audioDataSize + m_audioDataStart - byteCounter);
    }

    if (_f_mgr->readFromFile(audiofile, InBuff.getWritePtr(), availableBytes))
    {
        byteCounter += availableBytes; // Pull request #42
        InBuff.bytesWritten(availableBytes);
    }

    if (!f_stream)
    {
        if (m_controlCounter != 100)
        {
            if ((millis() - ctime) > timeout)
            {
                log_e("audioHeader reading timeout");
                m_f_running = false;
                return;
            }
            if (InBuff.bufferFilled() > maxFrameSize || InBuff.bufferFilled() == m_fileSize)
            { // read the file header first
                InBuff.bytesWasRead(readAudioHeader(InBuff.getMaxAvailableBytes()));
            }
            return;
        }
        else
        {
            if ((InBuff.freeSpace() > maxFrameSize) && (m_fileSize - byteCounter) > maxFrameSize && availableBytes)
            {
                // fill the buffer before playing
                return;
            }

            f_stream = true;
        }
    }

    if (m_fileStartPos > 0)
    {
        _need_calc_offset = true;
        setFilePos(m_fileStartPos);
        m_fileStartPos = -1;
    }

    if (m_resumeFilePos >= 0)
    {
        if (m_resumeFilePos < (int32_t)m_audioDataStart)
            m_resumeFilePos = m_audioDataStart;
        if (m_resumeFilePos >= (int32_t)m_audioDataStart + m_audioDataSize)
        {
            goto exit;
        }

        m_resumeFilePos = mp3_correctResumeFilePos(m_resumeFilePos);
        if (m_resumeFilePos == -1)
            goto exit;

        _f_mgr->seekPos(audiofile, m_resumeFilePos);
        InBuff.resetBuffer();
        byteCounter = m_resumeFilePos;
        f_fileDataComplete = false; // #570

        m_resumeFilePos = -1;
        f_stream = false;
    }
    // end of file reached? - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (f_fileDataComplete && InBuff.bufferFilled() < InBuff.getMaxBlockSize())
    {
        if (InBuff.bufferFilled())
        {
            if (!readID3V1Tag())
            {
                if (m_validSamples)
                {
                    playChunk(false);
                    return;
                } // play samples first
                int bytesDecoded = sendBytes(InBuff.getReadPtr(), InBuff.bufferFilled());
                if (bytesDecoded <= InBuff.bufferFilled())
                { // avoid InBuff overrun (can be if file is corrupt)
                    if (m_f_playing)
                    {
                        if (bytesDecoded > 2)
                        {
                            InBuff.bytesWasRead(bytesDecoded);
                            return;
                        }
                    }
                }
            }
        }

        if (m_f_loop && f_stream)
        { // eof
            setFilePos(m_audioDataStart);
            m_audioCurrentTime = 0;
            byteCounter = m_audioDataStart;
            f_fileDataComplete = false;
            return;
        } // loop
    exit:
        m_f_running = false;
        _f_mgr->closeFile(audiofile);

        MP3Decoder_FreeBuffers();

        m_audioCurrentTime = 0;
        m_audioFileDuration = 0;
        m_resumeFilePos = -1;
        m_haveNewFilePos = 0;

        return;
    }
    if (byteCounter == _audio_size)
    {
        f_fileDataComplete = true;
    }
    if (byteCounter == m_audioDataSize + m_audioDataStart)
    {
        f_fileDataComplete = true;
    }
    // play audio data - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (f_stream)
    {
        playAudioData();
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::playAudioData()
{
    if (m_validSamples)
    {
        playChunk(false);
        return;
    } // play samples first
    if (InBuff.bufferFilled() < InBuff.getMaxBlockSize())
        return; // guard

    int bytesDecoded = sendBytes(InBuff.getReadPtr(), InBuff.getMaxBlockSize());

    if (!m_f_running)
        return;

    if (bytesDecoded < 0)
    { // no syncword found or decode error, try next chunk
        log_e("err bytesDecoded %i", bytesDecoded);
        uint8_t next = 200;
        if (InBuff.bufferFilled() < next)
            next = InBuff.bufferFilled();
        InBuff.bytesWasRead(next); // try next chunk
    }
    else
    {
        if (bytesDecoded > 0)
        {
            InBuff.bytesWasRead(bytesDecoded);
            return;
        }
        if (bytesDecoded == 0)
            return; // syncword at pos0
    }

    return;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::initializeDecoder()
{
    uint32_t gfH = 0;
    uint32_t hWM = 0;

    if (!MP3Decoder_IsInit())
    {
        if (!MP3Decoder_AllocateBuffers())
        {
            goto exit;
        }
        gfH = ESP.getFreeHeap();
        hWM = uxTaskGetStackHighWaterMark(NULL);
        InBuff.changeMaxBlockSize(m_frameSizeMP3);
    }
    return true;

exit:
    stopSong();
    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Audio::findNextSync(uint8_t *data, size_t len)
{
    // Mp3 and aac audio data are divided into frames. At the beginning of each frame there is a sync word.
    // The sync word is 0xFFF. This is followed by information about the structure of the frame.
    // Wav files have no frames
    // Return: 0 the synchronous word was found at position 0
    //         > 0 is the offset to the next sync word
    //         -1 the sync word was not found within the block with the length len

    int nextSync;
    nextSync = MP3FindSyncWord(data, len);
    if (nextSync == -1)
        return len; // syncword not found, search next block

    return nextSync;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setDecoderItems()
{
    setChannels(MP3GetChannels());
    setSampleRate(MP3GetSampRate());
    setBitsPerSample(MP3GetBitsPerSample());
    setBitrate(MP3GetBitrate());

    if (m_bitsPerSample != 8 && m_bitsPerSample != 16)
    {
        stopSong();
    }

    if (m_channels != 1 && m_channels != 2)
    {
        stopSong();
    }

    reconfigI2S();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Audio::sendBytes(uint8_t *data, size_t len)
{
    int32_t bytesLeft;
    int nextSync = 0;
    if (!m_f_playing)
    {
        f_setDecodeParamsOnce = true;
        nextSync = findNextSync(data, len);
        if (nextSync == -1)
            return len;
        if (nextSync == 0)
        {
            m_f_playing = true;
        }
        return nextSync;
    }
    // m_f_playing is true at this pos
    bytesLeft = len;
    m_decodeError = 0;
    int bytesDecoded = 0;

    m_decodeError = MP3Decode(data, &bytesLeft, m_outBuff, 0);

    // m_decodeError - possible values are:
    //                   0: okay, no error
    //                 100: the decoder needs more data
    //                 < 0: there has been an error

    if (m_decodeError < 0)
    {                        // Error, skip the frame...
        m_f_playing = false; // seek for new syncword
        return 1;            // skip one byte and seek for the next sync word
    }
    bytesDecoded = len - bytesLeft;

    if (bytesDecoded == 0 && m_decodeError == 0)
    { // unlikely framesize
        log_e("framesize is 0, start decoding again");
        m_f_playing = false; // seek for new syncword
        // we're here because there was a wrong sync word so skip one byte and seek for the next
        return 1;
    }
    // status: bytesDecoded > 0 and m_decodeError >= 0
    char *st = NULL;
    std::vector<uint32_t> vec;

    m_validSamples = MP3GetOutputSamps() / getChannels();

    if (f_setDecodeParamsOnce && m_validSamples)
    {
        f_setDecodeParamsOnce = false;
        setDecoderItems();
        m_PlayingStartTime = millis();
    }

    uint16_t bytesDecoderOut = m_validSamples;
    if (m_channels == 2)
        bytesDecoderOut /= 2;
    if (m_bitsPerSample == 16)
        bytesDecoderOut *= 2;

    computeAudioTime(bytesDecoded);

    m_curSample = 0;
    playChunk(false);
    return bytesDecoded;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::computeAudioTime(uint16_t bytesDecoderIn)
{
    if (m_f_firstCurTimeCall)
    { // first call
        m_f_firstCurTimeCall = false;
        _sumBytesIn = 0;
        _sumBitRate = 0;
        _frame_counter = 0;
        _timeStamp = millis();
        _deltaBytesIn = 0;
        _need_calc_br = true;
    }

    _sumBytesIn += bytesDecoderIn;
    _deltaBytesIn += bytesDecoderIn;

    if (m_avr_bitrate)
    {
        m_audioCurrentTime = (float)(_sumBytesIn * 8) / m_avr_bitrate;

        if (m_haveNewFilePos > 0)
        {
            if (_need_calc_offset)
            {
                _need_calc_offset = false;

                if (m_haveNewFilePos < 3)
                    m_haveNewFilePos = 3;

                setTimeOffset(m_haveNewFilePos - 3);
            }

            _sumBytesIn = m_haveNewFilePos - m_audioDataStart;

            m_haveNewFilePos = 0;
        }
    }

    if (_need_calc_br && _timeStamp + 3000 < millis())
    {
        uint32_t t = millis();             // time tracking
        uint32_t delta_t = t - _timeStamp; //    ---"---
        _timeStamp = t;                    //    ---"---

        uint32_t bitRate = (float)(_deltaBytesIn * 8000) / delta_t; // we know the time and bytesIn to compute the bitrate

        _sumBitRate += bitRate;
        ++_frame_counter;

        m_avr_bitrate = (float)_sumBitRate / _frame_counter;
        m_audioFileDuration = round((float)(m_audioDataSize * 8) / m_avr_bitrate);

        _deltaBytesIn = 0;
        _need_calc_br = false;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setPinout(uint8_t BCLK, uint8_t LRC, uint8_t DOUT, int8_t MCLK)
{
    esp_err_t result = ESP_OK;

#if (ESP_ARDUINO_VERSION_MAJOR < 2)
    log_e("Arduino Version too old!");
#endif
#if (ESP_ARDUINO_VERSION_MAJOR == 2 && ESP_ARDUINO_VERSION_PATCH < 8)
    log_e("Arduino Version must be 2.0.8 or higher!");
#endif

#if (ESP_IDF_VERSION_MAJOR == 5)
    i2s_std_gpio_config_t gpio_cfg = {};
    gpio_cfg.bclk = (gpio_num_t)BCLK;
    gpio_cfg.din = (gpio_num_t)I2S_GPIO_UNUSED;
    gpio_cfg.dout = (gpio_num_t)DOUT;
    gpio_cfg.mclk = (gpio_num_t)MCLK;
    gpio_cfg.ws = (gpio_num_t)LRC;
    I2Sstop(0);
    result = i2s_channel_reconfig_std_gpio(m_i2s_tx_handle, &gpio_cfg);
    I2Sstart(0);
#else
    m_pin_config.bck_io_num = BCLK;
    m_pin_config.ws_io_num = LRC; //  wclk = lrc
    m_pin_config.data_out_num = DOUT;
    m_pin_config.data_in_num = I2S_GPIO_UNUSED;
    m_pin_config.mck_io_num = MCLK;
    result = i2s_set_pin((i2s_port_t)m_i2s_num, &m_pin_config);
#endif
    return (result == ESP_OK);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getFileSize()
{ // returns the size of webfile or local file
    if (!audiofile)
    {
        if (m_contentlength > 0)
        {
            return m_contentlength;
        }
        return 0;
    }
    return _audio_size;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getFilePos()
{
    if (!audiofile)
        return 0;
    return _f_mgr->getPos(audiofile);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getAudioDataStartPos()
{
    if (!audiofile)
        return 0;
    return m_audioDataStart;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getAudioFileDuration()
{
    if (!m_audioDataSize)
        return 0;

    if (!m_avr_bitrate)
        return 0;

    return m_audioFileDuration;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getAudioCurrentTime()
{ // return current time in seconds
    return round(m_audioCurrentTime);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setAudioPlayPosition(uint16_t sec)
{
    // Jump to an absolute position in time within an audio file
    // e.g. setAudioPlayPosition(300) sets the pointer at pos 5 min
    if (sec > getAudioFileDuration())
        sec = getAudioFileDuration();
    uint32_t filepos = m_audioDataStart + (m_avr_bitrate * sec / 8);
    return setFilePos(filepos);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setVolumeSteps(uint8_t steps)
{
    m_vol_steps = steps;
    if (steps < 1)
        m_vol_steps = 64; /* avoid div-by-zero :-) */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Audio::maxVolume() { return m_vol_steps; };
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getTotalPlayingTime()
{
    // Is set to zero by a connectToXXX() and starts as soon as the first audio data is available,
    // the time counting is not interrupted by a 'pause / resume' and is not reset by a fileloop
    return millis() - m_PlayingStartTime;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setTimeOffset(int sec)
{ // fast forward or rewind the current position in seconds
    if (!audiofile || !m_avr_bitrate)
        return false;

    uint32_t oneSec = m_avr_bitrate / 8;                 // bytes decoded in one sec
    int32_t offset = oneSec * sec;                       // bytes to be wind/rewind
    uint32_t startAB = m_audioDataStart;                 // audioblock begin
    uint32_t endAB = m_audioDataStart + m_audioDataSize; // audioblock end

    int32_t pos = getFilePos() - inBufferFilled();
    pos += offset;
    if (pos < (int32_t)startAB)
    {
        pos = startAB;
    }
    if (pos >= (int32_t)endAB)
    {
        pos = endAB;
    }

    setFilePos(pos);

    return true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setFilePos(uint32_t pos)
{
    if (!audiofile)
        return false;

    memset(m_outBuff, 0, m_outbuffSize);
    m_validSamples = 0;
    m_resumeFilePos = pos;  // used in processLocalFile()
    m_haveNewFilePos = pos; // used in computeAudioCurrentTime()

    return true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::audioFileSeek(const float speed)
{
    // 0.5 is half speed
    // 1.0 is normal speed
    // 1.5 is one and half speed
    if ((speed > 1.5f) || (speed < 0.25f))
        return false;

    uint32_t srate = getSampleRate() * speed;
#if ESP_IDF_VERSION_MAJOR == 5
    I2Sstop(0);
    m_i2s_std_cfg.clk_cfg.sample_rate_hz = srate;
    i2s_channel_reconfig_std_clock(m_i2s_tx_handle, &m_i2s_std_cfg.clk_cfg);
    I2Sstart(0);
#else
    i2s_set_sample_rates((i2s_port_t)m_i2s_num, srate);
#endif
    return true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setSampleRate(uint32_t sampRate)
{
    if (!sampRate)
        sampRate = 44100; // fuse, if there is no value -> set default #209
    m_sampleRate = sampRate;
    return true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getSampleRate() { return m_sampleRate; }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setBitsPerSample(int bits)
{
    if ((bits != 16) && (bits != 8))
        return false;
    m_bitsPerSample = bits;
    return true;
}
uint8_t Audio::getBitsPerSample() { return m_bitsPerSample; }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setChannels(int ch)
{
    m_channels = ch;
    return true;
}
uint8_t Audio::getChannels()
{
    if (m_channels == 0)
    { // this should not happen! #209
        m_channels = 2;
    }
    return m_channels;
}

void Audio::reconfigI2S()
{

#if ESP_IDF_VERSION_MAJOR == 5
    I2Sstop(0);
    m_i2s_std_cfg.clk_cfg.sample_rate_hz = m_sampleRate;
    i2s_channel_reconfig_std_clock(m_i2s_tx_handle, &m_i2s_std_cfg.clk_cfg);
    I2Sstart(0);
#else
    if (m_channels == 1)
    {
        m_i2s_config.channel_format = I2S_CHANNEL_FMT_ALL_RIGHT;
        i2s_set_clk((i2s_port_t)m_i2s_num, m_sampleRate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
    }
    if (m_channels == 2)
    {
        m_i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
        i2s_set_clk((i2s_port_t)m_i2s_num, m_sampleRate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
    }
#endif
    memset(m_filterBuff, 0, sizeof(m_filterBuff));        // Clear FilterBuffer
    IIR_calculateCoefficients(m_gain0, m_gain1, m_gain2); // must be recalculated after each samplerate change

    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setBitrate(int br)
{
    m_bitRate = br;
}
uint32_t Audio::getBitRate(bool avg)
{
    if (avg)
        return m_avr_bitrate;
    return m_bitRate;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setI2SCommFMT_LSB(bool commFMT)
{
    // false: I2S communication format is by default I2S_COMM_FORMAT_I2S_MSB, right->left (AC101, PCM5102A)
    // true:  changed to I2S_COMM_FORMAT_I2S_LSB for some DACs (PT8211)
    //        Japanese or called LSBJ (Least Significant Bit Justified) format

#if ESP_IDF_VERSION_MAJOR < 5
    if (commFMT)
    {
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_MSB); // v >= 2.0.0
    }
    else
    {
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S); // vers >= 2.0.0
    }

    i2s_driver_uninstall((i2s_port_t)m_i2s_num);
    i2s_driver_install((i2s_port_t)m_i2s_num, &m_i2s_config, 0, NULL);
#else
    i2s_channel_disable(m_i2s_tx_handle);
    if (commFMT)
    {
        m_i2s_std_cfg.slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    }
    else
    {
        m_i2s_std_cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    }
    i2s_channel_reconfig_std_slot(m_i2s_tx_handle, &m_i2s_std_cfg.slot_cfg);
    i2s_channel_enable(m_i2s_tx_handle);
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::computeVUlevel(int16_t sample[2])
{
    auto avg = [&](uint8_t *sampArr) { // lambda, inner function, compute the average of 8 samples
        uint16_t av = 0;
        for (int i = 0; i < 8; ++i)
        {
            av += sampArr[i];
        }
        return av >> 3;
    };

    auto largest = [&](uint8_t *sampArr) { // lambda, inner function, compute the largest of 8 samples
        uint16_t maxValue = 0;
        for (int i = 0; i < 8; ++i)
        {
            if (maxValue < sampArr[i])
                maxValue = sampArr[i];
        }
        return maxValue;
    };

    if (cnt0 == 64)
    {
        cnt0 = 0;
        ++cnt1;
    }
    if (cnt1 == 8)
    {
        cnt1 = 0;
        ++cnt2;
    }
    if (cnt2 == 8)
    {
        cnt2 = 0;
        ++cnt3;
    }
    if (cnt3 == 8)
    {
        cnt3 = 0;
        ++cnt4;
        f_vu = true;
    }
    if (cnt4 == 8)
    {
        cnt4 = 0;
    }

    if (!cnt0)
    { // store every 64th sample in the array[0]
        sampleArray[LEFTCHANNEL][0][cnt1] = abs(sample[LEFTCHANNEL] >> 7);
        sampleArray[RIGHTCHANNEL][0][cnt1] = abs(sample[RIGHTCHANNEL] >> 7);
    }
    if (!cnt1)
    { // store argest from 64 * 8 samples in the array[1]
        sampleArray[LEFTCHANNEL][1][cnt2] = largest(sampleArray[LEFTCHANNEL][0]);
        sampleArray[RIGHTCHANNEL][1][cnt2] = largest(sampleArray[RIGHTCHANNEL][0]);
    }
    if (!cnt2)
    { // store avg from 64 * 8 * 8 samples in the array[2]
        sampleArray[LEFTCHANNEL][2][cnt3] = largest(sampleArray[LEFTCHANNEL][1]);
        sampleArray[RIGHTCHANNEL][2][cnt3] = largest(sampleArray[RIGHTCHANNEL][1]);
    }
    if (!cnt3)
    { // store avg from 64 * 8 * 8 * 8 samples in the array[3]
        sampleArray[LEFTCHANNEL][3][cnt4] = avg(sampleArray[LEFTCHANNEL][2]);
        sampleArray[RIGHTCHANNEL][3][cnt4] = avg(sampleArray[RIGHTCHANNEL][2]);
    }
    if (f_vu)
    {
        f_vu = false;
        m_vuLeft = avg(sampleArray[LEFTCHANNEL][3]);
        m_vuRight = avg(sampleArray[RIGHTCHANNEL][3]);
    }
    ++cnt1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t Audio::getVUlevel()
{
    // avg 0 ... 127
    if (!m_f_running)
        return 0;
    return (m_vuLeft << 8) + m_vuRight;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setTone(int8_t gainLowPass, int8_t gainBandPass, int8_t gainHighPass)
{
    // see https://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/
    // values can be between -40 ... +6 (dB)

    m_gain0 = gainLowPass;
    m_gain1 = gainBandPass;
    m_gain2 = gainHighPass;

    // gain, attenuation (set in digital filters)
    int db = max(m_gain0, max(m_gain1, m_gain2));
    m_corr = pow10f((float)db / 20);

    IIR_calculateCoefficients(m_gain0, m_gain1, m_gain2);

    /*
          This will cause a clicking sound when adjusting the EQ.
          Because when the EQ is adjusted, the IIR filter will be cleared and played,
          mixed in the audio data frame, and a click-like sound will be produced.
      */
    /*
      int16_t tmp[2]; tmp[0] = 0; tmp[1]= 0;

      IIR_filterChain0(tmp, true ); // flush the filter
      IIR_filterChain1(tmp, true ); // flush the filter
      IIR_filterChain2(tmp, true ); // flush the filter
      */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::forceMono(bool m)
{                      // #100 mono option
    m_f_forceMono = m; // false stereo, true mono
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setBalance(int8_t bal)
{ // bal -16...16
    if (bal < -16)
        bal = -16;
    if (bal > 16)
        bal = 16;
    m_balance = bal;

    computeLimit();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setVolume(uint8_t vol, uint8_t curve)
{ // curve 0: default, curve 1: flat at the beginning

    uint16_t v = ESP_ARDUINO_VERSION_MAJOR * 100 + ESP_ARDUINO_VERSION_MINOR * 10 + ESP_ARDUINO_VERSION_PATCH;

    if (vol > m_vol_steps)
        m_vol = m_vol_steps;
    else
        m_vol = vol;

    if (curve > 1)
        m_curve = 1;
    else
        m_curve = curve;

    computeLimit();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Audio::getVolume() { return m_vol; }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::computeLimit()
{                               // is calculated when the volume or balance changes
    double l = 1, r = 1, v = 1; // assume 100%

    /* balance is left -16...+16 right */
    if (m_balance > 0)
    {
        r -= (double)abs(m_balance) / 16;
    }
    else if (m_balance < 0)
    {
        l -= (double)abs(m_balance) / 16;
    }

    switch (m_curve)
    {
    case 0:
        v = (double)pow(m_vol, 2) / pow(m_vol_steps, 2); // square (default)
        break;
    case 1: // logarithmic
        double log1 = log(1);
        if (m_vol > 0)
        {
            v = m_vol * ((std::exp(log1 + (m_vol - 1) * (std::log(m_vol_steps) - log1) / (m_vol_steps - 1))) / m_vol_steps) / m_vol_steps;
        }
        else
        {
            v = 0;
        }
        break;
    }

    m_limit_left = l * v;
    m_limit_right = r * v;

    // log_e("m_limit_left %f,  m_limit_right %f ",m_limit_left, m_limit_right);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::Gain(int16_t *sample)
{
    sample[LEFTCHANNEL] *= m_limit_left;
    sample[RIGHTCHANNEL] *= m_limit_right;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::inBufferFilled()
{
    // current audio input buffer fillsize in bytes
    return InBuff.bufferFilled();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::inBufferFree()
{
    // current audio input buffer free space in bytes
    return InBuff.freeSpace();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::inBufferSize()
{
    // current audio input buffer size in bytes
    return InBuff.getBufsize();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//            ***     D i g i t a l   b i q u a d r a t i c     f i l t e r     ***
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::IIR_calculateCoefficients(int8_t G0, int8_t G1, int8_t G2)
{ // Infinite Impulse Response (IIR) filters

    // G1 - gain low shelf   set between -40 ... +6 dB
    // G2 - gain peakEQ      set between -40 ... +6 dB
    // G3 - gain high shelf  set between -40 ... +6 dB
    // https://www.earlevel.com/main/2012/11/26/biquad-c-source-code/

    if (getSampleRate() < 1000)
        return; // fuse

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (G0 < -40)
        G0 = -40; // -40dB -> Vin*0.01
    if (G0 > 6)
        G0 = 6; // +6dB -> Vin*2
    if (G1 < -40)
        G1 = -40;
    if (G1 > 6)
        G1 = 6;
    if (G2 < -40)
        G2 = -40;
    if (G2 > 6)
        G2 = 6;

    const float FcLS = 500;    // Frequency LowShelf[Hz]
    const float FcPKEQ = 3000; // Frequency PeakEQ[Hz]
    float FcHS = 6000;         // Frequency HighShelf[Hz]

    if (getSampleRate() < FcHS * 2 - 100)
    { // Prevent HighShelf filter from clogging
        FcHS = getSampleRate() / 2 - 100;
        // according to the sampling theorem, the sample rate must be at least 2 * 6000 >= 12000Hz for a filter
        // frequency of 6000Hz. If this is not the case, the filter frequency (plus a reserve of 100Hz) is lowered
    }
    float K, norm, Q, Fc, V;

    // LOWSHELF
    Fc = (float)FcLS / (float)getSampleRate(); // Cutoff frequency
    K = tanf((float)PI * Fc);
    V = powf(10, fabs(G0) / 20.0);

    if (G0 >= 0)
    { // boost
        norm = 1 / (1 + sqrtf(2) * K + K * K);
        m_filter[LOWSHELF].a0 = (1 + sqrtf(2 * V) * K + V * K * K) * norm;
        m_filter[LOWSHELF].a1 = 2 * (V * K * K - 1) * norm;
        m_filter[LOWSHELF].a2 = (1 - sqrtf(2 * V) * K + V * K * K) * norm;
        m_filter[LOWSHELF].b1 = 2 * (K * K - 1) * norm;
        m_filter[LOWSHELF].b2 = (1 - sqrtf(2) * K + K * K) * norm;
    }
    else
    { // cut
        norm = 1 / (1 + sqrtf(2 * V) * K + V * K * K);
        m_filter[LOWSHELF].a0 = (1 + sqrtf(2) * K + K * K) * norm;
        m_filter[LOWSHELF].a1 = 2 * (K * K - 1) * norm;
        m_filter[LOWSHELF].a2 = (1 - sqrtf(2) * K + K * K) * norm;
        m_filter[LOWSHELF].b1 = 2 * (V * K * K - 1) * norm;
        m_filter[LOWSHELF].b2 = (1 - sqrtf(2 * V) * K + V * K * K) * norm;
    }

    // PEAK EQ
    Fc = (float)FcPKEQ / (float)getSampleRate(); // Cutoff frequency
    K = tanf((float)PI * Fc);
    V = powf(10, fabs(G1) / 20.0);
    Q = 2.5; // Quality factor
    if (G1 >= 0)
    { // boost
        norm = 1 / (1 + 1 / Q * K + K * K);
        m_filter[PEAKEQ].a0 = (1 + V / Q * K + K * K) * norm;
        m_filter[PEAKEQ].a1 = 2 * (K * K - 1) * norm;
        m_filter[PEAKEQ].a2 = (1 - V / Q * K + K * K) * norm;
        m_filter[PEAKEQ].b1 = m_filter[PEAKEQ].a1;
        m_filter[PEAKEQ].b2 = (1 - 1 / Q * K + K * K) * norm;
    }
    else
    { // cut
        norm = 1 / (1 + V / Q * K + K * K);
        m_filter[PEAKEQ].a0 = (1 + 1 / Q * K + K * K) * norm;
        m_filter[PEAKEQ].a1 = 2 * (K * K - 1) * norm;
        m_filter[PEAKEQ].a2 = (1 - 1 / Q * K + K * K) * norm;
        m_filter[PEAKEQ].b1 = m_filter[PEAKEQ].a1;
        m_filter[PEAKEQ].b2 = (1 - V / Q * K + K * K) * norm;
    }

    // HIGHSHELF
    Fc = (float)FcHS / (float)getSampleRate(); // Cutoff frequency
    K = tanf((float)PI * Fc);
    V = powf(10, fabs(G2) / 20.0);
    if (G2 >= 0)
    { // boost
        norm = 1 / (1 + sqrtf(2) * K + K * K);
        m_filter[HIFGSHELF].a0 = (V + sqrtf(2 * V) * K + K * K) * norm;
        m_filter[HIFGSHELF].a1 = 2 * (K * K - V) * norm;
        m_filter[HIFGSHELF].a2 = (V - sqrtf(2 * V) * K + K * K) * norm;
        m_filter[HIFGSHELF].b1 = 2 * (K * K - 1) * norm;
        m_filter[HIFGSHELF].b2 = (1 - sqrtf(2) * K + K * K) * norm;
    }
    else
    {
        norm = 1 / (V + sqrtf(2 * V) * K + K * K);
        m_filter[HIFGSHELF].a0 = (1 + sqrtf(2) * K + K * K) * norm;
        m_filter[HIFGSHELF].a1 = 2 * (K * K - 1) * norm;
        m_filter[HIFGSHELF].a2 = (1 - sqrtf(2) * K + K * K) * norm;
        m_filter[HIFGSHELF].b1 = 2 * (K * K - V) * norm;
        m_filter[HIFGSHELF].b2 = (V - sqrtf(2 * V) * K + K * K) * norm;
    }

    //    log_e("LS a0=%f, a1=%f, a2=%f, b1=%f, b2=%f", m_filter[0].a0, m_filter[0].a1, m_filter[0].a2,
    //                                                  m_filter[0].b1, m_filter[0].b2);
    //    log_e("EQ a0=%f, a1=%f, a2=%f, b1=%f, b2=%f", m_filter[1].a0, m_filter[1].a1, m_filter[1].a2,
    //                                                  m_filter[1].b1, m_filter[1].b2);
    //    log_e("HS a0=%f, a1=%f, a2=%f, b1=%f, b2=%f", m_filter[2].a0, m_filter[2].a1, m_filter[2].a2,
    //                                                  m_filter[2].b1, m_filter[2].b2);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// clang-format off
// Infinite Impulse Response (IIR) filters
int16_t* Audio::IIR_filterChain0(int16_t iir_in[2], bool clear) 
{ 
    uint8_t z1 = 0, z2 = 1;
    enum : uint8_t { in = 0, out = 1 };
    float          inSample[2];
    float          outSample[2];

    if(clear) {
        memset(m_filterBuff, 0, sizeof(m_filterBuff)); // zero IIR filterbuffer
        iir_out[0] = 0;
        iir_out[1] = 0;
        iir_in[0] = 0;
        iir_in[1] = 0;
    }

    inSample[LEFTCHANNEL] = (float)(iir_in[LEFTCHANNEL]);
    inSample[RIGHTCHANNEL] = (float)(iir_in[RIGHTCHANNEL]);

    outSample[LEFTCHANNEL] =
        m_filter[0].a0 * inSample[LEFTCHANNEL] + m_filter[0].a1 * m_filterBuff[0][z1][in][LEFTCHANNEL] +
        m_filter[0].a2 * m_filterBuff[0][z2][in][LEFTCHANNEL] - m_filter[0].b1 * m_filterBuff[0][z1][out][LEFTCHANNEL] -
        m_filter[0].b2 * m_filterBuff[0][z2][out][LEFTCHANNEL];

    m_filterBuff[0][z2][in][LEFTCHANNEL] = m_filterBuff[0][z1][in][LEFTCHANNEL];
    m_filterBuff[0][z1][in][LEFTCHANNEL] = inSample[LEFTCHANNEL];
    m_filterBuff[0][z2][out][LEFTCHANNEL] = m_filterBuff[0][z1][out][LEFTCHANNEL];
    m_filterBuff[0][z1][out][LEFTCHANNEL] = outSample[LEFTCHANNEL];
    iir_out[LEFTCHANNEL] = (int16_t)outSample[LEFTCHANNEL];

    outSample[RIGHTCHANNEL] = m_filter[0].a0 * inSample[RIGHTCHANNEL] +
                              m_filter[0].a1 * m_filterBuff[0][z1][in][RIGHTCHANNEL] +
                              m_filter[0].a2 * m_filterBuff[0][z2][in][RIGHTCHANNEL] -
                              m_filter[0].b1 * m_filterBuff[0][z1][out][RIGHTCHANNEL] -
                              m_filter[0].b2 * m_filterBuff[0][z2][out][RIGHTCHANNEL];

    m_filterBuff[0][z2][in][RIGHTCHANNEL] = m_filterBuff[0][z1][in][RIGHTCHANNEL];
    m_filterBuff[0][z1][in][RIGHTCHANNEL] = inSample[RIGHTCHANNEL];
    m_filterBuff[0][z2][out][RIGHTCHANNEL] = m_filterBuff[0][z1][out][RIGHTCHANNEL];
    m_filterBuff[0][z1][out][RIGHTCHANNEL] = outSample[RIGHTCHANNEL];
    iir_out[RIGHTCHANNEL] = (int16_t)outSample[RIGHTCHANNEL];

    return iir_out;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Infinite Impulse Response (IIR) filters
int16_t* Audio::IIR_filterChain1(int16_t iir_in[2], bool clear) 
{ 
    uint8_t z1 = 0, z2 = 1;
    enum : uint8_t { in = 0, out = 1 };
    float          inSample[2];
    float          outSample[2];

    if(clear) {
        memset(m_filterBuff, 0, sizeof(m_filterBuff)); // zero IIR filterbuffer
        iir_out1[0] = 0;
        iir_out1[1] = 0;
        iir_in[0] = 0;
        iir_in[1] = 0;
    }

    inSample[LEFTCHANNEL] = (float)(iir_in[LEFTCHANNEL]);
    inSample[RIGHTCHANNEL] = (float)(iir_in[RIGHTCHANNEL]);

    outSample[LEFTCHANNEL] =
        m_filter[1].a0 * inSample[LEFTCHANNEL] + m_filter[1].a1 * m_filterBuff[1][z1][in][LEFTCHANNEL] +
        m_filter[1].a2 * m_filterBuff[1][z2][in][LEFTCHANNEL] - m_filter[1].b1 * m_filterBuff[1][z1][out][LEFTCHANNEL] -
        m_filter[1].b2 * m_filterBuff[1][z2][out][LEFTCHANNEL];

    m_filterBuff[1][z2][in][LEFTCHANNEL] = m_filterBuff[1][z1][in][LEFTCHANNEL];
    m_filterBuff[1][z1][in][LEFTCHANNEL] = inSample[LEFTCHANNEL];
    m_filterBuff[1][z2][out][LEFTCHANNEL] = m_filterBuff[1][z1][out][LEFTCHANNEL];
    m_filterBuff[1][z1][out][LEFTCHANNEL] = outSample[LEFTCHANNEL];
    iir_out1[LEFTCHANNEL] = (int16_t)outSample[LEFTCHANNEL];

    outSample[RIGHTCHANNEL] = m_filter[1].a0 * inSample[RIGHTCHANNEL] +
                              m_filter[1].a1 * m_filterBuff[1][z1][in][RIGHTCHANNEL] +
                              m_filter[1].a2 * m_filterBuff[1][z2][in][RIGHTCHANNEL] -
                              m_filter[1].b1 * m_filterBuff[1][z1][out][RIGHTCHANNEL] -
                              m_filter[1].b2 * m_filterBuff[1][z2][out][RIGHTCHANNEL];

    m_filterBuff[1][z2][in][RIGHTCHANNEL] = m_filterBuff[1][z1][in][RIGHTCHANNEL];
    m_filterBuff[1][z1][in][RIGHTCHANNEL] = inSample[RIGHTCHANNEL];
    m_filterBuff[1][z2][out][RIGHTCHANNEL] = m_filterBuff[1][z1][out][RIGHTCHANNEL];
    m_filterBuff[1][z1][out][RIGHTCHANNEL] = outSample[RIGHTCHANNEL];
    iir_out1[RIGHTCHANNEL] = (int16_t)outSample[RIGHTCHANNEL];

    return iir_out1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Infinite Impulse Response (IIR) filters
int16_t* Audio::IIR_filterChain2(int16_t iir_in[2], bool clear) 
{ 
    uint8_t z1 = 0, z2 = 1;
    enum : uint8_t { in = 0, out = 1 };
    float          inSample[2];
    float          outSample[2];

    if(clear) {
        memset(m_filterBuff, 0, sizeof(m_filterBuff)); // zero IIR filterbuffer
        iir_out2[0] = 0;
        iir_out2[1] = 0;
        iir_in[0] = 0;
        iir_in[1] = 0;
    }

    inSample[LEFTCHANNEL] = (float)(iir_in[LEFTCHANNEL]);
    inSample[RIGHTCHANNEL] = (float)(iir_in[RIGHTCHANNEL]);

    outSample[LEFTCHANNEL] =
        m_filter[2].a0 * inSample[LEFTCHANNEL] + m_filter[2].a1 * m_filterBuff[2][z1][in][LEFTCHANNEL] +
        m_filter[2].a2 * m_filterBuff[2][z2][in][LEFTCHANNEL] - m_filter[2].b1 * m_filterBuff[2][z1][out][LEFTCHANNEL] -
        m_filter[2].b2 * m_filterBuff[2][z2][out][LEFTCHANNEL];

    m_filterBuff[2][z2][in][LEFTCHANNEL] = m_filterBuff[2][z1][in][LEFTCHANNEL];
    m_filterBuff[2][z1][in][LEFTCHANNEL] = inSample[LEFTCHANNEL];
    m_filterBuff[2][z2][out][LEFTCHANNEL] = m_filterBuff[2][z1][out][LEFTCHANNEL];
    m_filterBuff[2][z1][out][LEFTCHANNEL] = outSample[LEFTCHANNEL];
    iir_out2[LEFTCHANNEL] = (int16_t)outSample[LEFTCHANNEL];

    outSample[RIGHTCHANNEL] = m_filter[2].a0 * inSample[RIGHTCHANNEL] +
                              m_filter[2].a1 * m_filterBuff[2][z1][in][RIGHTCHANNEL] +
                              m_filter[2].a2 * m_filterBuff[2][z2][in][RIGHTCHANNEL] -
                              m_filter[2].b1 * m_filterBuff[2][z1][out][RIGHTCHANNEL] -
                              m_filter[2].b2 * m_filterBuff[2][z2][out][RIGHTCHANNEL];

    m_filterBuff[2][z2][in][RIGHTCHANNEL] = m_filterBuff[2][z1][in][RIGHTCHANNEL];
    m_filterBuff[2][z1][in][RIGHTCHANNEL] = inSample[RIGHTCHANNEL];
    m_filterBuff[2][z2][out][RIGHTCHANNEL] = m_filterBuff[2][z1][out][RIGHTCHANNEL];
    m_filterBuff[2][z1][out][RIGHTCHANNEL] = outSample[RIGHTCHANNEL];
    iir_out2[RIGHTCHANNEL] = (int16_t)outSample[RIGHTCHANNEL];

    return iir_out2;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::readID3V1Tag()
{
    // this is an V1.x id3tag after an audio block, ID3 v1 tags are ASCII
    // Version 1.x is a fixed size at the end of the file (128 bytes) after a <TAG> keyword.

    if (InBuff.bufferFilled() == 128 && startsWith((const char *)InBuff.getReadPtr(), "TAG"))
        return true;
    
    if (InBuff.bufferFilled() == 227 && startsWith((const char *)InBuff.getReadPtr(), "TAG+"))
        return true;

    return false;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int32_t Audio::mp3_correctResumeFilePos(uint32_t resumeFilePos)
{
    /* this checks that the first 22 bits of the next frame header are the same as the current frame header, but it's still not foolproof
     * (could accidentally find a sequence in the bitstream which appears to match but is not actually the next frame header)
     * this could be made more error-resilient by checking several frames in a row and verifying that nSlots is the same in each case
     */
    boolean found = false;
    uint32_t pos = resumeFilePos;
    uint32_t pos1 = 0, pos2 = 0;
    uint32_t maxPos = m_audioDataStart + m_audioDataSize;
    uint8_t byte1a, byte2a, byte3a;
    uint8_t byte1b, byte2b, byte3b;

    if (pos + 3 >= maxPos)
        goto exit;
    if (pos < m_audioDataStart)
        pos = m_audioDataStart;

    _f_mgr->seekPos(audiofile, pos);

    while (!found)
    {
        if (pos + 3 >= maxPos)
            goto exit;
        _f_mgr->readFromFile(audiofile, &byte1a);
        ++pos;
        _f_mgr->readFromFile(audiofile, &byte2a);
        ++pos;
        while (true)
        {
            if (byte1a == 0xFF && (byte2a & 0x0E0) == 0xE0)
            {
                _f_mgr->readFromFile(audiofile, &byte3a);
                ++pos;
                pos1 = pos - 3;
                break;
            }
            byte1a = byte2a;
            _f_mgr->readFromFile(audiofile, &byte2a);
            ++pos;
            if (pos >= maxPos)
                goto exit;
        }
        // log_e("pos1 %i, byte1a %X, byte2a %X, byte3a %X", pos1, byte1a, byte2a, byte3a);

        if (pos + 3 >= maxPos)
            goto exit;
        _f_mgr->readFromFile(audiofile, &byte1b);
        ++pos;
        _f_mgr->readFromFile(audiofile, &byte2b);
        ++pos;
        while (true)
        {
            if (byte1b == 0xFF && (byte2b & 0x0E0) == 0xE0)
            {
                _f_mgr->readFromFile(audiofile, &byte3b);
                ++pos;
                pos2 = pos - 3;
                break;
            }
            byte1b = byte2b;
            _f_mgr->readFromFile(audiofile, &byte2b);
            ++pos;
            if (pos >= maxPos)
                goto exit;
        }

        if ((byte1a == byte1b) && (byte2a == byte2b) && ((byte3a & 0xFC) == (byte3b & 0xFC)))
        {
            if (byte1a == 0xFF && byte2a == 0xFF && byte3a == 0xFF)
                found = false; // 0xFFFFFF can't be
            else
                found = true;
        }
    }
    (void)pos2;
    // log_e("found pos1=%i, pos2=%i", pos1, pos2);
    if (found)
        return (pos1);

exit:
    return -1;
}