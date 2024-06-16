#include <Keyboard.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap1 = Adafruit_MPR121();
Adafruit_MPR121 cap2 = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
int lasttouched = 0;
int currtouched = 0;

bool touch_status = 0;
bool direction = 0; //0 for left, 1 for right
bool multitouch = 0;

const int touchKey[4] = {"Q","E","1","3"};

void setup() {
  Serial.begin(9600);
  Keyboard.begin();
  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
//  cap1.begin(0x5A);
//  cap2.begin(0x5C);
//  cap3.begin(0x5C);

  if (!cap1.begin(0x5A)) {
    Serial.println("MPR121 0x5A not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 0x5A found!");

  if (!cap2.begin(0x5C)) {
    Serial.println("MPR121 0x5C not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 0x5C found!");

}
void loop() {

  if(cap1.touched() != 0 && cap2.touched() == 0){
    lasttouched = currtouched;
    currtouched = log(cap1.touched())/log(2) + 1;
  }
  if(cap1.touched() == 0 && cap2.touched() != 0){
    lasttouched = currtouched;
    currtouched = log(cap2.touched())/log(2) + 13;
  }

//  Serial.println(currtouched);

  //Touch status and direction detection
  if(abs(lasttouched - currtouched) == 1){
    if(lasttouched - currtouched > 0){
      direction = 0;
    }
    else if(lasttouched - currtouched < 0){
      direction = 1;
    }
    touch_status = 1;
  }

  //Direction detection
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
//  if(abs(lasttouched - currtouched > 6)){
//    multitouch = 1;
//  }

  //Key pressing process
  if(multitouch == 0 && touch_status == 1){
    if (cap1.touched() == 0 && cap2.touched() == 0){
      Keyboard.release('q');
      Keyboard.release('e');
      touch_status = 0;
    }
    else{
      if(direction == 0){
        Keyboard.release('e');
        Keyboard.press('q');
      }
      else{
        Keyboard.release('q');
        Keyboard.press('e');
      }
    }
  }
}