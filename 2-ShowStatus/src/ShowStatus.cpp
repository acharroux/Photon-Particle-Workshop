/*
 * Project ShowStatus
 * Description: Display various status with SSD1306
 * Author: Alain Charroux
 * Date: 24/04/2019
 */

#include "application.h"
#include "Adafruit_SSD1306/Adafruit_SSD1306.h"

#include "WifiPanel.h"
#include "SoftwarePanel.h"

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// I2C address used by our hardware
#define OLED_I2C_ADDRESS 0x3C

// not used but looks like it still has to be provided to adafruit lib
// Default is probably D4 but we plan to use this pin for other purpose
#define OLED_RESET D3

static Adafruit_SSD1306 gDisplay(OLED_RESET);

// a little bit risky to be confident on order of init of global variables

/** this will display infos about the wifi connection */
static WifiPanel gWifiPanel(gDisplay);

/** this will display infos about the current firmware and internal state */
static SoftwarePanel gSoftwarePanel(gDisplay);

/** for simplicity we will manage the display of the current panel via a naive pointer  */
static Panel *gCurrentPanel = NULL;

/****************************************************
 * one simple function to force the current panel
 * for demonstration purpose, forcing the panel
 * will be signaled by an event */

bool forcePanel(String iPanelName)
{
  bool lKnownPanel = false;
  // not very efficient char to string conversion but much clearer
  if (iPanelName == "Wifi")
  {
    lKnownPanel = true;
    gCurrentPanel = &gWifiPanel;
  }

  if (iPanelName == "Software")
  {
    lKnownPanel = true;
    gCurrentPanel = &gSoftwarePanel;
  }

  if (lKnownPanel)
  {
    gCurrentPanel->refreshValues();
    gCurrentPanel->display();
    Particle.publish("PanelRequested", iPanelName, PRIVATE);
    return true;
  }
  else
  {
    // unknown panel
    Particle.publish("UnknownPanelRequested", iPanelName, PRIVATE);
    return false;
  }
}
/** 
 * various event handlers 
 * subscription will be done to MY_DEVICES : only events explicitely sent to my devices
 * will be catched
*/

/****************************************************************/
/**
 * force the display of the Wifi panel 
 * no data provided by the event
*/

void DisplayWifiEventHandler(const char *event, const char *data)
{
  forcePanel("Wifi");
}

/****************************************************************/
/**
 * force the display of the Software panel 
 * no data provided by the event
*/

void DisplaySoftwareEventHandler(const char *event, const char *data)
{
  forcePanel("Software");
}

/****************************************************************/
/* change the display of the Wifi panel 
   data provides the name of the panel to display
*/

void ChangePanelEventHandler(const char *event, const char *data)
{
  forcePanel(data);
}

/**
 * declare functions. 
 * functions are quite similar to events except it can be called only for one photon at a time
 * events can target all devices in the world (!), or all of your devices or a specific device
 */

int ChangePanelFunction(String iPanelName)
{
  return static_cast<bool>(forcePanel(iPanelName));
}

void setup()
{
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  gDisplay.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS); // initialize with the I2C addr  (for the 128x64)
  gWifiPanel.initAfterConnect();
  gSoftwarePanel.initAfterConnect();

  /* now we register the events that will switch between the 2 panels */

  Particle.subscribe("DisplaySoftwarePanel", DisplaySoftwareEventHandler, MY_DEVICES);
  Particle.subscribe("DisplayWifiPanel", DisplayWifiEventHandler, MY_DEVICES);
  Particle.subscribe("ChangePanel", ChangePanelEventHandler, MY_DEVICES);

  /* now we register the function that will force a panel to be displayed */
  Particle.function("ChangePanel", ChangePanelFunction);

  // start with the software panel
  forcePanel("Software");
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{
  gCurrentPanel->refreshValues();
  gCurrentPanel->display();
  delay(1000);
}