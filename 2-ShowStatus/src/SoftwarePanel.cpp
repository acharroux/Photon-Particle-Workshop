#include "application.h"
#include "Adafruit_SSD1306/Adafruit_SSD1306.h"

#include "SoftwarePanel.h"

/******************************************/

SoftwarePanel::SoftwarePanel(Adafruit_SSD1306 &iDisplay) : Panel(iDisplay), mNbSent(0), mNbReceived(0)
{
}

/******************************************/

void SoftwarePanel::initAfterConnect(void)
{
    if (Particle.connected())
    {
        Particle.subscribe("particle/device", &SoftwarePanel::captureDeviceInfos, this, System.deviceID());
        delay(1000);
        Particle.publish("particle/device/name", PRIVATE);
        delay(1000);
        Particle.publish("particle/device/ip", PRIVATE);
    }
}

/******************************************/

void SoftwarePanel::display(void)
{
    resetDisplay();

    setFont(LargeFont);
    getDisplay().printf("Software");
    newLine();

    setFont(SmallFont);
    getDisplay().printf("Name:%s", mDeviceName.c_str());
    newLine();
    getDisplay().printf("PubIP:%s", mPublicIP.c_str());
    newLine();
    getDisplay().printf("Out:%04d In:%04d", mNbSent, mNbReceived);
    newLine();
    unsigned lUpTime = System.uptime();
    getDisplay().printf("Uptime: %02d:%02d:%02d", (lUpTime % 60 % (24 * 3600)) / 3600, (lUpTime % 3600) / 60, lUpTime % 60);

    getDisplay().display();
}

/******************************************/

void SoftwarePanel::captureDeviceInfos(const char *topic, const char *data)
{
    if (String(topic) == "particle/device/name")
    {
        mDeviceName = data;
        mNbReceived++;
    }
    if (String(topic) == "particle/device/ip")
    {
        mPublicIP = data;
        mNbReceived++;
    }
}
