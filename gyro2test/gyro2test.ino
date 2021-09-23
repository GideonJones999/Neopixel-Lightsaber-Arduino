/*
  Get scaled and calibrated output of MPU6050
*/

#include "MPU6050.h"
#include "DFRobotDFPlayerMini.h"
#include "SoftwareSerial.h"
#include <Adafruit_NeoPixel.h>
#include "Arduino.h"


int16_t ax, ay, az;
int16_t gx, gy, gz;
long ACC, GYR, COMPL;
boolean SWING_ALLOW = true;
unsigned long LAST_SWING_TIME, SWING_TIMER;
boolean idle = false, swinging = false, crashing = false;

#define NUM_LEDS 131        // number of microcircuits WS2811 on LED strip (note: one WS2811 controls 3 LEDs!)
#define BTN_TIMEOUT 800     // button hold delay, ms
#define BRIGHTNESS 255      // max LED brightness (0 - 255)

#define SWING_TIMEOUT 500   // timeout between swings

#define SWING_L_THR 150     // swing angle speed threshold
#define SWING_THR 300       // fast swing angle speed threshold

#define STRIKE_THR 150      // hit acceleration threshold
#define STRIKE_S_THR 320    // hard hit acceleration threshold

#define FLASH_DELAY 80      // flash time while hit
#define PULSE_ALLOW 1       // blade pulsation (1 - allow, 0 - disallow)
#define PULSE_AMPL 20       // pulse amplitude
#define PULSE_DELAY 30      // delay between pulses

#define bladePin 6          // Where to connect the blades on the arduino
#define buttonPin 2         // where to connect the button on the arduino
#define voltagePin A6

bool isBladeOn = true;

// Create instance
MPU6050 accelgyro;
DFRobotDFPlayerMini mp3_play;
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

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

#define Teal strip.Color(pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]))
#define Blue strip.Color(pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]));
#define Green strip.Color(pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]));
#define Red strip.Color(pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]));
#define Orange strip.Color(pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]));
#define Purple strip.Color(pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]));
#define Yellow strip.Color(pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]));
#define Magenta strip.Color(pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]));
#define Indigo strip.Color(pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]));
#define White strip.Color(pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]),pgm_read_byte(&gamma8[]));

void setup() {
  // Start console
  Serial.begin(115200);
  gyroSetup();
  mp3Setup();
  //  neopixelSetup();
  playSound("ignite");
}

void gyroSetup() {
  // Initial calibration of gyro
  accelgyro.initialize();
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
  accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
}

void mp3Setup() {
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
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
  switch (soundType[0]) {
    case 'h': //high swing
      Serial.println("high swing noise");
            mp3_play.play(1);

      break;
    case 'l': //low swing
      Serial.println("low swing noise");
            mp3_play.play(1);

      break;
    case 's': //strong hit
      Serial.println("strong hit noise");
            mp3_play.play(1);
//          play(4 through 12)
      break;
    case 'w': //weak hit
      Serial.println("weak hit noise");
            mp3_play.play(1);
//            play(13 through 21) 
      break;
    case 'i': //ignite
      Serial.println("ignite noise");
            mp3_play.play(1);
//            play(3)
      break;
    case 'e': //extinguish
      Serial.println("extinguish noise");
            mp3_play.play(1);
//            play(2)
      break;
     case 'o': //on
      Serial.println("saber on");
            mp3_play.play(1);
//            play()
      break;
defualt:
      Serial.println("hum noise");
      mp3_play.play(1);
      break;
  }
}

void loop() {
//  if (isBladeOn) {
//    randomBladePulse();
    getCompl();
//    checkButton();
//    checkStrike();
//    checkSwing();
//    checkBattery();
//    playSound("default");
//  }
//  checkButtonActivate();
}

void checkButtonActivate() {
  //check if the button is pressed to turn blade on  
}

void checkSwing() {
  if (GYR > SWING_L_THR && (millis() - LAST_SWING_TIME > 500) && !swinging) {
    LAST_SWING_TIME = millis();
    Serial.print("SWING  ");
    idle = false;
    swinging = true;
    if (GYR >= SWING_THR) {
      Serial.println("HIGH");
      playSound("highswing");
    } else {
      Serial.println("LOW");
      playSound("lowswing");
    }
  }
}

void checkButton() {
  //if button is pressed once, set isBladeOn to false
  //playSound("extinguish");
  //if button is held, change color 
}

void checkStrike() {

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
