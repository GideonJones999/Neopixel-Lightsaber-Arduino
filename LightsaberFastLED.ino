#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
// #include <DFRobotDFPlayerMini.h>
#include "MPU6050.h"
#include <FastLED.h>


//int16_t ax, ay, az;
//int16_t gx, gy, gz;
//long ACC, GYR, COMPL;
#define FLASH_DELAY 80      // flash time while hit

#define lightsPin    6
#define bladeCheckPin 9
#define buttonPin 4
#define BTN_TIMEOUT 800     // button hold delay, ms

#define SWING_L_THR 150     // swing angle speed threshold
#define SWING_THR 300       // fast swing angle speed threshold

#define STRIKE_THR 150      // hit acceleration threshold
#define STRIKE_S_THR 320    // hard hit acceleration threshold

#define N_LEDS 131
#define BLADE_IGNITE_TIME 250
#define LIGHT_DELAY BLADE_IGNITE_TIME / N_LEDS

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
CRGB leds[N_LEDS]; //FASTLed
CRGB bladeColor;
DFPlayerMini_Fast mp3_play;
//SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
//Adafruit_NeoPixel strip(N_LEDS, PIN, NEO_GRB + NEO_KHZ800); //see FastLED in setup
MPU6050 accelgyro;

unsigned long btn_timer;
unsigned long btn_timer2;
unsigned long PULSE_timer; 
unsigned long LAST_SWING_TIME; 
unsigned long SWING_TIMER;

bool btnState, btnflag, holdflag;
bool idle = false;
bool swinging = false; 
bool crashing = false; 
bool activating = false;  
bool isBladeActivated = false;
bool isBladeIn = false;

int currentColor = 1;
int randHum, randNoise;
//int LIGHT_DELAY = BLADE_IGNITE_TIME / N_LEDS;
//int lightDelay = 5;
//int bladeRed, bladeGreen, bladeBlue;
//String currentColorText;



//const uint8_t PROGMEM gamma8[] = {
//  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
//  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
//  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
//  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
//  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
//  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
//  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
//  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
//  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
//  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
//  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
//  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
//  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
//  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
//  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
//};


void setup() {
  Serial.begin(19200);
  // delay(5000);
  Serial.println(F("Startup Start"));
  pinMode(bladeCheckPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.println(F("Pin Mode Added"));
  mp3Setup();
  playSound("on");
  Serial.println(F("MP3 Setup"));
  FastLED.addLeds<WS2811, lightsPin, RGB>(leds, N_LEDS);
  neoPixelSetup();
  Serial.println(F("LEDs Added"));
  // mp3Setup();
  // playSound("on");
  // Serial.println(F("MP3 Setup"));
  gyroSetup();
  Serial.println(F("Gyro Setup"));
  Serial.println(F("Startup completed"));
//  Serial.printme("lightdelay ");
//  Serial.printlnme(lightDelay);
}

void mp3Setup() {
  Serial.println(F("Start MP3 Setup"));
  
  Serial.println(F("Serial Settings"));
  mySoftwareSerial.begin(9600);
  Serial.println(F("Serial Start"));
  Serial.println("Start MP3 Setup");
  if (!mp3_play.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
//  if(4==4) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  Serial.println("DFPlayerMini online");
  mp3_play.volume(30);  //Set volume value. From 0 to 30
  randomSeed(analogRead(0));
}

void neoPixelSetup() {
  if ((EEPROM.read(0) >= 1) && (EEPROM.read(0) <= 11)) {
    currentColor = EEPROM.read(0);
  } else {
    EEPROM.write(0, 1);
    currentColor = 1;
  }
  //strip.begin();
  //strip.setBrightness(25);
  FastLED.setBrightness(25);
  //strip.clear();
  changeColor(currentColor);
  //  bladeColor = strip.Color(pgm_read_byte(&gamma8[bladeRed]), pgm_read_byte(&gamma8[bladeGreen]), pgm_read_byte(&gamma8[bladeBlue]));

  //    checkBattery();
}

void gyroSetup() {
  // Initial calibration of gyro
  Serial.println(F("MPU6050 Gyro"));
  // MPU6050 accelgyro;
  accelgyro.initialize();
  //accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
  //accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  Serial.println(F("Set Accel"));
  accelgyro.setFullScaleAccelRange(3); // +/- 16g | 1024 LSB/mg
  Serial.println(F("Accel Set"));
  Serial.println(F("Set Gyro"));
  accelgyro.setFullScaleGyroRange(0); //  +/- 250 degrees/sec
  Serial.println(F("Gyro Set"));
  Serial.println(F("MPU6050 Gyro Initialized"));
}


void loop() {
  checkButton();
  if (isBladeActivated || activating) {
    if (isBladeIn) {
      if (mp3_play.isPlaying() == false) {
        playSound("hum");
      }
      checkButton();
      getCompl();
      checkButton();
      //checkStrike();
      //checkSwing();
      checkButton();
      swinging = false;
      crashing = false;
    }
    else {
      //strip.fill(bladeColor);
      fill_solid(leds, 5, bladeColor);
      //      Serial.printlnme("ln 134");
      //strip.show();
      FastLED.show();
    }
  } else { //not isBladeActivated
    if (!activating) {
      fill_solid(leds, N_LEDS, CRGB::Black);
    }
    //strip.show();
    FastLED.show();
  }
}

void playSound(String soundType) {
  randNoise = random(1, 9); //picking random number from 1 to 8
  switch (soundType[0]) {
    case 'f': //fast swing
      Serial.print(F("fast swing noise"));
      mp3_play.playFolder(5, randNoise);
//      Serial.printlnme(randNoise);
      break;
    case 'l': //sLow swing
      Serial.print(F("slow swing noise"));
      mp3_play.playFolder(4, randNoise);
//      Serial.printlnme(randNoise);
      break;
    case 's': //strong hit
      Serial.print(F("strong hit noise"));
      mp3_play.playFolder(7, randNoise);
//      Serial.printlnme(randNoise);
      break;
    case 'w': //weak hit
      Serial.print(F("weak hit noise"));
      mp3_play.playFolder(6, randNoise);
//      Serial.printlnme(randNoise);
      break;
    case 'i': //ignite
      randNoise = random(1, 3);
      Serial.print(F("ignite noise"));
      mp3_play.playFolder(1, randNoise);
//      Serial.printlnme(randNoise);
      randHum = randNoise;
      break;
    case 'e': //extinguish
      randNoise = random(1, 3);
      Serial.print(F("extinguish noise"));
      mp3_play.playFolder(2, randNoise);
//      Serial.printlnme(randNoise);
      break;
    case 'o': //on
      Serial.println(F("saber on noise"));
      mp3_play.play(1);
      break;
    case 'p': //power off
      Serial.println(F("saber off noise"));
      mp3_play.play(2);
      break;
    case 'h': //hum
      Serial.println(F("hum noise"));
      mp3_play.playFolder(3, randHum);
//      Serial.printlnme(randHum);
      break;
    case 'm': //misfire
      Serial.println(F("Saber misfire"));
      mp3_play.play(3);
      break;
    case 'c': //change color
      Serial.println(F("Color Change Sound"));
      mp3_play.play(4);
      break;
    default:
      Serial.print(F("hum noise"));
      mp3_play.playFolder(3, randHum);
      //Serial.printlnme(randHum);
      break;
  }
}

static void checkButton() {
  
  delay(100);
  btnState = !digitalRead(buttonPin);
  if (btnflag && btnState && (millis() - btn_timer > BTN_TIMEOUT) && !holdflag) {
    checkBladeThere();
    Serial.println(F("Button Held!"));
    holdflag = 1;
    if (isBladeActivated) {
      if (isBladeIn) {
        extinguishNova();
      }
      else {
        changeHiltLight(CRGB::Black);
      }
    }
    delay(500);
    changeColorAudio(currentColor + 1);
    activating = true;
    Serial.println(F("221 activating"));
    isBladeActivated = true;
    if (isBladeIn) {
      igniteNova(bladeColor);
    }
    else {
      changeHiltLight(bladeColor);
    }
    activating = false;
  }
  else if (btnState && !btnflag) {
    checkBladeThere();
    Serial.println(F("Button Pressed!"));
    btnflag = 1;
    btn_timer = millis();
    if (isBladeActivated) {
      if (isBladeIn) {
        extinguishNova();
      }
      else {
        changeHiltLight(CRGB::Black);
      }
      Serial.println(F("244 blade is activating false"));
      isBladeActivated = false;
    }
    else {
      activating = true;
      Serial.println(F("249 blade is activating true"));
      isBladeActivated = true;
      if (isBladeIn) {
        igniteNova(bladeColor);
      }
      else {
        changeHiltLight(bladeColor);
      }
      isBladeActivated = true;
    }
    activating = false;
  }
  else if (!btnState && btnflag) {
    btnflag = 0;
    holdflag = 0;
  }
}

static void changeColorAudio (int newColorNumb) {
  playSound("change");
  Serial.println(F("playingAudio"));
  delay(2500);
  changeColor(currentColor + 1);
}

static void changeHiltLight(CRGB c) {
  playSound("misfire");
  isBladeActivated = !isBladeActivated;
  Serial.print(F("change Hilt Light. Blade is "));
  if (isBladeActivated) {
    Serial.println(F("on"));
  }
  else {
    Serial.println(F("off"));
  }
  fill_solid(leds, N_LEDS, c);
  Serial.println(F("ln 276"));
  //strip.show();
  FastLED.show();
}


static void checkBladeThere() {
  isBladeIn = !digitalRead(bladeCheckPin);
  if (isBladeIn == 1) {
    Serial.println(F("Blade is in"));
  } else {
    Serial.println(F("Blade is out"));
  }
}

static void ignite(uint32_t c) {
  Serial.println(F("Ignition"));
  playSound("ignite");
  for (uint16_t i = 0; i < N_LEDS; i++) {
    //  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    //    strip.setPixelColor(i  , c); // Draw new pixel
    leds[i] = c; // Draw new pixel
    //strip.show();
    FastLED.show();
    delay(LIGHT_DELAY);
  }
  randHum = random(1, 3); //Get Random Value of 1 or 2
}

static void igniteNova(CRGB c) {
  Serial.println(F("Ignition N"));
  playSound("ignite");
  for (uint16_t i = 0; i < N_LEDS; i++) {
    if (i + 1 != N_LEDS) {
      //      strip.setPixelColor(i + 1, strip.Color(255, 255, 255));
      leds[i + 1] = CRGB::White;
    }
    //    strip.setPixelColor(i  , c); // Draw new pixel
    leds[i] = c;
    //strip.show();
    FastLED.show();
    delay(LIGHT_DELAY);
  }
  randHum = random(1, 3); //Get Random Value of 1 or 2
}

static void igniteUnstable(uint32_t c) {
  Serial.println(F("Ignition U"));
  playSound("ignite");
  //  for (uint16_t i = 0; i < strip.numPixels(); i++) {
  for (uint16_t i = 0; i < N_LEDS; i++) {
    //    if (i != strip.numPixels() && i % 4 == 0) {
    if (i != N_LEDS && i % 4 == 0) {
      //      strip.setPixelColor(i + 1, strip.Color(255, 255, 255));
      leds[i + 1] = CRGB::White;
    }
    //    strip.setPixelColor(i  , c); // Draw new pixel
    leds[i] = c;
    //strip.show();
    FastLED.show();
    delay(LIGHT_DELAY);
  }
  randHum = random(1, 3); //Get Random Value of 1 or 2
}

static void extinguish() {
  Serial.println(F("Extinguish"));
  playSound("extinguish");
  //  for (uint16_t i = strip.numPixels(); i > 0; i--) {
  for (uint16_t i = N_LEDS-1; i > 0; i--) {
    leds[i] = CRGB::Black;
    //    strip.setPixelColor(i, 0);
    //strip.show();
    FastLED.show();
    delay(LIGHT_DELAY);
  }
  //strip.clear();
  FastLED.clear();
  //strip.show();
  FastLED.show();
}

static void extinguishNova() {
  Serial.println(F("Extinguish N"));
  playSound("extinguish");
  //  for (uint16_t i = strip.numPixels(); i > 0; i--) {
  for (uint16_t i = N_LEDS-1; i > 0; i--) {
    //      strip.setPixelColor(i + 1, 0);
    leds[i + 1] = CRGB::Black;
    if (i != 0) {
      //            strip.setPixelColor(i, strip.Color(255, 255, 255));
      leds[i] = CRGB::White;
    }
    //strip.show();
    FastLED.show();
    delay(LIGHT_DELAY);
  }
  //strip.clear();
  FastLED.clear();
  //strip.show();
  FastLED.show();
}

static void extinguishUnstable() {
  Serial.println(F("Extinguish U"));
  playSound("extinguish");

  //  for (uint16_t i = strip.numPixels(); i > 0; i--) {
  for (uint16_t i = N_LEDS-1; i > 0; i--) {
    leds[i+1] = CRGB::Black;
//      strip.setPixelColor(i + 1, 0);
    if (i % 4 == 0 && i != 0) {
      leds[i] = CRGB::White;
//      strip.setPixelColor(i, strip.Color(255, 255, 255));
    }
    //strip.show();
    FastLED.show();
    delay(LIGHT_DELAY);
  }
  //strip.clear();
  FastLED.clear();
  //strip.show();
  FastLED.show();
}

void changeColor(int NewColorNumber) {
  if (NewColorNumber == 11 || NewColorNumber == 0) {
    currentColor = 1;
  } else {
    currentColor = NewColorNumber;
  }
  EEPROM.write(0, currentColor);
  int bladeRed, bladeGreen, bladeBlue;
  switch (currentColor) {
    case 1:
//      currentColorText = "Teal";
      bladeRed = 91;
      bladeGreen = 255;
      bladeBlue = 246;
      break;
    case 2:
//      currentColorText = "Blue";
      bladeRed = 0;
      bladeGreen = 0;
      bladeBlue = 255;
      break;
    case 3:
//      currentColorText = "Green";
      bladeRed = 9;
      bladeGreen = 255;
      bladeBlue = 9;
      break;
    case 4:
//      currentColorText = "Red";
      bladeRed = 255;
      bladeGreen = 0;
      bladeBlue = 0;
      break;
    case 5:
//      currentColorText = "Orange";
      bladeRed = 255;
      bladeGreen = 128;
      bladeBlue = 0;
      break;
    case 6:
//      currentColorText = "Purple";
      bladeRed = 148;
      bladeGreen = 0;
      bladeBlue = 255;
      break;
    case 7:
//      currentColorText = "Gold";
      bladeRed = 254;
      bladeGreen = 221;
      bladeBlue = 0;
      break;
    case 8:
//      currentColorText = "Magenta";
      bladeRed = 255;
      bladeGreen = 50;
      bladeBlue = 255;
      break;
    case 9:
//      currentColorText = "Indigo";
      bladeRed = 38;
      bladeGreen = 37;
      bladeBlue = 230;
      break;
    case 10:
//      currentColorText = "White";
      bladeRed = 255;
      bladeGreen = 255;
      bladeBlue = 255;
      break;
    default:
//      currentColorText = "Teal";
      bladeRed = 91;
      bladeGreen = 255;
      bladeBlue = 246;
      break;
  }
  Serial.println(F("Blade Color changed: "));
//  Serial.printlnme(currentColorText);
  bladeColor = CRGB(bladeRed, bladeGreen, bladeBlue);
}

void checkSwing(long GYR) {
  if (GYR > SWING_L_THR && (millis() - LAST_SWING_TIME > 500) && !swinging && !crashing) {
    LAST_SWING_TIME = millis();
    Serial.print(F("SWING"));
    swinging = true;
    idle = false;
    if (GYR >= SWING_THR) {
      Serial.println(F("FAST"));
      playSound("fast swing");
      delay(500);
    } else {
      Serial.println(F("SLOW"));
      playSound("lslow swing");
      delay(500);
    }
  }
}

void checkStrike(long ACC) {
  if (ACC > STRIKE_THR && !crashing) {
    swinging = false;
    crashing = true;
    idle = false;
    Serial.print(F("STRIKE  "));
    if (ACC >= STRIKE_S_THR) {
      Serial.println(F("STRONG"));
      playSound("strong hit");
      hit_flash();
    } else {
      Serial.println(F("WEAK"));
      playSound("weak hit");
      hit_flash();
    }
  }
}

void hit_flash() {
  Serial.println(F("FLASH!"));
  fill_solid(leds, N_LEDS, CRGB::White);
//  strip.fill(strip.Color(255, 255, 255));
  Serial.println(F("ln 439"));
  //strip.show();
  FastLED.show();
  delay(FLASH_DELAY);
fill_solid(leds, N_LEDS, bladeColor);
//strip.show();
  FastLED.show();
  Serial.println(F("ln 498"));
}

void getCompl() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  MPU6050 accelgyro;
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  //-- Scaled and calibrated output:
  // Accel
  long accelX = abs(ax / 100);
  long accelY = abs(ay / 100);
  long accelZ = abs(az / 100);
  // Gyro
  long gyroX = abs(gx / 100);
  long gyroY = abs(gy / 100);
  long gyroZ = abs(gz / 100);
  // Vector Sum
  long ACC = sqrt(sq((long)accelX) + sq((long)accelY) + sq((long)accelZ));
  long GYR = sqrt(sq((long)gyroX) + sq((long)gyroY) + sq((long)gyroZ));
  //COMPL = ACC + GYR;

  //--Display Values
//  Serial.printme("ACC (");
//  Serial.printme(accelX);
//  Serial.printme(",");
//  Serial.printme(accelY);
//  Serial.printme(",");
//  Serial.printme(accelZ);
//  Serial.printme(")  ");
//  Serial.printlnme(ACC);
//  Serial.printme("GYR (");
//  Serial.printme(gyroX);
//  Serial.printme(",");
//  Serial.printme(gyroY);
//  Serial.printme(",");
//  Serial.printme(gyroZ);
//  Serial.printme(")  ");
//  Serial.printlnme(GYR);
//  Serial.printlnme(COMPL);
  delay(500);
  checkStrike(ACC);
  checkSwing(GYR);
}
