/**
 * Sensirion SCD30
 */
 
#include <paulvha_SCD30.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>

#define D3 0

#define MH_Z14_ANALOG_PIN 6


int CO2 = 0 ;

// Anzahl der "NeoPixel"
#define NUM_PIXELS 12

#define START_LED 10

// Mit welchem GPIO ist der LED-Strip verbunden?
#define PIXEL_PIN D3

#define COLOR_ORDER RGB

// Bereich 0-64
#define BRIGHTNESS 50

// Definition des Pixelarray
CRGB pixels[NUM_PIXELS];

// Start Position des Cycling-Effekt
int pixelAddress = 0;

SCD30 airSensorSCD30; // Objekt SDC30 Umweltsensor

void setup() { // Einmalige Initialisierung
  Serial.begin(115200);
  Serial.println("Starting...");

  WiFi.forceSleepBegin(); // Wifi off

  // Den LED-Streifen initialisieren und die Helligkeit einstellen
  FastLED.addLeds<NEOPIXEL, PIXEL_PIN>(pixels, NUM_PIXELS); // Init der Fastled-Bibliothek
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() { // Kontinuierliche Wiederholung

  // wait for the pin to go HIGH and measure HIGH time
  while(digitalRead(MH_Z14_ANALOG_PIN) == HIGH) {;}
  unsigned long duration = pulseIn(MH_Z14_ANALOG_PIN, HIGH);
  
  //from datasheet
  //CO2 ppm = 2000 * (Th - 2ms) / (Th + Tl - 4ms)
  //  given Tl + Th = 1004
  //        Tl = 1004 - Th
  //        = 2000 * (Th - 2ms) / (Th + 1004 - Th -4ms)
  //        = 2000 * (Th - 2ms) / 1000 = 2 * (Th - 2ms)
  CO2 = 2 * ((duration/1000) - 2);

  Serial.print("CO2:" + String(String(CO2)));
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
