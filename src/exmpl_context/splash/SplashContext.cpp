#include "SplashContext.h"

#include "meow/manager/settings/SettingsManager.h"
#include "meow/manager/spi/SPI_Manager.h"
#include "meow/manager/sd/SD_Manager.h"

#include "../WidgetCreator.h"
#include "meow/ui/widget/image/Image.h"

#include "../resources/const.h"
#include "../resources/color.h"
#include "../resources/string.h"

#include "meowui_setup/sd_setup.h"

#define LBL_X_OFFSET 70
#define SLEEP_PIN 46

SplashContext::SplashContext()
{
    pinMode(SLEEP_PIN, OUTPUT);
    digitalWrite(SLEEP_PIN, HIGH);
    pinMode(SD_PIN_CS, OUTPUT);
    digitalWrite(SD_PIN_CS, HIGH);
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);

    _start_time = millis();

    WidgetCreator creator;
    //
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    //
    uint16_t y_pos{TFT_CUTOUT};

    SD_Manager &sd = SD_Manager::getInst();

    SPI_Manager spi_mngr;
    spi_mngr.initBus(SD_SPI_BUS, SD_PIN_SCLK, SD_PIN_MISO, SD_PIN_MOSI);

    sd.setup(SD_PIN_CS, spi_mngr.getSpi4Bus(SD_SPI_BUS), SD_FREQUENCY, SD_MOUNTPOINT, SD_MAX_FILES);
    if (sd.mount())
        addLabel(DISPLAY_PADDING, y_pos, STR_SUCCSESS, TFT_GREEN);
    else
        addLabel(DISPLAY_PADDING, y_pos, STR_FAIL, TFT_RED);

    addLabel(LBL_X_OFFSET, y_pos, STR_SD_LBL, TFT_WHITE);

    y_pos += chr_hgt_font2 + 3;

    // check psram
    bool psram_ok = psramInit();
    if (psram_ok)
        addLabel(DISPLAY_PADDING, y_pos, STR_SUCCSESS, TFT_GREEN);
    else
        addLabel(DISPLAY_PADDING, y_pos, STR_FAIL, TFT_RED);

    addLabel(LBL_X_OFFSET, y_pos, STR_PSRAM_LBL, TFT_WHITE);
}

bool SplashContext::loop()
{
    return true;
}

void SplashContext::update()
{
    if (millis() - _start_time > SHOWING_INIT_TIME)
    {
        openContextByID(ContextID::ID_CONTEXT_HOME);
    }
}

void SplashContext::addLabel(uint16_t x_pos, uint16_t y_pos, const char *res_str, uint16_t color)
{
    EmptyLayout *layout = getLayout()->castTo<EmptyLayout>();

    Label *lbl = new Label(_widget_id);
    layout->addWidget(lbl);
    lbl->setText(res_str);
    lbl->setBackColor(COLOR_MAIN_BACK);
    lbl->setTextColor(color);
    lbl->setPos(x_pos, y_pos);
    lbl->initWidthToFit();

    ++_widget_id;
}
