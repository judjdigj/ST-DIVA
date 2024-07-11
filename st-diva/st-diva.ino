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

uint16_t lasttouched1 = 0;
uint16_t currtouched1 = 0;
uint16_t lasttouched2 = 0;
uint16_t currtouched2 = 0;
uint16_t lasttouched3 = 0;
uint16_t currtouched3 = 0;

int curr = 0;
int last = 0;

void setup() {
  Serial.begin(9600);
  cap1.begin(0x5A);
  cap2.begin(0x5B);
  cap3.begin(0x5C);
  Keyboard.begin();
  delay(1000);
///  touchCalibrate();
}


void loop() {
  touchpadReader();

  #ifdef DEBUG
  for(uint16_t i=0; i<36; i++){
    Serial.print(touchpad[i]);
  }
  Serial.println(" ");
  #endif

  #ifndef DEBUG
  //Last = Curr
  last = curr;
  curr = 0;

  //Itering touch status
  for(uint16_t i=0; i<36; i++){
    if(touchpad[i]){
      curr = i;  //4 point maximium
      iftouched = 1;
    }
  }
 //direction detection
  if(iftouched){
    slideDetect(curr, last);
  }

  //Slide status

  #endif
}

void touchpadReader(){


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

void slideDetect(int x, int y){
  Serial.print("Direction: ");
  if(x + y == 0){
    Keyboard.releaseAll();
  }
  else if(x - y == 1){
    Keyboard.press('e');
    Keyboard.release('q');
  }
  else if(x - y == -1){
    Keyboard.press('q');
    Keyboard.release('e');
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