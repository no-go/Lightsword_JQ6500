#include <Wire.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <JQ6500_Serial.h>


#define ON_SOUND    5
#define IDLE_SOUND  3
#define SWING_SOUND 1
#define HIT_SOUND   2
#define OFF_SOUND   4


const double ACTON_SWING  = 40000;
const double ACTON_HIT    = 54000;

JQ6500_Serial mp3(PIN3,PIN4);

// I2C address of the MPU-6050 (SDA=PIN0 (A4), SCL=PIN2 (A5))
const int MPU=0x68;

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double val;
byte loops=0;

int modus = 0;
int buttonState = 0;

// analog LED pins
int pins[] = {5,6,9,10,11};

void laserOn() {
  for (int i=0;i<5;i++) {
    for (int x = 0; x<=180; x+=10) {
      analogWrite(pins[i], x);
      delay(4);
    }
  }
}

void laserOff() {
  for (int i=4;i>=0;i--) {
    for (int x = 180; x>=0; x-=20) {
      analogWrite(pins[i], x);
      delay(4);
    }
  }
}

void setup() {
  pinMode(8, INPUT_PULLUP);

  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);

  mp3.begin(9600);
  mp3.reset();
  mp3.setVolume(20);
}

void loop() {
  loops++;
  buttonState = digitalRead(8);
  delay(5);

  if (loops%3==0 && modus>0) {
    // measure because we may woop
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 14, true); // we need to read all 6 registers, really!
  
    GyX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
    GyY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    AcX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    AcY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    if (AcX <0) AcX*=-1;
    if (AcY <0) AcY*=-1;
    if (AcZ <0) AcZ*=-1;
    val = ((double)AcX+(double)AcY+(double)AcZ);
    if (val<0) val*=-1;
    //Serial.println(val);

    analogWrite(pins[2], 110+val/500);
    analogWrite(pins[4], 110+val/500);
  }

  if (modus == 0 && buttonState == LOW) {
    mp3.setLoopMode(MP3_LOOP_ONE_STOP);
    laserOn();
    mp3.playFileByIndexNumber(ON_SOUND);
    delay(900);
    mp3.setLoopMode(MP3_LOOP_ONE);
    mp3.playFileByIndexNumber(IDLE_SOUND);
    modus = 1;
  } else if ((modus == 1 || modus == 2 || modus == 3) && buttonState == LOW) {
    mp3.setLoopMode(MP3_LOOP_ONE_STOP);
    mp3.playFileByIndexNumber(OFF_SOUND);
    laserOff();
    delay(800);
    modus = 0;
  }

  if (modus==1) {
    if(val > ACTON_HIT) {
      modus=3;
      mp3.setLoopMode(MP3_LOOP_ONE_STOP);
      for (int i=0;i<5;i++) {
        digitalWrite(pins[i], HIGH);
      }
      mp3.playFileByIndexNumber(HIT_SOUND);
      delay(100);
      for (int i=0;i<5;i++) {
        analogWrite(pins[i], 180);
      }
      delay(400);
    } else if(val > ACTON_SWING) {
      modus=2;
      mp3.playFileByIndexNumber(SWING_SOUND);
    }  
  } else if (modus==2 || modus==3) {
    if(val < ACTON_SWING) {
      mp3.setLoopMode(MP3_LOOP_ONE);
      mp3.playFileByIndexNumber(IDLE_SOUND);
      modus=1;
    }  
  }

  
}

