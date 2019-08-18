#ifndef _SOFTWARE_PANEL_
#define _SOFTWARE_PANEL_

#include "application.h"
#include "Panel.h"

class SoftwarePanel : public Panel
{
public:
    SoftwarePanel(Adafruit_SSD1306 &iDisplay);

    /** some infos will be valid only after cloud connection
     * so we need to provide ou own implementation of initAfterConnect 
     */

    void initAfterConnect(void);

    /** dump all content in a 128x64 oled display */
    void display();

private:
    String mPublicIP;
    String mDeviceName;
    int mNbSent;
    int mNbReceived;

    void captureDeviceInfos(const char *topic, const char *data);

    SoftwarePanel(void);
};

#endif // _SOFTWARE_PANEL_
