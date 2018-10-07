#include <Arduino.h>
#include <SoftwareSerial.h>
#include <JQ6500_Serial.h>

JQ6500_Serial mp3(PIN3,PIN4);

void setup() {
  mp3.begin(9600);
  mp3.reset();
  mp3.setVolume(20);
  mp3.setLoopMode(MP3_LOOP_ONE);
  mp3.playFileByIndexNumber(2);
}

void loop() {
  //if (mp3.getStatus() != MP3_STATUS_PLAYING) {
    //mp3.playFileByIndexNumber(1);  
  //}
}

