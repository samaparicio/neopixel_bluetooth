#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
#include <Adafruit_NeoPixel.h>


/*
 * (c) 2017 Sam Aparicio - MIT license - https://opensource.org/licenses/MIT
 * Learn more about this project at - https://samaparicio.github.io/bluetooth-neopixels.html
 * Shout out to Adafruit - https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/controller
 * Shout out to Tweaking4All - https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 */
 
//Perform a factory reset when running this sketch 1 yes, 0 no
#define FACTORYRESET_ENABLE     1

// PIN on Arduino that connects to Neopixels
#define PIN                     6

// How many pixels on your strip. 1m normally has 60
#define NUMPIXELS               240

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// packet buffer to store received bluetooth messages
extern uint8_t packetbuffer[];

//Color
uint8_t red = 100;
uint8_t green = 100;
uint8_t blue = 100;

// 5 sets the animationState to all one color
uint8_t animationState = 5;

void setup(void)
{
  delay(500);

  strip.begin(); // This initializes the NeoPixel library.
  allOneColor(); // sets all the pixels to one color to begin with

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Neopixel for Clara"));
  Serial.println(F("-------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in Command mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then use the color picker and game controller"));
  Serial.println();

  ble.verbose(false);

  /* Wait for connection */
  while (! ble.isConnected()) {
    delay(500);
  }

  Serial.println(F("***********************"));

  // Set Bluefruit to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("***********************"));

}


void loop(void)
{
  // Wait for new data to arrive
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);

  // Color picker selection
  if (packetbuffer[1] == 'C') {
    red = packetbuffer[2];
    green = packetbuffer[3];
    blue = packetbuffer[4];
    Serial.print ("RGB #");
    if (red < 0x10) Serial.print("0");
    Serial.print(red, HEX);
    if (green < 0x10) Serial.print("0");
    Serial.print(green, HEX);
    if (blue < 0x10) Serial.print("0");
    Serial.println(blue, HEX);
    allOneColor();
  }

  // Button presses
  if (packetbuffer[1] == 'B') {

    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    Serial.print ("Button "); Serial.print(buttnum);
    animationState = buttnum;
    if (pressed) {
      Serial.println(" pressed");
    } else {
      Serial.println(" released");
    }
  }

  if (animationState == 1) { // button labeled "1" in control pad
    initializePixels();
    theaterChaseRainbow(50);
  }

  if (animationState == 2) { // button labeled "2" in control pad
    initializePixels();
    cylonBounce(0xff, 0, 0, 4, 10, 50);
  }

  if (animationState == 3) { // button labeled "3" in control pad
    initializePixels();
    RGBLoop();

  }

  if (animationState == 4) { // button labeled "4" in control pad
    initializePixels();
    rainbowCycle(20);
  }

  if (animationState == 5) { // button up arrow in control pad
    initializePixels();
    allOneColor();
  }

  if (animationState == 6) { // button down arrow in control pad
    initializePixels();
    fire(55, 120, 15);
  }

  if (animationState == 7) { // button left arrow in control pad
    initializePixels();
    colorWipe(red, green, blue, 50);
  }
  if (animationState == 8) { // button right arrow in control pad
    initializePixels();
    twinkleRandom(20, 100, false);

  }



}

void allOneColor() {
  for (uint8_t i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  showStrip(); // This sends the updated pixel color to the hardware.
}


void theaterChaseRainbow(int SpeedDelay) {
  byte *c;

  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < NUMPIXELS; i = i + 3) {
        c = Wheel( (i + j) % 255);
        setPixel(i + q, *c, *(c + 1), *(c + 2)); //turn every third pixel on
      }
      showStrip();

      delay(SpeedDelay);

      for (int i = 0; i < NUMPIXELS; i = i + 3) {
        setPixel(i + q, 0, 0, 0);    //turn every third pixel off
      }
    }
  }
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < NUMPIXELS; i++) {
      c = Wheel(((i * 256 / NUMPIXELS) + j) & 255);
      setPixel(i, *c, *(c + 1), *(c + 2));
    }
    showStrip();
    delay(SpeedDelay);
  }
}

void cylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {

  for (int i = 0; i < NUMPIXELS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for (int i = NUMPIXELS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);
}

void RGBLoop() {
  for (int j = 0; j < 3; j++ ) {
    // Fade IN
    for (int k = 0; k < 256; k++) {
      switch (j) {
        case 0: setAll(k, 0, 0); break;
        case 1: setAll(0, k, 0); break;
        case 2: setAll(0, 0, k); break;
      }
      showStrip();
      delay(3);
    }
    // Fade OUT
    for (int k = 255; k >= 0; k--) {
      switch (j) {
        case 0: setAll(k, 0, 0); break;
        case 1: setAll(0, k, 0); break;
        case 2: setAll(0, 0, k); break;
      }
      showStrip();
      delay(3);
    }
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}

void fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUMPIXELS];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUMPIXELS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUMPIXELS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUMPIXELS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if ( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for ( int j = 0; j < NUMPIXELS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if ( t192 > 0x40 ) {            // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}


void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  for (uint16_t i = 0; i < NUMPIXELS; i++) {
    setPixel(i, red, green, blue);
    showStrip();
    delay(SpeedDelay);
  }
}


void twinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i = 0; i < Count; i++) {
    setPixel(random(NUMPIXELS), random(0, 255), random(0, 255), random(0, 255));
    showStrip();
    delay(SpeedDelay);
    if (OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  delay(SpeedDelay);
}

void showStrip() {
  strip.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {

  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUMPIXELS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void initializePixels() {
  for (uint8_t i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // off
  }
}

