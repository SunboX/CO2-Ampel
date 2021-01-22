/**
   SenseAir Sunrise
*/

#include <ESP8266WiFi.h>
#include <FastLED.h>
#include "Sunrise.h"

#define D1 5
#define D2 4
#define D3 0

#define PIN_EN 2
#define SAMPLE_DURATION_MAX 200UL

int CO2 = 0 ;

// Anzahl der "NeoPixel"
#define NUM_PIXELS 12

#define START_LED 0

// Mit welchem GPIO ist der LED-Strip verbunden?
#define PIXEL_PIN D3

#define COLOR_ORDER RGB

// Bereich 0-64
#define BRIGHTNESS 50

// Definition des Pixelarray
CRGB pixels[NUM_PIXELS];

// Start Position des Cycling-Effekt
int pixelAddress = 0;

Sunrise sunrise;
unsigned long int measurementInterval;
unsigned long int measurementDuration;
unsigned long int nextMeasurement;

void delayUntil(unsigned long int time)
{
  while ((long)(millis() - time) < 0L);
}

void switchMode(measurementmode_t mode)
{
  while (true)
  {
    measurementmode_t measurementMode;
    if (!sunrise.GetMeasurementModeEE(measurementMode))
    {
      Serial.println("*** ERROR: Could not get measurement mode");
      while (true);
    }

    if (measurementMode == mode)
    {
      break;
    }

    Serial.println("Attempting to switch measurement mode...");
    if (!sunrise.SetMeasurementModeEE(mode))
    {
      Serial.println("*** ERROR: Could not set measurement mode");
      while (true);
    }

    if (!sunrise.HardRestart())
    {
      Serial.println("*** ERROR: Failed to restart the device");
      while (true);
    }
  }
}

void setup() { // Einmalige Initialisierung
  Serial.begin(9600);
  Serial.println("Starting...");

  WiFi.forceSleepBegin(); // Wifi off
  // 1.1  If a custom pin is prefered over VCC.
  //      Note:   If using the reset method/command, the state of the ENABLE pin must be set to LOW and then HIGH again to wake up the device.
  //              This is done automatically if Begin() is provided with the pin connected to ENABLE.
  sunrise.Begin(PIN_EN);

  // 2.1  If VCC is prefered:
  //      Note:   If using the reset method/command, you must manuelly set ENABLE pin to LOW and then back to HIGH again.
  //              Either by swapping the ENABLE pin from VCC to ground and back or cut the power to the device entirely.

  // 1.2  Awake() is only necessary if going with option 1.
  sunrise.Awake();

  switchMode(measurementmode_t::MM_CONTINUOUS);

  uint16_t mp;
  if (!sunrise.GetMeasurementPeriodEE(mp))
  {
    Serial.println("*** ERROR: Could not get measurement period");
    while (true);
  }
  Serial.print("Measurement period:  ");
  Serial.println(mp);
  measurementInterval = mp * 1000UL;

  uint16_t nos;
  if (!sunrise.GetNumberOfSamplesEE(nos))
  {
    Serial.println("*** ERROR: Could not get number of samples");
    while (true);
  }
  Serial.print("Number of samples:   ");
  Serial.println(nos);
  measurementDuration = SAMPLE_DURATION_MAX * nos;

  Serial.println("Done");
  Serial.println();
  Serial.flush();

  nextMeasurement = measurementInterval;

  // Den LED-Streifen initialisieren und die Helligkeit einstellen
  FastLED.addLeds<NEOPIXEL, PIXEL_PIN>(pixels, NUM_PIXELS); // Init der Fastled-Bibliothek
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() { // Kontinuierliche Wiederholung
  delayUntil(nextMeasurement);
  nextMeasurement += measurementInterval;
  delay(measurementDuration);
  delay(500UL);

  Serial.println("Measuring...");
  if (sunrise.ReadMeasurement())
  {
    CO2 = sunrise.GetCO2();

    Serial.print("CO2:" + String(String(CO2)));
    Serial.println();
  }
  else
  {
    Serial.println("*** ERROR: Could not read measurement");
  }
  Serial.println();

  if (CO2 < 600) {
    show(1, 0x00FF00);
  } else if (CO2 < 700) {
    show(2, 0x2EFF00);
  } else if (CO2 < 800) {
    show(3, 0x5DFF00);
  } else if (CO2 < 900) {
    show(4, 0x8BFF00);
  } else if (CO2 < 1000) {
    show(5, 0xB9FF00);
  } else if (CO2 < 1100) {
    show(6, 0xE8FF00);
  } else if (CO2 < 1200) {
    show(7, 0xFFE800);
  } else if (CO2 < 1300) {
    show(8, 0xFFB900);
  } else if (CO2 < 1400) {
    show(9, 0xFF8B00);
  } else if (CO2 < 1500) {
    show(10, 0xFF5D00);
  } else if (CO2 < 1600) {
    show(11, 0xFF2E00);
  } else if (CO2 < 2000) {
    show(12, 0xFF0000);
  }

  // Alert
  if (CO2 >= 2000) {
    show(0, 0x000000);
    delay(1000);
    show(12, 0xFF0000);
    delay(1000);
    show(0, 0x000000);
    delay(1000);
    show(12, 0xFF0000);
    delay(1000);
    show(0, 0x000000);
    delay(1000);
    show(12, 0xFF0000);
    delay(1000);
    show(0, 0x000000);
    delay(1000);
    show(12, 0xFF0000);
    delay(1000);
  }
  else {
    delay(2000);
  }
}

void show(int numPixels, CRGB color) {

  for (int i = 0; i < NUM_PIXELS; i++)
  {
    pixels[i] = CRGB::Black; // Schaltet alle LEDs aus
  }

  int curLED = START_LED;

  while (numPixels--) {
    pixels[curLED++] = color;
    if (curLED == NUM_PIXELS) {
      curLED = 0;
    }
  }

  FastLED.show();
}
