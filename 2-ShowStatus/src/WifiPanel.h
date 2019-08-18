#ifndef _WIFI_PANEL_
#define _WIFI_PANEL_

#include "application.h"
#include "Panel.h"

class WifiPanel : public Panel
{
public:
    WifiPanel(Adafruit_SSD1306 &iDisplay);

    bool refreshValues(void);

    void display(void);

private:
    String mSSID;
    String mMacAddress;
    String mIPAddress;
    String mHostName;
    float mSignalStrength;
    float mSignalQuality;

    /** a bunch of accessors **/
    const String &getSSID(void) const;
    const String &getMacAddress(void) const;
    const String &getIPAddress(void) const;
    const String &getHostName(void) const;
    float getSignalStrength(void) const;
    float getSignalQuality(void) const;

    WifiPanel(void);
};

#endif // _WIFI_PANEL_
