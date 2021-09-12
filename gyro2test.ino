/*
 Get scaled and calibrated output of MPU6050
 */

#include "MPU6050.h"

int16_t ax, ay, az;
int16_t gx, gy, gz;
long ACC, GYR, COMPL;
boolean swing_allow = true;
unsigned long last_swing_time, swing_timer;


#define swing_timeout 500
#define swing_low_thr 150
#define swing_thr 300

// Create instance
MPU6050 accelgyro;

void setup() {
  // Initial calibration of gyro
  accelgyro.initialize();
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
  accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  // Start console
  Serial.begin(9600);
}

void loop() {
  randomBladePulse(); 
  getCompl();
  checkButton();
  checkStrike();
  checkSwing();
  checkBattery();
}

void checkSwing() {
  if(GYR > 80 && (millis() - last_swing_time > 100)) {
    last_swing_time = millis();
    Serial.print("SWING  ");    
    if(((millis() - swing_timer) > swing_timeout) && swing_allow) {
      int randSound = random(5);
      if(GYR >= swing_thr) {
        Serial.println("HIGH");
        //play swingsound[randSound]
      } else {
        Serial.println("LOW");
        //play swingLsound[randSound]
      }
    //no swinging
    swing_allow = false;
    swing_timer = millis(); 
    }
  }
}

void checkButton() {

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
  long accelX = abs(ax/100);
  long accelY = abs(ay/100);
  long accelZ = abs(az/100);
  // Gyro
  long gyroX = abs(gx/100);
  long gyroY = abs(gy/100);
  long gyroZ = abs(gz/100);
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
