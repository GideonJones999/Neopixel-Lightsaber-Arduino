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

const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
}

void loop() {
  #define Blue strip.Color(pgm_read_byte(&gamma8[0]),pgm_read_byte(&gamma8[0]),pgm_read_byte(&gamma8[255]))
  #define Green strip.Color(pgm_read_byte(&gamma8[6]), pgm_read_byte(&gamma8[108]), pgm_read_byte(&gamma8[6]))
  #define Orange strip.Color(pgm_read_byte(&gamma8[255]),pgm_read_byte(&gamma8[128]),pgm_read_byte(&gamma8[0]))
  #define Purple strip.Color(pgm_read_byte(&gamma8[75]), pgm_read_byte(&gamma8[0]), pgm_read_byte(&gamma8[130]))
  #define Yellow strip.Color(pgm_read_byte(&gamma8[255]), pgm_read_byte(&gamma8[255]), pgm_read_byte(&gamma8[0]))
  #define Cyan strip.Color(pgm_read_byte(&gamma8[0]),pgm_read_byte(&gamma8[255]),pgm_read_byte(&gamma8[255]))//(pgm_read_byte(&gamma8[65]),pgm_read_byte(&gamma8[181]),pgm_read_byte(&gamma8[175]))
  #define Magenta strip.Color(pgm_read_byte(&gamma8[176]), pgm_read_byte(&gamma8[49]), pgm_read_byte(&gamma8[156]))
  #define Red strip.Color(pgm_read_byte(&gamma8[255]), pgm_read_byte(&gamma8[0]), pgm_read_byte(&gamma8[0]))
  #define White strip.Color(pgm_read_byte(&gamma8[255]), pgm_read_byte(&gamma8[255]), pgm_read_byte(&gamma8[255]))

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
