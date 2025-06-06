#include "CalcContext.h"

#include "../WidgetCreator.h"

#include "../resources/string.h"
#include "../resources/color.h"
#include "../resources/const.h"

CalcContext::CalcContext()
{
    WidgetCreator creator;

    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
}

bool CalcContext::loop()
{
    return true;
}

void CalcContext::update()
{

}
