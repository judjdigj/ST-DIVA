#include <Keyboard.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"

//#define DEBUG

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

Adafruit_MPR121 cap1 = Adafruit_MPR121();
Adafruit_MPR121 cap2 = Adafruit_MPR121();
Adafruit_MPR121 cap3 = Adafruit_MPR121();

bool direction_1 = 0;
bool direction_2 = 0;

int curr_1[2] = {0};
int curr_2[2] = {0};
int last_1[2] = {0};
int last_2[2] = {0};

unsigned long lastTask1Time = 0;
unsigned long lastTask2Time = 0;

bool touchpad[36] = {0};
int touch_status[4] = {0};
bool iftouched = 0;
uint16_t thresholds = 0;

void setup() {
  Serial.begin(9600);
  cap1.begin(0x5A);
  cap2.begin(0x5B);
  cap3.begin(0x5C);
  Keyboard.begin();
  touchCalibrate();
}


void loop() {
  touchpadReaderRAW();

  #ifdef DEBUG
  for(uint16_t i=0; i<36; i++){
    Serial.print(touchpad[i]);
  }
  Serial.println(" ");
  #endif

  #ifndef DEBUG
  //Last = Curr
  for(uint16_t i=0; i<2; i++){
    last_1[i] = curr_1[i];
    last_2[i] = curr_2[i];
    curr_1[i] = 0;
    curr_2[i] = 0;
  }

  //Itering touch status
  uint16_t j=0;
  iftouched = 0;
  for(uint16_t i=0; i<36; i++){
    if(touchpad[i]){
      if(j<4){
        touch_status[j] = i;  //4 point maximium
        j++;
        iftouched = 1;
      }
    }
  }

  if(!iftouched){
    for(uint16_t i=0; i<4; i++){
      touch_status[i] = 0;
    }
  }

  //Storage touched sensor into batch
  for(uint16_t i=0; i<2; i++){
    curr_1[i] = touch_status[i];
    curr_2[i] = touch_status[i+2];
  }
  //Slide status


  slideDetect(curr_1[0], last_1[0], curr_1[1], last_1[1]);

  #endif
}

void touchpadReader(){
  uint16_t lasttouched1 = 0;
  uint16_t currtouched1 = 0;
  uint16_t lasttouched2 = 0;
  uint16_t currtouched2 = 0;
  uint16_t lasttouched3 = 0;
  uint16_t currtouched3 = 0;


  lasttouched1 = currtouched1;
  currtouched1 = cap1.touched();
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched1 & _BV(i)) && !(lasttouched1 & _BV(i)) ) {
      touchpad[i] = 1;
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched1 & _BV(i)) && (lasttouched1 & _BV(i)) ) {
      touchpad[i] = 0;
    }
  }

  lasttouched2 = currtouched2;
  currtouched2 = cap2.touched();
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched2 & _BV(i)) && !(lasttouched2 & _BV(i)) ) {
      touchpad[i+12] = 1;
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched2 & _BV(i)) && (lasttouched2 & _BV(i)) ) {
      touchpad[i+12] = 0;
    }
  }

  lasttouched3 = currtouched3;
  currtouched3 = cap3.touched();
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched3 & _BV(i)) && !(lasttouched3 & _BV(i)) ) {
      touchpad[i+24] = 1;
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched3 & _BV(i)) && (lasttouched3 & _BV(i)) ) {
      touchpad[i+24] = 0;
    }
  }
}

void touchpadReaderRAW(){
  for(uint8_t i=0; i<12; i++){
    if(cap1.filteredData(i) > thresholds){
      touchpad[i] = 0;
    }
    else{
      touchpad[i] = 1;
    }
  }
  for (uint8_t i=0; i<12; i++){
    if(cap2.filteredData(i) > thresholds){
      touchpad[i+12] = 0;
    }
    else{
      touchpad[i+12] = 1;
    }
  }
  for (uint8_t i=0; i<12; i++){
    if(cap3.filteredData(i) > thresholds){
      touchpad[i+24] = 0;
    }
    else{
      touchpad[i+24] = 1;
    }
  }
}

void slideDetect(int x, int y, int x1, int y1){
  if(x + y + x1 + y1 == 0){
    Keyboard.releaseAll();
  }
  else if(x - y == 1 && x1 - y1 == 1){
    Keyboard.release('q');
    Keyboard.press('e');
  }
  else if(x - y == -1 && x1 - y1 == -1){
    Keyboard.release('e');
    Keyboard.press('q');
  }
}

void touchCalibrate(){
  uint16_t total = 0;
  for(uint16_t j=0; j<3; j++){
    for(uint16_t i=0; i<12; i++){
      total = total + cap1.filteredData(i);
    }
  }
  thresholds = total/36 - 40;
}