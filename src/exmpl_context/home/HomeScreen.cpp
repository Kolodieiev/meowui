#include "HomeContext.h"
//

#include "../resources/ico/battery.h"
#include "./res/wall.h"
//
#include "../WidgetCreator.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
//
#include "meow/util/img/BmpUtil.h"

HomeContext::HomeContext()
{
    WidgetCreator creator;

    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    Image *wallpp_img = new Image(ID_WALLPAPER);

    BmpUtil util;
    ImgData bmp = util.loadBmp("/wall/wallpaper.bmp");

    if (bmp.width > 0)
    {
        _wallpaper_ptr = bmp.data_ptr;
        wallpp_img->init(bmp.width, bmp.height);
        wallpp_img->setSrc(_wallpaper_ptr);
    }
    else
    {
        wallpp_img->init(D_WIDTH, D_HEIGHT);
        wallpp_img->setSrc(DEFAULT_WALL);
    }

    layout->addWidget(wallpp_img);

    _bat_cap_lbl = new Label(ID_BAT_LVL);
    layout->addWidget(_bat_cap_lbl);
    _bat_cap_lbl->setText(EMPTY_BAT);
    _bat_cap_lbl->setWidth(56);
    _bat_cap_lbl->setHeight(35);
    _bat_cap_lbl->setAlign(Label::ALIGN_CENTER);
    _bat_cap_lbl->setGravity(Label::GRAVITY_CENTER);
    _bat_cap_lbl->setPos(_display.width() - TFT_CUTOUT - _bat_cap_lbl->getWidth(), DISPLAY_PADDING);
    _bat_cap_lbl->setTransparency(true);

    _bat_ico = new Image(1);
    _bat_cap_lbl->setBackImg(_bat_ico);
    _bat_ico->init(_bat_cap_lbl->getWidth(), _bat_cap_lbl->getHeight());
    _bat_ico->setSrc(ICO_BATTERY);
    _bat_ico->setTransparency(true);
    _bat_ico->setTranspColor(TFT_TRANSPARENT);

    updateBatCap();
}

HomeContext::~HomeContext()
{
    delete _bat_ico;
    free(_wallpaper_ptr);
}

bool HomeContext::loop()
{
    return true;
}

void HomeContext::update()
{
    if (_input.isReleased(BtnID::BTN_D))
    {
        _input.lock(BtnID::BTN_D, 500);
        openContextByID(ID_CONTEXT_MENU);
    }

    if (millis() - _upd_timer > UPD_DISPLAY_INTERVAL)
    {
        _upd_timer = millis();
        updateBatCap();
    }
}

void HomeContext::updateBatCap()
{
    //  float DIV_K{0.5827};
    //  const uint8_t READ_CYCLES_NUMBER = 128;

    // float bat_voltage{0.0f};

    // for (uint8_t i{0}; i < READ_CYCLES_NUMBER; ++i)
    //     bat_voltage += analogRead(PIN_VOLT_MEASH);

    // bat_voltage /= READ_CYCLES_NUMBER;
    // bat_voltage *= 3.3;
    // bat_voltage /= 4095;
    // bat_voltage /= DIV_K;

    // String volt_str = String(bat_voltage);
    // _bat_cap_lbl->setText(volt_str);
}
