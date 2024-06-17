#include <Keyboard.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

Adafruit_MPR121 cap1 = Adafruit_MPR121();
Adafruit_MPR121 cap2 = Adafruit_MPR121();
Adafruit_MPR121 cap3 = Adafruit_MPR121();

bool l_direction = 0;
bool r_direction = 0;


int currtouched1 = 0;
int lasttouched1 = 0;

int currtouched2 = 0;
int lasttouched2 = 0;

int currtouched3 = 0;
int lasttouched3 = 0;

int r_touch1 = 0;
int l_touch2 = 0;
int r_touch2 = 0;


bool touchpad[36] = {0};
int touch_status[4] = {0};

void setup() {
  Serial.begin(9600);
  cap1.begin(0x5A);
  cap2.begin(0x5B);
  cap3.begin(0x5C);
  Keyboard.begin();
}


void loop() {
  touchpadReader();

//Touch status array generate;
  uint16_t j = 0;
  for(uint16_t i=0; i<36; i++){
    if(touchpad[i]){
      if(j<4){
        touch_status[j] = i;
        j++;
      }
    }
  }

//Touch status array dealing
  for(uint16_t i=0; i<4; i++){
    Serial.print(touch_status[i]);
    Serial.print("      ");
  }
  Serial.println();

//Initialize touch status array
  for(uint16_t i=0; i<4; i++){
    touch_status[i] = 0;
  }
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