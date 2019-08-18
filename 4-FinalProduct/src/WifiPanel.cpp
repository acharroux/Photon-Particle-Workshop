#include "application.h"
#include "Adafruit_SSD1306/Adafruit_SSD1306.h"
#include "Panel.h"
#include "WifiPanel.h"

/*******************************************/

WifiPanel::WifiPanel(Adafruit_SSD1306 &iDisplay) : Panel(iDisplay), mSignalStrength(0.0), mSignalQuality(0.0)
{
}

/*******************************************/
/**
 *  fetch all interesting parameters of the current WiFi connection
 *  return true if parameters has changed (only changes of signalStrength & signalQuality are tracked at this time)
 **/

bool WifiPanel::refreshValues(void)
{
    bool lHasChanged = false;

    if (WiFi.ready())
    {
        mSSID = WiFi.SSID();

        byte lMacAddress[6] = {0};
        WiFi.macAddress(lMacAddress);
        byte *b = lMacAddress;
        mMacAddress = String::format("%X:%X:%X:%X:%X:%X", *(b++), *(b++), *(b++), *(b++), *(b++), *(b++));

        mIPAddress = WiFi.localIP().toString();

        mHostName = WiFi.hostname();

        WiFiSignal lSignal = WiFi.RSSI();

        if (lSignal.getStrength() != mSignalStrength)
            lHasChanged = true;
        if (lSignal.getQuality() != mSignalQuality)
            lHasChanged = true;
        mSignalStrength = lSignal.getStrength();
        mSignalQuality = lSignal.getQuality();
    }
    else
    {
        mSSID = "";
        mMacAddress = "";
        mIPAddress = "";
        mHostName = "";
        if (mSignalStrength != 0.0)
            lHasChanged = true;
        if (mSignalQuality != 0.0)
            lHasChanged = true;
        mSignalQuality = 0.0;
        mSignalStrength = 0.0;
    }
    return lHasChanged;
}

/** a bunch of accessors **/

/*******************************************/

const String &WifiPanel::getSSID(void) const
{
    return mSSID;
}

/*******************************************/

const String &WifiPanel::getMacAddress(void) const
{
    return mMacAddress;
}

/*******************************************/

const String &WifiPanel::getIPAddress(void) const
{
    return mIPAddress;
}

/*******************************************/

const String &WifiPanel::getHostName(void) const
{
    return mHostName;
}

/*******************************************/

float WifiPanel::getSignalStrength(void) const
{
    return mSignalStrength;
}

/*******************************************/

float WifiPanel::getSignalQuality(void) const
{
    return mSignalQuality;
}

/*******************************************/

void WifiPanel::display(void)
{
    resetDisplay();

    setFont(LargeFont);
    getDisplay().printf("WIFI");
    newLine();

    setFont(SmallFont);
    getDisplay().printf("SSID:%s", getSSID().c_str());
    newLine();
    getDisplay().printf("Strength:%.02f%%", getSignalStrength());
    newLine();
    getDisplay().printf("Quality:%.02f%%", getSignalQuality());

    setVPos(getVPos() + 3);
    newLine();
    getDisplay().printf("IP:%s", getIPAddress().c_str());
    newLine();
    getDisplay().printf("MAC:%s", getMacAddress().c_str());

    getDisplay().display();
}
