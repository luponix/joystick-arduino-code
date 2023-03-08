// Zombie stick v6 - luponix, 2023.03.07
// Unites a modified sidewinder hat and a SS495A sensor into one device

#include <Joystick.h>
#include "ADS1X15.h"

ADS1115 ADS(0x48);
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  4, 1,                  // Button Count, Hat Switch Count
  true, false, false,    // X but no Y and Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

void setup() {
  ADS.begin();
  ADS.setGain(0);      // 6.144 volt
  ADS.setDataRate(7);  // fast
  ADS.setMode(0);      // continuous mode
  ADS.readADC(0);      // first read to trigger


  pinMode(3, INPUT_PULLUP); // fire button 
  pinMode(5, INPUT_PULLUP); // upper side button
  pinMode(6, INPUT_PULLUP); // lower side button
  pinMode(7, INPUT_PULLUP); // button below the hat
 
  // Hat buttons
  pinMode(8, INPUT_PULLUP); // Hat top
  pinMode(9, INPUT_PULLUP); // Hat right
  pinMode(10, INPUT_PULLUP);// Hat below
  pinMode(11, INPUT_PULLUP);// Hat left

  // Hall sensor
  pinMode(A0, INPUT_PULLUP);

  Joystick.begin(false);
  Joystick.setXAxisRange(0, 65536);

  //Debug
  //Serial.begin(115200);
}

//unsigned long t0 = 0;
void loop() {
  //unsigned long t1 = micros();
  // time since last loop
  //unsigned long t2 = t1-t0;
  //t0 = t1;
  //char buffer[20];
  //utoa(t2,buffer,10);
  //Serial.println(buffer);

  // read in the fire trigger
  Joystick.setButton(0, digitalRead(3) == LOW ? 1 : 0);

  // read upper side button
  Joystick.setButton(2, digitalRead(5) == LOW ? 1 : 0);

  // read lower side button
  Joystick.setButton(3, digitalRead(6) == LOW ? 1 : 0);

  // read the button bellow the hat
  Joystick.setButton(1, digitalRead(7) == LOW ? 1 : 0);


  // read the Hall sensor through the ads1115 board
  uint32_t axis = ADS.getValue();
  Joystick.setXAxis(axis);//map(analogRead(A0), 0, 1024, 0, 65536));
  
  
  // read the hat and set one of the 8 directions
  int hat = 0;
  hat += (digitalRead(8) == LOW ? 1 : 0);
  hat += (digitalRead(9) == LOW ? 2 : 0);
  hat += (digitalRead(10) == LOW ? 4 : 0);
  hat += (digitalRead(11) == LOW ? 8 : 0);

  // this arrangement may have to shift if the starting point is different
  // 0 1 2 3 => top, right, below, left
  // 1 2 4 8 -> 0, 1, 2, 4, 8, 3, 6, 12, 9
  switch(hat){
    case 0: // hat is in neutral position
      Joystick.setHatSwitch(0, JOYSTICK_HATSWITCH_RELEASE);
      break;
    case 1: // top
      Joystick.setHatSwitch(0, 0);
      break;
    case 2: // right
      Joystick.setHatSwitch(0, 90);
      break;
    case 3: // top + right
      Joystick.setHatSwitch(0, 45);
      break;
    case 4: // below
      Joystick.setHatSwitch(0, 180);
      break;
    case 6: // below + right
      Joystick.setHatSwitch(0, 135);
      break;
    case 8: // left
      Joystick.setHatSwitch(0, 270);
      break;
    case 9: // top + left      
      Joystick.setHatSwitch(0, 315);
      break;
    case 12:// below + left
      Joystick.setHatSwitch(0, 225);
      break;
  }


  Joystick.sendState();

  // the performance of this has not been tested yet
  // test the sample rate of the ads1115
  // create a pulsing button to measure the updates per second of the joystick device
  // this software can get altered in a bunch of ways to make it more efficient depending on what becomes the bottleneck

}
