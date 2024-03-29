#include <EEPROM.h>
#include <DFPlayerMini_Fast.h>
#include "MPU6050.h"
#include "SoftwareSerial.h"
#include <Adafruit_NeoPixel.h>
#include "Arduino.h"


int16_t ax, ay, az;
int16_t gx, gy, gz;
long ACC, GYR, COMPL;
boolean SWING_ALLOW = true;
unsigned long LAST_SWING_TIME, SWING_TIMER;
boolean idle = false, swinging = false, crashing = false;
int randHum;
int currentColor = 1;
uint32_t bladeColor;
int bladeRed = 65, bladeBlue = 181, bladeGreen = 175;

#define NUM_LEDS 131        // number of microcircuits WS2811 on LED strip (note: one WS2811 controls 3 LEDs!)
#define BTN_TIMEOUT 800     // button hold delay, ms
#define BRIGHTNESS 255      // max LED brightness (0 - 255)

#define SWING_TIMEOUT 500   // timeout between swings

#define SWING_L_THR 150     // swing angle speed threshold
#define SWING_THR 300       // fast swing angle speed threshold

#define STRIKE_THR 27      // hit acceleration threshold
#define STRIKE_S_THR 40    // hard hit acceleration threshold

#define FLASH_DELAY 80      // flash time while hit
#define PULSE_ALLOW 1       // blade pulsation (1 - allow, 0 - disallow)
#define PULSE_AMPL 20       // pulse amplitude
#define PULSE_DELAY 30      // delay between pulses

#define bladePin 6          // Where to connect the blades on the arduino
#define buttonPin 2         // where to connect the button on the arduino
#define voltagePin A6
#define N_LEDS 131

bool isBladeOn = true;

// Create instance
MPU6050 accelgyro;
DFPlayerMini_Fast mp3_play;
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, bladePin, NEO_GRB + NEO_KHZ800);

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

  // Start console
  Serial.begin(115200);
  gyroSetup();
  mp3Setup();
  if ((EEPROM.read(0) >= 1) && (EEPROM.read(0) <= 11)) {
    currentColor = EEPROM.read(0);
  } else {
    EEPROM.write(0, 1);
    currentColor = 1;
  }
  //  neopixelSetup();
  Serial.print("Blade Color: ");
  Serial.println(EEPROM.read(0));
  delay(500);
  playSound("onsaber");
  delay(3000);
  randHum = random(1, 36);
  changeColor(currentColor + 1);
}

void neopixelSetup() {
  strip.begin();
  bladeColor = strip.Color(pgm_read_byte(&gamma8[bladeRed]), pgm_read_byte(&gamma8[bladeGreen]), pgm_read_byte(&gamma8[bladeBlue]));


}

void gyroSetup() {
  // Initial calibration of gyro
  Serial.println("MPU6050 Gyro");
  accelgyro.initialize();
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
  accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  Serial.println("MPU6050 Gyro Initialized");

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

void playSound(String soundType) {
  int randSound = 0;
  switch (soundType[0]) {
    case 'f': //fast swing
      randSound = random(1, 12);
      Serial.print("fast swing noise  ");
      Serial.println(randSound);
      mp3_play.playFolder(4, randSound);
      //      delay()
      break;
    case 'l': //sLow swing
      randSound = random(1, 11);
      Serial.print("slow swing noise  ");
      Serial.println(randSound);
      mp3_play.playFolder(3, randSound);
      break;
    case 's': //strong hit
      randSound = random(1, 9);
      Serial.print("strong hit noise  ");
      Serial.println(randSound);
      mp3_play.playFolder(1, randSound);
      break;
    case 'w': //weak hit
      randSound = random(1, 8);
      Serial.print("weak hit noise  ");
      Serial.println(randSound);
      mp3_play.playFolder(2, randSound);
      break;
    case 'i': //ignite
      randSound = random(1, 85);
      Serial.print("ignite noise  ");
      Serial.println(randSound);
      mp3_play.playFolder(6, randSound);
      break;
    case 'e': //extinguish
      randSound = random(1, 19);
      Serial.print("extinguish noise  ");
      Serial.println(randSound);
      mp3_play.playFolder(7, randSound);
      break;
    case 'o': //on
      Serial.println("saber on noise");
      mp3_play.play(1);
      break;
    case 'p': //power off
      Serial.println("saber off noise");
      mp3_play.play(3);
      break;
    case 'h': //hum
      Serial.print("hum noise  ");
      Serial.println(randHum);
      mp3_play.playFolder(5, randHum);
      break;
defualt:
      Serial.print("hum noise  ");
      Serial.println(randHum);
      mp3_play.playFolder(5, randHum);
      break;
  }
}

void loop() {
  //  if (isBladeOn) {
  //    randomBladePulse();
  getCompl();
  //    checkButton();
  checkStrike();
  checkSwing();
  //    checkBattery();
  //      playSound("default");
  //  }
  //  checkButtonActivate();
  //  testAudio();
  //  playSound("poweroff");
  //  delay(1000);
  swinging = false;
  crashing = false;
  idle = true;
  if (mp3_play.isPlaying() == false) mp3_play.playFolder(5, randHum);

}
//
//void loopHum() {
//  Serial.print("hum noise  ");
//      Serial.println(randHum);
//      mp3_play.loop(mp3_play.playFolder(5, randHum));
//}

void testAudio() {
  playSound("on");
  delay(3000);
  playSound("ignite");
  delay(1000);
  playSound("hum");
  delay(1000);
  playSound("fast swing");
  delay(1000);
  playSound("hum");
  delay(1000);
  playSound("lslow swing");
  delay(1000);
  playSound("hum");
  delay(1000);
  playSound("weak hit");
  delay(1000);
  playSound("hum");
  delay(1000);
  playSound("strong hit");
  delay(1000);
  playSound("hum");
  delay(1000);
  playSound("extinguish");
  delay(1000);
  randHum = random(1, 7);
  playSound("poweroff");
  delay(3000);
}

void testBlade() {}

void changeColor(int NewColorNumber) {
  if (NewColorNumber == 11 || NewColorNumber == 0) {
    currentColor = 1;
  } else {
    currentColor = NewColorNumber;
  }
  switch (currentColor) {
    case 1: //TEAL
      bladeRed = 65;
      bladeGreen = 181;
      bladeBlue = 175;
      break;
    case 2: //BLUE
      bladeRed = 0;
      bladeGreen = 0;
      bladeBlue = 255;
      break;
    case 3: //GREEN
      bladeRed = 6;
      bladeGreen = 150;
      bladeBlue = 6;
      break;
    case 4: //RED
      bladeRed = 255;
      bladeGreen = 0;
      bladeBlue = 0;
      break;
    case 5: //ORANGE
      bladeRed = 255;
      bladeGreen = 128;
      bladeBlue = 0;
      break;
    case 6: //PURPLE
      bladeRed = 75;
      bladeGreen = 0;
      bladeBlue = 130;
      break;
    case 7: //YELLOW/GOLD
      bladeRed = 254;
      bladeGreen = 221;
      bladeBlue = 0;
      break;
    case 8: //MAGENTA
      bladeRed = 255;
      bladeGreen = 50;
      bladeBlue = 255;
      break;
    case 9: //INDIGO
      bladeRed = 38;
      bladeGreen = 37;
      bladeBlue = 230;
      break;
    case 10: //WHITE
      bladeRed = 255;
      bladeGreen = 255;
      bladeBlue = 255;
      break;
    default: //TEAL
      bladeRed = 65;
      bladeGreen = 181;
      bladeBlue = 175;
      break;
  }
  EEPROM.write(0, currentColor);
  Serial.print("Blade Color changed: ");
  Serial.println(currentColor);
}


void checkButtonActivate() {
  //check if the button is pressed to turn blade on
}

void checkSwing() {
  if (GYR > SWING_L_THR && (millis() - LAST_SWING_TIME > 500) && !swinging && !crashing) {
    LAST_SWING_TIME = millis();
    Serial.print("SWING  ");
    swinging = true;
    idle=false;
    if (GYR >= SWING_THR) {
      Serial.println("FAST");
      playSound("fast swing");
      delay(500);
    } else {
      Serial.println("SLOW");
      playSound("lslow swing");
      delay(500);
    }
  }
}

void checkButton() {
  //if button is pressed once, set isBladeOn to false
  //  playSound("extinguish");
  //if button is held, change color
  //  changeColor(currentColor+1);
}

void checkStrike() {
  if (ACC > STRIKE_THR && !crashing) {
    swinging=false;
    crashing=true;
    idle=false;
    Serial.print("STRIKE  ");
    hit_flash();
    if (ACC >= STRIKE_S_THR) {
      Serial.println("STRONG");
      playSound("strong hit");
    } else {
      Serial.println("WEAK");
      playSound("weak hit");
    }
  }
}

void hit_flash() {
  Serial.println("FLASH!");
//  setAll(255, 255, 255);
  delay(FLASH_DELAY);
//  setAll(red, blue, green);
}

void checkBattery() {

}

void randomBladePulse() {

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
