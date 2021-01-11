// Simple NeoPixel test.  Lights just a few pixels at a time so a
// 1m strip can safely be powered from Arduino 5V pin.  Arduino
// may nonetheless hiccup when LEDs are first connected and not
// accept code.  So upload code first, unplug USB, connect pixels
// to GND FIRST, then +5V and digital pin 6, then re-plug USB.
// A working strip will show a few pixels moving down the line,
// cycling between red, green and blue.  If you get no response,
// might be connected to wrong end of strip (the end wires, if
// any, are no indication -- look instead for the data direction
// arrows printed on the strip).

#include <Adafruit_NeoPixel.h>

#define PIN      6
#define N_LEDS 131

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
}

void loop() {
  #define Blue strip.Color(pgm_read_byte(0,0,255)
  #define Green strip.Color(pgm_read_byte(6, 108, 6)
  #define Orange strip.Color(pgm_read_byte(255,164,8)
  #define Purple strip.Color(75, 0, 130)
  #define Yellow strip.Color(255, 255, 0)
  #define Cyan strip.Color(0,255,255)//(65,181,175)
  #define Magenta strip.Color(176, 49, 156)
  #define Red strip.Color(255, 0, 0)
  #define White strip.Color(255, 255, 255)

//  strip.setPixelColor(1, Blue);
//  strip.setPixelColor(2, Green);
//  strip.setPixelColor(3, Orange);
//  strip.setPixelColor(4, Purple);
//  strip.setPixelColor(5, Yellow);
//  strip.setPixelColor(6, Cyan);
//  strip.setPixelColor(7, Magenta);
//  strip.setPixelColor(8, Red);
//  strip.setPixelColor(9, White);
//  strip.show();
  chase(Blue);
  chase(Green);
  chase(Orange);
  chase(Purple);
  chase(Yellow);
  chase(Cyan);
  chase(Magenta);
  chase(Red);
  chase(White);
//  dadTest(strip.Color(225,0,0));
//  dadTest(strip.Color(0,225,0));
//  dadTest(strip.Color(0,0,225));
//  ignite(strip.Color(255, 0, 0)); // Red
//  ignite(strip.Color(0, 255, 0)); // Green
//  ignite(strip.Color(0, 0, 255)); // Blue
//  extinguish();
}

static void chase(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels()+4; i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip.show();
      delay(25);
  }
}

static void dadTest(uint32_t c) {
   strip.setPixelColor(71  , c); // Draw new pixel
   strip.show();
   delay(50);
   strip.setPixelColor(72  , c); // Draw new pixel
   strip.show();
   delay(50);
   strip.setPixelColor(73  , c); // Draw new pixel
   strip.show();
   delay(50);
   strip.setPixelColor(74  , c); // Draw new pixel
   strip.show();
   delay(50);
   strip.setPixelColor(130  , c); // Draw new pixel
   strip.show();
   delay(50);
}

static void ignite(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.show();
      delay(25);
  }
}

static void extinguish() {
  for(uint16_t i=strip.numPixels(); i >= 0; i--) {
      strip.setPixelColor(i, 0);
      strip.show();
      delay(25);
  }
}
