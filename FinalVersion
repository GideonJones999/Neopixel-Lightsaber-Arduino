#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include "SoftwareSerial.h"
#include <DFPlayerMini_Fast.h>
#include "MPU6050.h"



int16_t ax, ay, az;
int16_t gx, gy, gz;
long ACC, GYR, COMPL;
#define FLASH_DELAY 80      // flash time while hit

#define PIN    6
#define N_LEDS 10
#define bladeCheckPin 8
#define buttonPin 4
#define BTN_TIMEOUT 800     // button hold delay, ms
int bladeIgnitionTime = 100;
boolean btnState, btn_flag, hold_flag;
int lightDelay = bladeIgnitionTime / N_LEDS;
int currentColor = 1;
int bladeRed, bladeGreen, bladeBlue;
String currentColorText;
uint32_t bladeColor;
bool isBladeActivated = false;
bool isBladeIn = false;
unsigned long btn_timer;
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFPlayerMini_Fast mp3_play;
Adafruit_NeoPixel strip(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
MPU6050 accelgyro;


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
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};


void setup() {
  Serial.begin(19200);
  pinMode(bladeCheckPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  neoPixelSetup();
  mp3Setup();
  Serial.println("Startup completed");
  playSound("on");
}

void mp3Setup() {
  Serial.println(F("DFRobot DFPlayer Mini Setup"));
  mySoftwareSerial.begin(9600);
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!mp3_play.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));

  mp3_play.volume(30);  //Set volume value. From 0 to 30
}

void neoPixelSetup() {
  if ((EEPROM.read(0) >= 1) && (EEPROM.read(0) <= 11)) {
    currentColor = EEPROM.read(0);
  } else {
    EEPROM.write(0, 1);
    currentColor = 1;
  }
  strip.begin();
  strip.setBrightness(25);
  strip.clear();
  changeColor(currentColor);
  bladeColor = strip.Color(pgm_read_byte(&gamma8[bladeRed]), pgm_read_byte(&gamma8[bladeGreen]), pgm_read_byte(&gamma8[bladeBlue]));
  //  checkBattery();
}

void loop() {
  if (isBladeActivated) {
    checkButton();
    if (isBladeIn) {
      getCompl();
//      checkStrike();
//      checkSwing();
      //check other things, like clash, swing, etc.
//      if (mp3_play.isPlaying() == false) {
//        playSound("hum");
//      }

    }
    else {
      strip.fill(bladeColor);
      strip.show();
    }
  } else {
    checkButton();
    strip.fill(0, 0, 0);
    strip.show();
  }
}

void playSound(String soundType) {
  switch (soundType[0]) {
    case 'f': //fast swing
      Serial.print("fast swing noise  ");
      mp3_play.playFolder(5, 1);
      break;
    case 'l': //sLow swing
      Serial.print("slow swing noise  ");
      mp3_play.playFolder(4, 1);
      break;
    case 's': //strong hit
      Serial.print("strong hit noise  ");
      mp3_play.playFolder(7, 1);
      break;
    case 'w': //weak hit
      Serial.print("weak hit noise  ");
      mp3_play.playFolder(6, 1);
      break;
    case 'i': //ignite
      Serial.print("ignite noise  ");
      mp3_play.playFolder(1, 1);
      break;
    case 'e': //extinguish
      Serial.print("extinguish noise  ");
      mp3_play.playFolder(2, 1);
      break;
    case 'o': //on
      Serial.println("saber on noise");
      mp3_play.play(1);
      break;
    case 'p': //power off
      Serial.println("saber off noise");
      mp3_play.play(2);
      break;
    case 'h': //hum
      Serial.println("hum noise  ");
      mp3_play.playFolder(3, 1);
      break;
    case 'm': //misfire
      Serial.println("Saber misfire");
      mp3_play.play(3);
      break;
    case 'c': //change color
      Serial.println("Color Change Sound");
      mp3_play.play(4);
      break;
defualt:
      Serial.print("hum noise  ");
      mp3_play.playFolder(3, 1);
      break;
  }
}

static void checkButton() {
  delay(100);
  btnState = !digitalRead(buttonPin);
  if (btn_flag && btnState && (millis() - btn_timer > BTN_TIMEOUT) && !hold_flag) {
    checkBladeThere();
    Serial.println("Button Held!");
    hold_flag = 1;
    if (isBladeActivated) {
      if (isBladeIn) {
        extinguishNova();
      }
      else {
        changeHiltLight(strip.Color(0, 0, 0));
      }
    }
    delay(500);
    changeColorAudio(currentColor + 1);

    if (isBladeIn) {
      igniteNova(bladeColor);
    }
    else {
      changeHiltLight(bladeColor);
    }
  }
  else if (btnState && !btn_flag) {
    checkBladeThere();
    Serial.println("Button Pressed!");
    btn_flag = 1;
    btn_timer = millis();
    if (isBladeActivated) {
      if (isBladeIn) {
        extinguishNova();
      }
      else {
        changeHiltLight(strip.Color(0, 0, 0));
      }
      isBladeActivated = false;
    }
    else {
      if (isBladeIn) {
        igniteNova(bladeColor);
      }
      else {
        changeHiltLight(bladeColor);
      }
      isBladeActivated = true;
    }
    checkBladeThere();

  }
  else if (!btnState && btn_flag) {
    btn_flag = 0;
    hold_flag = 0;
  }

}

static void changeColorAudio (int newColorNumb) {
  playSound("change");
  Serial.println("playingAudio");
  delay(2500);
  changeColor(currentColor + 1);
}

static void changeHiltLight(uint32_t c) {
  playSound("misfire");
  isBladeActivated = !isBladeActivated;
  Serial.print("change Hilt Light. Blade is ");
  if (isBladeActivated) {
    Serial.println("on");
  }
  else {
    Serial.println("off");
  }
  strip.fill(c);
  strip.show();
}


static void checkBladeThere() {
  isBladeIn = digitalRead(bladeCheckPin);
  if (isBladeIn == 1) {
    Serial.println("Blade is in");
  } else {
    Serial.println("Blade is out");
  }
}

//static void chase(uint32_t c) {
//  Serial.print("Chase: ");
//  Serial.println(c);
//  for (int i = 0; i < strip.numPixels(); i++) {
//    strip.setPixelColor(i, c); // Draw new pixel
//    strip.setPixelColor(i - 2, 0); // Erase pixel a few steps back
//    strip.show();
//    delay(lightDelay);
//  }
//}

static void ignite(uint32_t c) {
  Serial.println("Ignition");
  playSound("ignite");
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i  , c); // Draw new pixel
    strip.show();
    delay(lightDelay);
  }
}

static void igniteNova(uint32_t c) {
  Serial.println("Ignition N");
  playSound("ignite");
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    if (i != strip.numPixels()) {
      strip.setPixelColor(i + 1, strip.Color(255, 255, 255));
    }
    strip.setPixelColor(i  , c); // Draw new pixel
    strip.show();
    delay(lightDelay);
  }
}

static void igniteUnstable(uint32_t c) {
  Serial.println("Ignition U");
  playSound("ignite");
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    if (i != strip.numPixels() && i % 4 == 0) {
      strip.setPixelColor(i + 1, strip.Color(255, 255, 255));
    }
    strip.setPixelColor(i  , c); // Draw new pixel
    strip.show();
    delay(lightDelay);
  }
}

static void extinguish() {
  Serial.println("Extinguish");
  playSound("extinguish");
  for (uint16_t i = strip.numPixels(); i > 0; i--) {
    strip.setPixelColor(i, 0);
    strip.show();
    delay(lightDelay);
  }
  strip.clear();
  strip.show();
}

static void extinguishNova() {
  Serial.println("Extinguish N");
  playSound("extinguish");
  for (uint16_t i = strip.numPixels(); i > 0; i--) {
    if (i != 0) {
      strip.setPixelColor(i + 1, 0);
    }
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.show();
    delay(lightDelay);
  }
  strip.clear();
  strip.show();
}

static void extinguishUnstable() {
  Serial.println("Extinguish U");
  playSound("extinguish");

  for (uint16_t i = strip.numPixels(); i > 0; i--) {
    if (i != 0) {
      strip.setPixelColor(i + 1, 0);
    }
    if (i % 4 == 0) {
      strip.setPixelColor(i, strip.Color(255, 255, 255));
    }
    strip.show();
    delay(lightDelay);
  }
  strip.clear();
  strip.show();
}

void changeColor(int NewColorNumber) {
  if (NewColorNumber == 11 || NewColorNumber == 0) {
    currentColor = 1;
  } else {
    currentColor = NewColorNumber;
  }
  EEPROM.write(0, currentColor);
  switch (currentColor) {
    case 1:
      currentColorText = "Teal";
      bladeRed = 91;
      bladeGreen = 255;
      bladeBlue = 246;
      break;
    case 2:
      currentColorText = "Blue";
      bladeRed = 0;
      bladeGreen = 0;
      bladeBlue = 255;
      break;
    case 3:
      currentColorText = "Green";
      bladeRed = 9;
      bladeGreen = 255;
      bladeBlue = 9;
      break;
    case 4:
      currentColorText = "Red";
      bladeRed = 255;
      bladeGreen = 0;
      bladeBlue = 0;
      break;
    case 5:
      currentColorText = "Orange";
      bladeRed = 255;
      bladeGreen = 128;
      bladeBlue = 0;
      break;
    case 6:
      currentColorText = "Purple";
      bladeRed = 148;
      bladeGreen = 0;
      bladeBlue = 255;
      break;
    case 7:
      currentColorText = "Gold";
      bladeRed = 254;
      bladeGreen = 221;
      bladeBlue = 0;
      break;
    case 8:
      currentColorText = "Magenta";
      bladeRed = 255;
      bladeGreen = 50;
      bladeBlue = 255;
      break;
    case 9:
      currentColorText = "Indigo";
      bladeRed = 38;
      bladeGreen = 37;
      bladeBlue = 230;
      break;
    case 10:
      currentColorText = "White";
      bladeRed = 255;
      bladeGreen = 255;
      bladeBlue = 255;
      break;
    default:
      currentColorText = "Teal";
      bladeRed = 91;
      bladeGreen = 255;
      bladeBlue = 246;
      break;
  }
  Serial.print("Blade Color changed: ");
  Serial.println(currentColorText);
  bladeColor = strip.Color(pgm_read_byte(&gamma8[bladeRed]), pgm_read_byte(&gamma8[bladeGreen]), pgm_read_byte(&gamma8[bladeBlue]));
}

void checkSwing() {

}

void checkStrike() {

}

void hit_flash() {
  Serial.println("FLASH!");
  strip.fill(strip.Color(255, 255, 255));
  strip.show();
  delay(FLASH_DELAY);
  strip.fill(bladeColor);
}

void getCompl() {
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
  ACC = sqrt(sq((long)accelX) + sq((long)accelY) + sq((long)accelZ));
  GYR = sqrt(sq((long)gyroX) + sq((long)gyroY) + sq((long)gyroZ));
  COMPL = ACC + GYR;

  //--Display Values
  Serial.print("ACC (");
  Serial.print(accelX);
  Serial.print(",");
  Serial.print(accelY);
  Serial.print(",");
  Serial.print(accelZ);
  Serial.print(")  ");
  Serial.println(ACC);
  Serial.print("GYR (");
  Serial.print(gyroX);
  Serial.print(",");
  Serial.print(gyroY);
  Serial.print(",");
  Serial.print(gyroZ);
  Serial.print(")  ");
  Serial.println(GYR);
  Serial.println(COMPL);
  delay(500);
}

