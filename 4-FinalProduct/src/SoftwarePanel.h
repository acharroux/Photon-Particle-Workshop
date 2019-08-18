#ifndef _SOFTWARE_PANEL_
#define _SOFTWARE_PANEL_

#include "application.h"
#include "Panel.h"

class SoftwareState;

class SoftwarePanel : public Panel
{
public:
    SoftwarePanel(Adafruit_SSD1306 &iDisplay,SoftwareState &iSoftwareState);

    /** some infos will be valid only after cloud connection
     * so we need to provide ou own implementation of initAfterConnect 
     */

    void initAfterConnect(void);

    /** dump all content in a 128x64 oled display */
    void display();

    // datat is unused at this time
    String JSONDump(const char *data) const;

private:
    SoftwareState& mSoftwareState;
    String mPublicIP;
    String mDeviceName;

    void captureDeviceInfos(const char *topic, const char *data);
    String uptime(void) const;

    SoftwarePanel(void);
};

#endif // _SOFTWARE_PANEL_
