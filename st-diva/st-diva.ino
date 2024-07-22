#include <XInput.h>

#include <Adafruit_NeoPixel.h>

#include <Wire.h>
#include "Adafruit_MPR121.h"

//#define DEBUG

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define LED_PIN    0
#define LED_COUNT 30

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t magenta = strip.Color(255, 0, 255);
uint32_t black = strip.Color(0, 0, 0);


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

bool touchpad[30] = {0};
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

  pinMode(6, INPUT_PULLUP); 
  pinMode(7, INPUT_PULLUP); 
  pinMode(8, INPUT_PULLUP); 
  pinMode(9, INPUT_PULLUP); 

  cap1.begin(0x5A);
  cap2.begin(0x5B);
  cap3.begin(0x5C);
  XInput.begin();
  delay(500);

  strip.begin();
  strip.show();
  strip.setBrightness(64);
  for(int i=29; i>=0;i--){
    strip.setPixelColor(i, magenta);
    strip.show();
    delay(30);
  }
  delay(1000);
  strip.fill(black, 0, 30);

///  touchCalibrate();
}


void loop() {
  touchpadReader();

  #ifdef DEBUG
  for(uint16_t i=0; i<30; i++){
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
  for(int i=0; i<30; i++){
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
    for(int i = j+10; i<30; i++){
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
    XInput.release(TRIGGER_RIGHT);
    XInput.release(TRIGGER_LEFT);
    XInput.release(BUTTON_RB);
    XInput.release(BUTTON_LB);
  }
  else{
    switch(direction1){
      case -2:{
        XInput.release(BUTTON_RB);
        XInput.release(BUTTON_LB);
        break;
      }
      case -1:{
        XInput.press(BUTTON_LB);
        XInput.release(BUTTON_RB);
        break;
      }
      case 1:{
        XInput.press(BUTTON_RB);
        XInput.release(BUTTON_LB);
        break; 
      }
      case 0:{
        break;
      }
    }
    switch(direction2){
      case -2:{
        XInput.release(TRIGGER_RIGHT);
        XInput.release(TRIGGER_LEFT);
        break;
      }
      case -1:{
        XInput.press(TRIGGER_LEFT);
        XInput.release(TRIGGER_RIGHT);
        break;
      }
      case 1:{
        XInput.press(TRIGGER_RIGHT);
        XInput.release(TRIGGER_LEFT);
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
      strip.setPixelColor(29-i, magenta);
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched1 & _BV(i)) && (lasttouched1 & _BV(i)) ) {
      touchpad[i] = 0;
      strip.setPixelColor(29-i, black);
    }
  }

  lasttouched2 = currtouched2;
  currtouched2 = cap2.touched();
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched2 & _BV(i)) && !(lasttouched2 & _BV(i)) ) {
      touchpad[i+12] = 1;
      strip.setPixelColor(17-i, magenta);
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched2 & _BV(i)) && (lasttouched2 & _BV(i)) ) {
      touchpad[i+12] = 0;
      strip.setPixelColor(17-i, black);
    }
  }

  lasttouched3 = currtouched3;
  currtouched3 = cap3.touched();
  for (uint8_t i=0; i<6; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched3 & _BV(i)) && !(lasttouched3 & _BV(i)) ) {
      touchpad[i+24] = 1;
      strip.setPixelColor(5-i, magenta);
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched3 & _BV(i)) && (lasttouched3 & _BV(i)) ) {
      touchpad[i+24] = 0;
      strip.setPixelColor(5-i, black);
    }
  }
  strip.show();
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

void buttonInput(){
  if(digitalRead(9) == HIGH){
    XInput.release(BUTTON_Y);
  }
  else{
    XInput.press(BUTTON_Y);
  }

  if(digitalRead(8) == HIGH){
    XInput.release(BUTTON_X);
  }
  else{
    XInput.press(BUTTON_X);
  }

  if(digitalRead(7) == HIGH){
    XInput.release(BUTTON_A);
  }
  else{
    XInput.press(BUTTON_A);
  }

  if(digitalRead(6) == HIGH){
    XInput.release(BUTTON_B);
  }
  else{
    XInput.press(BUTTON_B);
  }
}