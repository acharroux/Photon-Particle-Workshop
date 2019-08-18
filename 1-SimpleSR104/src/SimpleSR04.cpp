/*
 * Project SimpleSR04
 * Description: Simple work with HC-SR04
 * Author: Alain Charroux
 * Date: 22 April 2019
 */

#include "application.h"
#include "HC_SR04.h"

// Our hardware configuration

#define TRIG_PIN D4
#define ECHO_PIN D5

// Our hardware : only a basic HC-SR04
static HC_SR04 gSonar = HC_SR04(TRIG_PIN, ECHO_PIN);

// Our published state ie the last range
static double gCentimeters = 0.0;

// the led pattern we will use to signal a valid range

static LEDStatus gValidRangeStatus;

// keep track of things even if not visible anywhere at this time

static unsigned long gNbPings = 0;  // the # of times we ask the range
static unsigned long gNbValids = 0; // the # of times the range was valid
// this will be used to emit an valid range event only from time to time
static unsigned long gNbCurrentValids = 0; // the # of times the range was valid since last time it was invalid

// return true every 10 valid range ie roughly every second with the delay in our current loop

static bool rangeIsValidForALongTime(unsigned long iNbValid)
{
  return (iNbValid > 10) && (iNbValid % 10 == 0);
}

/**
 * a range is accepted if between 15 & 25 cm
 * */

static bool isValidRange(double iRange)
{
  return (iRange >= 15) && (iRange <= 25);
}

/** 
 * be very vocal about the provided probe
**/

static void signalValidRange(double iRange)
{
  /** 1 flash the system led with our pattern */
  gValidRangeStatus.setActive(true);
  /** 2 publish our event "ValidRange"  **/
  Particle.publish("ValidRange", String::format("%.2f", iRange), PRIVATE);
  delay(200);
  /** 3 get back to standard system led pattern **/
  gValidRangeStatus.setActive(false);
}

// setup() runs once, when the device is first turned on.

void setup()
{
  // our 'valid range' LEDStatus needs to be tuned before actual usage
  // Orange, blinking fast
  gValidRangeStatus.setColor(RGB_COLOR_RED);
  gValidRangeStatus.setPattern(LED_PATTERN_BLINK);
  gValidRangeStatus.setSpeed(LED_SPEED_FAST);
  gValidRangeStatus.setPeriod(50);

  // publish our state
  // From now, the last valid range found by gSonar is *PERMANENTLY* accessible via the cloud variable named "cm"
  Particle.variable("cm", gCentimeters);
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{
  gCentimeters = gSonar.getDistanceCM();
  // keep tracks of what we have done
  gNbPings++;
  if (isValidRange(gCentimeters))
  {
    gNbValids++;
    gNbCurrentValids++;
  }
  else
  {
    gNbCurrentValids = 0;
  }

  if (rangeIsValidForALongTime(gNbCurrentValids))
  {
    signalValidRange(gCentimeters);
  }
  // classic lines to debug this stuff
  // but what if the device is not on your desk but in the customer's home near Oulan Bator ?
#ifdef DEBUG_ALAIN
  Serial.printf("%f\t", gCentimeters);
#endif // DEBUG
  delay(100);
}