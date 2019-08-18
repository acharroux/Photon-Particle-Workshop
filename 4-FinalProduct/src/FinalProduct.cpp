/*
 * Project ShowStatus
 * Description: Manage a sonar and a display panel. Final product
 * Author: Alain Charroux
 * Date: 24/04/2019
 */

#include "application.h"

#include "Adafruit_SSD1306/Adafruit_SSD1306.h"
#include "HC_SR04.h"

#include "SoftwareState.h"
#include "WifiPanel.h"
#include "SoftwarePanel.h"

/*****************************************************************
 * Product identification
 * these are the only changes to do to activate the product management
 */

// The product id as given by Particle's admin console
// It won't change for the whole lifetime of the product
PRODUCT_ID(8925);

// the product version. It has to be incremented each time a
// new reference version is deployed via Particle's admin console
PRODUCT_VERSION(1);

/*************************************************************************************************
           HARDWARE SECTION
 ************************************************************************************************/

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// I2C address used by our hardware
#define OLED_I2C_ADDRESS 0x3C

// not used but looks like it still has to be provided to adafruit lib
// Default is probably D4 but we plan to use this pin for other purpose
#define OLED_RESET D3

static Adafruit_SSD1306 gDisplay(OLED_RESET);

#define TRIG_PIN D4
#define ECHO_PIN D5

// Our hardware : only a basic HC-SR04
static HC_SR04 gSonar = HC_SR04(TRIG_PIN, ECHO_PIN);

/*************************************************************************************************
 * SOFTWARE STATUS SECTION
 ************************************************************************************************/

static SoftwareState gSoftwareState;

/*************************************************************************************************
 * PANELS SECTION
 ************************************************************************************************/

// a little bit risky to be confident on order of init of global variables
// but so easy ...

/** this will display infos about the wifi connection */
static WifiPanel gWifiPanel(gDisplay);

/** this will display infos about the current firmware and internal state */
static SoftwarePanel gSoftwarePanel(gDisplay, gSoftwareState);

/** for simplicity we will manage the display of the current panel via a naive pointer  */
static Panel *gCurrentPanel = NULL;

/*************************************************************************************************
           CLOUD SECTION
 ************************************************************************************************/

/****************************************************
 * one simple function to force the current panel
 * for demonstration purpose, forcing the panel
 * will be signaled by an event */

static bool forcePanel(String iPanelName)
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
    // for demonstration purpose, we try to track every activity related to cloud
    gSoftwareState.trackSentEvent();
    return true;
  }
  else
  {
    // unknown panel
    Particle.publish("UnknownPanelRequested", iPanelName, PRIVATE);
    // for demonstration purpose, we try to track every activity related to cloud
    gSoftwareState.trackSentEvent();
    return false;
  }
}

// the led pattern we will use to signal a valid range

static LEDStatus gValidRangeStatus;

/** 
 * be very vocal about the provided probe
**/

static void signalValidRange(double iRange)
{
  /** 1 flash the system led with our pattern */
  gValidRangeStatus.setActive(true);
  /** 2 publish our event "InterestingRange"  **/
  Particle.publish("ValidRange", String::format("%.2f", iRange), PRIVATE);
  // for demonstration purpose, we try to track every activity related to cloud
  gSoftwareState.trackSentEvent();
  delay(200);
  /** 3 get back to standard system led pattern **/
  gValidRangeStatus.setActive(false);
}

/** 
 * various event handlers 
 * subscription will be done to MY_DEVICES : only events explicitely sent to my devices
 * will be catched
*/

/****************************************************************
* send infos about us to the cloud
*/

static void WhoIsConnectedEventHandler(const char *event, const char *data)
{
  // for demonstration purpose, we try to track every activity related to cloud
  gSoftwareState.trackEventReceived();
  Particle.publish("Connected", gSoftwarePanel.JSONDump(data));
}

/****************************************************************/
/* change the display of the Wifi panel 
   data provides the name of the panel to display
*/

static void ChangePanelEventHandler(const char *event, const char *data)
{
  // for demonstration purpose, we try to track every activity related to cloud
  gSoftwareState.trackEventReceived();
  forcePanel(data);
}

/****************************************************************/

static void ForceOTAEventHandler(const char *event, const char *data)
{
  // for demonstration purpose, we try to track every activity related to cloud
  gSoftwareState.trackEventReceived();
  // obviously a true product would make much more checks before accepting to reboot like this
  // only for demonstration purpose !!
  Particle.publish("OTARequested", "", PRIVATE);
  System.reset();
}

/**
 * declare cloud functions. 
 * functions are quite similar to events except it can be called only for one photon at a time
 * events can target all devices in the world (!), or all of your devices or a specific device
 */

static int ChangePanelFunction(String iPanelName)
{
  return static_cast<bool>(forcePanel(iPanelName));
}

void setup()
{
  // hardware setup
  // Panels
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  gDisplay.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS); // initialize with the I2C addr  (for the 128x64)

  // there are some init that have to be done AFTER connection
  // and setup is called after connection

  gWifiPanel.initAfterConnect();
  gSoftwarePanel.initAfterConnect();

  // SR104 object has been already setup by ts constructor
  // but we need to setup a little bit the status led used by our usage of SR104
  // our 'interesting range' LEDStatus needs to be tuned before actual usage
  // Orange, blinking fast
  gValidRangeStatus.setColor(RGB_COLOR_RED);
  gValidRangeStatus.setPattern(LED_PATTERN_BLINK);
  gValidRangeStatus.setSpeed(LED_SPEED_FAST);
  gValidRangeStatus.setPeriod(50);

  // Cloud registrations

  // publish the current range
  // From now, the last range found by gSonar is permanently accessible via the cloud variable named "cm"
  Particle.variable("cm", gSoftwareState.getRangeVariable());

  // register the event that will switch between the 2 panels
  Particle.subscribe("ChangePanel", ChangePanelEventHandler, MY_DEVICES);

  // register an event to see who is connected
  Particle.subscribe("WhoIsConnected", WhoIsConnectedEventHandler, MY_DEVICES);

  /* register the function that will force a panel to be displayed */
  Particle.function("ChangePanel", ChangePanelFunction);

  // automatic product OTA is done each time a big cloud handshake is done
  // such handshake is done on various triggers. Amongst them, there are
  // -> power off/on
  // -> a call to System.reset
  // -> every XXX hours (XXXdepending on platform and other obscure conditions)
  //
  // The last one is the smartest one : it will retain current permanent variables
  // but it is defintely the least immediate.
  /// We will force a rest when requested via event ForceOTA

  // Note there is a new feature in beta that will trigger 'immediate' product OTA

  // register the event that will force an OTA on all devices
  Particle.subscribe("ForceOTA", ForceOTAEventHandler, MY_DEVICES);

  // start with the software panel
  forcePanel("Software");
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{
  double lRange = gSonar.getDistanceCM();

  gSoftwareState.setRange(lRange);

  if (gSoftwareState.rangeIsValidForALongTime())
  {
    signalValidRange(gSoftwareState.getRange());
  }
  gCurrentPanel->refreshValues();
  gCurrentPanel->display();
  delay(100);
}