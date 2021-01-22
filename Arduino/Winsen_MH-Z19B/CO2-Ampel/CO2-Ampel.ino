/**
   Sensirion MH-Z19B
*/

#include <MHZ.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>

#define D1 5
#define D2 4
#define D3 0

// pin for uart reading
#define MH_Z19_RX D1
#define MH_Z19_TX D2

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

MHZ mhz(MH_Z19_RX, MH_Z19_TX, NULL, MHZ19B);

void setup() { // Einmalige Initialisierung
  Serial.begin(9600);
  Serial.println("Starting...");

  WiFi.forceSleepBegin(); // Wifi off

  // Den LED-Streifen initialisieren und die Helligkeit einstellen
  FastLED.addLeds<NEOPIXEL, PIXEL_PIN>(pixels, NUM_PIXELS); // Init der Fastled-Bibliothek
  FastLED.setBrightness(BRIGHTNESS);

  show(0, 0x000000);

  mhz.setAutoCalibrate(true);

  unsigned long preHeatingTime = 3L * 60L * 1000L;
  unsigned long timeDelta = preHeatingTime / NUM_PIXELS;
  int i = 0;
  if (mhz.isPreHeating()) {
    Serial.println("Preheating:");
    while (mhz.isPreHeating()) {
      Serial.println(preHeatingTime);
      preHeatingTime -= timeDelta;
      cylon(timeDelta, NUM_PIXELS - i++);
    }
    Serial.println();
  }
}

void loop() { // Kontinuierliche Wiederholung
  CO2 = mhz.readCO2UART();

  if (CO2 == STATUS_NO_RESPONSE) {
    Serial.println("no response");
    delay(5000);
  }
  else if (CO2 == STATUS_CHECKSUM_MISMATCH) {
    Serial.println("checksum mismatch");
    delay(5000);
  }
  else if (CO2 == STATUS_INCOMPLETE) {
    Serial.println("incomplete");
    delay(5000);
  }
  else if (CO2 == STATUS_NOT_READY) {
    Serial.println("not ready");
    delay(5000);
  }
  else {
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

void fadeall()
{
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels[i].nscale8(250);
  }
}

// Farb-Regenbogen-Effekt
void cylon(int milliseconds, int leds)
{
  if (leds <= 0) {
    leds = 1;
  }
  int amount = milliseconds / (leds * 20);
  for (int i = 0; i < amount; i++)
  {
    static uint8_t hue = 0;
    for (int i = 0; i < leds; i++) {
      pixels[i] = CHSV(hue++, 255, 255);
      FastLED.show();
      fadeall();
      delay(10);
    }
    for (int i = (leds) - 1; i >= 0; i--) {
      pixels[i] = CHSV(hue++, 255, 255);
      FastLED.show();
      fadeall();
      delay(10);
    }
  }
}
