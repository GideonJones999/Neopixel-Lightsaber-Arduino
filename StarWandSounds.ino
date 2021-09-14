#include <Adafruit_NeoPixel.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NUM_LEDS 7

#define BRIGHTNESS 70
const int buttonPin = 6;     // the number of the pushbutton pin
const int neoPin = 2;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
bool pressed = false;
int debounce_counter = 0;
unsigned long millis_at_press=0;
//bool songStarted;
int spell=1;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, neoPin, NEO_GRB + NEO_KHZ800);

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini mp3_play;
void printDetail(uint8_t type, int value);

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT); 
  //randomSeed(analogRead(A0));
  dfm_setup();
  neopixel_setup();
}

byte neopix_gamma[] = {
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

void dfm_setup()
{  
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  mySoftwareSerial.begin(9600);    
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!mp3_play.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  mp3_play.volume(30);  //Set volume value. From 0 to 30
}

void neopixel_setup(){
    // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  checkButton();
}

void checkButton(){
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  if (debounce_counter > 0) {
    debounce_counter--;
  } else {
    if (buttonState == LOW) { //not pressed
      if (pressed) {
        int timesincepress = millis() - millis_at_press;
        if (timesincepress < 500){
          if (timesincepress > 50){
            Serial.println(timesincepress);
            SpellOrder();
          }
        }                       
        debounce_counter=100;
      }  
      pressed=false;
    } else { //pressed
      if (pressed == false) { //this is first press
        //Serial.println("first press");
        millis_at_press = millis();
      }
      pressed=true;
      if (millis() - millis_at_press > 500){
        //pressed=false;
        debounce_counter=100;
//        Serial.print("long ");
//        Serial.print(millis());
//        Serial.print(" - ");
//        Serial.print(millis_at_press);
//        Serial.print(" = ");
//        Serial.println(millis() - millis_at_press);
        song();
      }           
    }
  }
}

void(){
  //int myrand=random(2,7);  
  //Serial.println(spell);
  spell++;
  if (spell == 7){
    spell = 2;
  }
  if (spell == 2) {
    warnicorn();
  } else if (spell == 3) {
    all_seeing_eye();
  } else if (spell == 4) {
    goodtime();
  } else if (spell == 5) {
    narwhal();
  } else {
    spider();
  }  
}

void song() {
  //songStarted=true;
  Serial.println("song");
  mp3_play.play(1);
  pulseColor_fast(strip.Color(255, 255,0), 3); //Yellow
  pulseColor_fast(strip.Color(255, 255,0), 3); //Yellow
  pulseColor_fast(strip.Color(0,0,255), 3); //Blue
  pulseColor_fast(strip.Color(0,0,255), 3); //Blue
  theaterChase(strip.Color(255, 0,250), 40); // Pinkish  
  rainbowFade (3,9); //pretty much 30 seconds//rainbow fade until good time  
  pulseColor_fast(strip.Color(255, 255,0), 3); //Yellow
  pulseColor_fast(strip.Color(0,0,255), 3); //Blue
  rainbowFade(1,33); 
  rainbowFade(3,6); 
  theaterChase(strip.Color(255, 0,250), 125); // Pinkish
  pulseColor_fast(strip.Color(255, 255,0), 3); //Yellow
  //songStarted=false;
}

void warnicorn() {
  Serial.println("warnicorn");
  mp3_play.play(2);
  pulseColor(strip.Color(255, 0,250), 2); // Pinkish
  pulseColor(strip.Color(0, 0, 255), 2); // Blue
  pulseColor(strip.Color(255, 255,0), 2); //Yellow
}

void all_seeing_eye() {
  Serial.println("all_seeing_eye");
  mp3_play.play(3);
  rainbowFade(3,9); //pretty much 30 seconds  
  pulseColor(strip.Color(255, 0,250), 2); // Pinkish
  pulseColor(strip.Color(0, 0, 255), 2); // Blue
  pulseColor(strip.Color(255, 255,0), 2); //Yellow
  pulseColor(strip.Color(255, 153,0), 2); //orange  
}

void goodtime() {
  Serial.println("goodtime");
  mp3_play.play(4);
  pulseColor_fast(strip.Color(255, 255,0), 3); //Yellow
  pulseColor_fast(strip.Color(0,0,255), 3); //blue
}

void narwhal() {
  Serial.println("narhwal");
  mp3_play.play(5);
  delay(250);
  pulseColor(strip.Color(255, 255,0), 6); //Yellow
}

void spider() {
  Serial.println("spider");
  mp3_play.play(6);
  delay(550);
  pulseColor(strip.Color(255, 0,250), 6); // Pinkish 
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void pulseWhite(uint8_t wait) {
  for(int j = 0; j < 256 ; j++){
      for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(0,0,0, neopix_gamma[j] ) );
        }
        delay(wait);
        strip.show();
      }

  for(int j = 255; j >= 0 ; j--){
      for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(0,0,0, neopix_gamma[j] ) );
        }
        delay(wait);
        strip.show();
      }
}

void pulseColor(uint32_t c, uint8_t wait) {    
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c );
     }
   //strip.show();
   for(int j = 1; j < 70 ; j++){
      strip.setBrightness(j);
      //strip.begin();
      delay(wait);
      strip.show();
      }

  for(int j = 70; j >= 1 ; j--){
      strip.setBrightness(j);
      //strip.begin();
      delay(wait*3);
      strip.show();
     }
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0,0,0 ) );
     }
  strip.setBrightness(BRIGHTNESS);
  strip.show();     
}

void pulseColor_fast(uint32_t c, uint8_t wait) {    
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c );
     }
   //strip.show();
   for(int j = 1; j < 70 ; j=j+2){
      strip.setBrightness(j);
      //strip.begin();
      delay(wait);
      strip.show();
      }

  for(int j = 70; j >= 1 ; j=j-2){
      strip.setBrightness(j);
      //strip.begin();
      delay(wait*3);
      strip.show();
     }
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0,0,0 ) );
     }
  strip.setBrightness(BRIGHTNESS);
  strip.show();     
}


void rainbowFade(uint8_t wait, int rainbowLoops) {
  float fadeMax = 100.0;
  int fadeVal = 0;
  uint32_t wheelVal;
  int redVal, greenVal, blueVal;

  for(int k = 0 ; k < rainbowLoops ; k ++){
    for(int j=0; j<256; j++) { // 5 cycles of all colors on wheel
      for(int i=0; i< strip.numPixels(); i++) {
        wheelVal = Wheel(((i * 256 / strip.numPixels()) + j) & 255);

        redVal = red(wheelVal) * float(fadeVal/fadeMax);
        greenVal = green(wheelVal) * float(fadeVal/fadeMax);
        blueVal = blue(wheelVal) * float(fadeVal/fadeMax);

        strip.setPixelColor( i, strip.Color( redVal, greenVal, blueVal ) );
      }
      //First loop, fade in!
      if(k == 0 && fadeVal < fadeMax-1) {
          fadeVal++;
      }
      //Last loop, fade out!
      else if(k == rainbowLoops - 1 && j > 255 - fadeMax ){
          fadeVal--;
      }
        strip.show();
        delay(wait);
    }  
  }
}

void rainbowFade2White(uint8_t wait, int rainbowLoops, int whiteLoops) {
  float fadeMax = 100.0;
  int fadeVal = 0;
  uint32_t wheelVal;
  int redVal, greenVal, blueVal;

  for(int k = 0 ; k < rainbowLoops ; k ++){
    for(int j=0; j<256; j++) { // 5 cycles of all colors on wheel
      for(int i=0; i< strip.numPixels(); i++) {
        wheelVal = Wheel(((i * 256 / strip.numPixels()) + j) & 255);

        redVal = red(wheelVal) * float(fadeVal/fadeMax);
        greenVal = green(wheelVal) * float(fadeVal/fadeMax);
        blueVal = blue(wheelVal) * float(fadeVal/fadeMax);

        strip.setPixelColor( i, strip.Color( redVal, greenVal, blueVal ) );
      }
      //First loop, fade in!
      if(k == 0 && fadeVal < fadeMax-1) {
          fadeVal++;
      }
      //Last loop, fade out!
      else if(k == rainbowLoops - 1 && j > 255 - fadeMax ){
          fadeVal--;
      }
        strip.show();
        delay(wait);
    }  
  }
  delay(500);

  for(int k = 0 ; k < whiteLoops ; k ++){

    for(int j = 0; j < 256 ; j++){
        for(uint16_t i=0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(0,0,0, neopix_gamma[j] ) );
          }
          strip.show();
        }
        delay(2000);
    for(int j = 255; j >= 0 ; j--){
        for(uint16_t i=0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(0,0,0, neopix_gamma[j] ) );
          }
          strip.show();
        }
  }
  delay(500);
}

void whiteOverRainbow(uint8_t wait, uint8_t whiteSpeed, uint8_t whiteLength ) {
  
  if(whiteLength >= strip.numPixels()) whiteLength = strip.numPixels() - 1;

  int head = whiteLength - 1;
  int tail = 0;

  int loops = 3;
  int loopNum = 0;

  static unsigned long lastTime = 0;


  while(true){
    for(int j=0; j<256; j++) {
      for(uint16_t i=0; i<strip.numPixels(); i++) {
        if((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head) ){
          strip.setPixelColor(i, strip.Color(0,0,0, 255 ) );
        }
        else{
          strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
        }
        
      }

      if(millis() - lastTime > whiteSpeed) {
        head++;
        tail++;
        if(head == strip.numPixels()){
          loopNum++;
        }
        lastTime = millis();
      }

      if(loopNum == loops) return;
    
      head%=strip.numPixels();
      tail%=strip.numPixels();
        strip.show();
        delay(wait);
    }
  }
  
}
void fullWhite() {
  
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0,0,0, 255 ) );
    }
      strip.show();
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256 * 5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait, int rainbowLoops) {
  uint16_t i, j;
  for(int k = 0 ; k < rainbowLoops ; k ++){
    for(j=0; j<256; j++) {
      for(i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel((i+j) & 255));
      }
      strip.show();
      delay(wait);
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3,0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3,0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0,0);
}

uint8_t red(uint32_t c) {
  return (c >> 16);
}
uint8_t green(uint32_t c) {
  return (c >> 8);
}
uint8_t blue(uint32_t c) {
  return (c);
}
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

