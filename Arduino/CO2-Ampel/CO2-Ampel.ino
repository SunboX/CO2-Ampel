#include <paulvha_SCD30.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>

#define SCD30_ADDRESS 0x61

#define D1 5
#define D2 4
#define D3 0

int CO2 = 0 ;

// Anzahl der "NeoPixel"
#define NUM_PIXELS 12

#define START_LED 10

// Mit welchem GPIO ist der LED-Strip verbunden?
#define PIXEL_PIN 0

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

  Wire.begin(D2, D1); // ---- Initialisiere den I2C-Bus

  Wire.setClock(100000L);            // 100 kHz SCD30
  Wire.setClockStretchLimit(200000L);// CO2-SCD30

  if (Wire.status() != I2C_OK) {
    Serial.println("Something wrong with I2C");
  }

  if (airSensorSCD30.begin(Wire, false) == false) {
    Serial.println("The SCD30 did not respond. Please check wiring.");

    delay(300);

    while (1) {
      yield();
      delay(1);
    }
  }

  airSensorSCD30.setAutoSelfCalibration(false); // Sensirion no auto calibration

  airSensorSCD30.setMeasurementInterval(2);     // CO2-Messung alle 5 s

  // Den LED-Streifen initialisieren und die Helligkeit einstellen
  FastLED.addLeds<NEOPIXEL, PIXEL_PIN>(pixels, NUM_PIXELS); // Init der Fastled-Bibliothek
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() { // Kontinuierliche Wiederholung
  CO2 = airSensorSCD30.getCO2() ;
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
  } else if (CO2 < 1800) {
    show(12, 0xFF0000);
  }

  // Alert
  if (CO2 >= 1800) {
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
