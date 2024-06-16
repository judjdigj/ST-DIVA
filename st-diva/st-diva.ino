#include <Keyboard.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"
Adafruit_MPR121 cap1 = Adafruit_MPR121();
Adafruit_MPR121 cap2 = Adafruit_MPR121();
Adafruit_MPR121 cap3 = Adafruit_MPR121();

int lasttouched = 0;
int currtouched = 0;

int l_lasttouched = 0;
int l_currtouched = 0;
int r_lasttouched = 0;
int r_currtouched = 0;

bool touch_status = 0;
bool direction = 0; //0 for left, 1 for right
bool multitouch = 0;

void setup() {
  Serial.begin(9600);
  Keyboard.begin();
  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }

  if (!cap1.begin(0x5A)) {
    Serial.println("MPR121 0x5A not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 0x5A found!");

  if (!cap2.begin(0x5B)) {
    Serial.println("MPR121 0x5C not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 0x5C found!");

  if (!cap3.begin(0x5C)) {
    Serial.println("MPR121 0x5C not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 0x5C found!");
}


void loop() {
  lasttouched = currtouched;
  currtouched = TouchFormater();


  if(cap1.touched() == 0 && cap2.touched() == 0 && cap3.touched() == 0){
    touch_status = 0;
    Keyboard.release('q');
    Keyboard.release('e');   
  }

  //Touch status and direction detection
  if(abs(lasttouched - currtouched) == 1){
    if(lasttouched - currtouched > 0){
      direction = 0;
      touch_status = 1;
    }
    else if(lasttouched - currtouched < 0){
      direction = 1;
      touch_status = 1;
    }
  }

  if(abs(lasttouched - currtouched > 6)){
    multitouch = 1;
  }


  //Single key pressing process
  if(touch_status == 1 && multitouch == 0){
    if(direction == 0){
      Keyboard.release('e');
      Keyboard.press('q');
    }
    else{
      Keyboard.release('q');
      Keyboard.press('e');
    }
  }



//Formatted touch into a 1-32
int TouchFormater(){
  if(cap1.touched() != 0 && cap2.touched() == 0 && cap3.touched() == 0){
    return currtouched = log(cap1.touched())/log(2) + 1;
  }
  if(cap1.touched() == 0 && cap2.touched() != 0 && cap3.touched() == 0){
    return currtouched = log(cap2.touched())/log(2) + 13;
  }
  if(cap1.touched() == 0 && cap2.touched() == 0 && cap3.touched() != 0){
    return currtouched = log(cap3.touched())/log(2) + 25;
  }
  if(cap1.touched() == 0 && cap2.touched() == 0 && cap3.touched() == 0){
    return 0;
  }
}