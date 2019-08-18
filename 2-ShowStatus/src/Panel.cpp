#include "Adafruit_SSD1306/Adafruit_SSD1306.h"
#include "Panel.h"

/*******************************************/
/*******************************************/
/*******************************************/

Panel::Panel(Adafruit_SSD1306 &iDisplay) : mDisplay(iDisplay), mVPos(0), mFont(SmallFont)
{
}

/*******************************************/

Panel::~Panel(void)
{
}

/*******************************************/

void Panel::resetDisplay(void)
{
    getDisplay().clearDisplay();
    mDisplay.setTextColor(WHITE);
    setVPos(0);
}

/*******************************************/

void Panel::setFont(eFont iFontSize)
{
    switch (iFontSize)
    {
    case LargeFont:
        mFont = LargeFont;
        break;

    default:
        mFont = SmallFont;
        break;
    }
    mDisplay.setTextSize(mFont);
}

/*******************************************/

Panel::eFont Panel::getFont(void) const
{
    return mFont;
}

/*******************************************/

void Panel::setVPos(int iVPos)
{
    mVPos = iVPos;
    getDisplay().setCursor(0, getVPos());
}

/*******************************************/

int Panel::getVPos(void) const
{
    return mVPos;
}

/*******************************************/

int Panel::getFontSize(void) const
{
    switch (getFont())
    {
    case LargeFont:
        return 17 + 1;

    default:
        return 8 + 1;
    }
}

/*******************************************/

void Panel::newLine(void)
{
    setVPos(getVPos() + getFontSize());
}

/*******************************************/

Adafruit_SSD1306 &Panel::getDisplay(void) const
{
    return mDisplay;
}

/*******************************************/

void Panel::initAfterConnect(void)
{
}

/*******************************************/

bool Panel::refreshValues(void)
{
    return true;
}
