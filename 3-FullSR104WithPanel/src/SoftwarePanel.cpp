#include "application.h"
#include "Adafruit_SSD1306/Adafruit_SSD1306.h"

#include "SoftwareState.h"
#include "SoftwarePanel.h"

/******************************************/

SoftwarePanel::SoftwarePanel(Adafruit_SSD1306 &iDisplay, SoftwareState &iSoftwareState) : Panel(iDisplay), mSoftwareState(iSoftwareState)
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
    getDisplay().printf("Uptime: %s", uptime().c_str());
    newLine();
    getDisplay().printf("In:%04lu Out:%04lu", mSoftwareState.getNbReceivedEvents(), mSoftwareState.getNbSentEvents());
    newLine();
    getDisplay().printf("Range:%03.2f (%d:%d)", mSoftwareState.getRange(), mSoftwareState.getNbValidRanges(), mSoftwareState.getNbCurrentValids());

    getDisplay().display();
}

/******************************************/

String SoftwarePanel::JSONDump(const char *data) const
{
    // there is cool JSON library somewhere
    // too lazy to find it
    return String::format("{ \"Name\: \"%s\", \"uptime\": \"%s\", \"NbRanges\": %lu, \"NbValidRanges\":%lu }",mDeviceName.c_str(),uptime().c_str(),mSoftwareState.getNbRanges(),mSoftwareState.getNbValidRanges());
}

/******************************************/

void SoftwarePanel::captureDeviceInfos(const char *topic, const char *data)
{
    if (String(topic) == "particle/device/name")
    {
        mDeviceName = data;
    }
    if (String(topic) == "particle/device/ip")
    {
        mPublicIP = data;
    }
}


/******************************************/

String SoftwarePanel::uptime(void) const
{
    unsigned lUpTime = System.uptime();
    return String::format("%02d:%02d:%02d", (lUpTime % (24 * 3600)) / 3600, (lUpTime % 3600) / 60, lUpTime % 60);
}
