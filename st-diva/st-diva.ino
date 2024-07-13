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
uint16_t thresholds = 0;

uint16_t lasttouched1 = 0;
uint16_t currtouched1 = 0;
uint16_t lasttouched2 = 0;
uint16_t currtouched2 = 0;
uint16_t lasttouched3 = 0;
uint16_t currtouched3 = 0;

int curr1 = 0;
int last1 = 0;
int curr2 = 0;
int last2 = 0;

int touchpoint1 = 0;
int touchpoint2 = 0;

int direction1 = 0;
int direction2 = 0;

void setup() {
  Serial.begin(9600);

  cap1.begin(0x5A);
  delay(300);
  cap2.begin(0x5B);
  delay(300);
  cap3.begin(0x5C);
  delay(300);
  Keyboard.begin();

  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(6, INPUT_PULLUP);  
  pinMode(7, INPUT_PULLUP);  
  pinMode(8, INPUT_PULLUP);  
  pinMode(9, INPUT_PULLUP);  

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

  //Last = Curr
  last1 = curr1;
  last2 = curr2;
  curr1 = 0;
  curr2 = 0;
  //Itering touch status
  int tmp = 0;
  int j = 0;
  bool reverse = 0;
  bool iftouched = false;
  for(int i=0; i<36; i++){
    if(touchpad[i]){
      iftouched = true;
      if(touchpoint1 == 0){
        touchpoint1 = i;
        j = i;
        break;
      }
      else if(touchpoint2 == 0){
        if(abs(touchpoint1 - i) <= 2){
          touchpoint1 = i;
          j = i;
          break;
        }
        else{
          touchpoint2 = i;
          j = i;
          break;
        }
      }
      else{
        if(abs(i-touchpoint2)<=2){
          touchpoint2 = i;
          j = i;
          reverse = 1;
          break;
        }
        else{
          touchpoint1 = i;
          j = i;
          break;
        }
      }
    }
  }

  if(j != 0){
    for(int i = j+10; i<36; i++){
      if(touchpad[i]){
        if(touchpoint2 == 0){
          touchpoint2 = i;
          break;
        }
        else{
          if(abs(i - touchpoint1) <= 2){
            reverse = 1;
            touchpoint1 = i;
            break;
          }
          else{
            touchpoint2 = i;
            break;
          }
        }
      }
    }
  }
#ifdef DEBUG
/*
  Serial.print("touchpoint1 = ");
  Serial.println(touchpoint1);
  Serial.print("touchpoint2 = ");
  Serial.println(touchpoint2);
*/

#endif


 //direction detection
  if(!iftouched){
    touchpoint1 = 0;
    touchpoint2 = 0;
    direction1 = -2;
    direction2 = -2;
  }


#ifndef DEBUG

  else if(touchpoint2 == 0){
    curr1 = touchpoint1;
    direction1 = slideDetect(curr1, last1);
    direction2 = -2;
  }
  else{
    curr1 = touchpoint1;
    curr2 = touchpoint2;
    direction1 = slideDetect(curr1, last1);
    direction2 = slideDetect(curr2, last2);
  }

  //Slide status
  if(!iftouched){
    Keyboard.release('e');
    Keyboard.release('q');
    Keyboard.release('u');
    Keyboard.release('o');
  }
  else{
    switch(direction1){
      case -2:{
        Keyboard.release('e');
        Keyboard.release('q');
        break;
      }
      case -1:{
        Keyboard.press('q');
        Keyboard.release('e');
        break;
      }
      case 1:{
        Keyboard.press('e');
        Keyboard.release('q');
        break; 
      }
      case 0:{
        break;
      }
    }
    switch(direction2){
      case -2:{
        Keyboard.release('u');
        Keyboard.release('o');
        break;
      }
      case -1:{
        Keyboard.press('u');
        Keyboard.release('o');
        break;
      }
      case 1:{
        Keyboard.press('o');
        Keyboard.release('u');
        break; 
      }
      case 0:{
        break;
      }
    }
  }

  //ButtonInput
  buttonInput();

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

int slideDetect(int x, int y){
  //Serial.print("Direction: ");
  if(x + y == 0){
    return -2;
  }
  else if(x - y > 0 && x - y < 2){
    return 1;
  }
  else if(x - y < 0 && x - y > -2){
    return -1;
  }
  else{
    return 0;
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

void buttonInput(){
  if(digitalRead(6) == HIGH){
    digitalWrite(18, LOW);
    Keyboard.release('w');
  }
  else{
    digitalWrite(18, HIGH);
    Keyboard.press('w');
  }

  if(digitalRead(7) == HIGH){
    digitalWrite(19, LOW);
    Keyboard.release('a');
  }
  else{
    digitalWrite(19, HIGH);
    Keyboard.press('a');
  }

  if(digitalRead(8) == HIGH){
    digitalWrite(20, LOW);
    Keyboard.release('s');
  }
  else{
    digitalWrite(20, HIGH);
    Keyboard.press('s');
  }

  if(digitalRead(9) == HIGH){
    digitalWrite(21, LOW);
    Keyboard.release('d');
  }
  else{
    digitalWrite(21, HIGH);
    Keyboard.press('d');
  }
  Serial.println("  ");
}