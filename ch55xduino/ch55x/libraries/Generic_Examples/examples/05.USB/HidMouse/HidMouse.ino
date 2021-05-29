/*
  HID Keyboard example


  created 2021
  by betaEncoder for use with CH55xduino

  This example code is in the public domain.

*/

//For windows user, if you ever played with other HID device with the same PID C55D
//You may need to uninstall the previous driver completely        


#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userUsbHidMouse/USBHIDMouse.h"

#define BUTTON1_PIN 15
#define BUTTON2_PIN 16
#define BUTTON3_PIN 17

bool button1PressPrev = false;
bool button2PressPrev = false;
bool button3PressPrev = false;


void setup() {
  USBInit();
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
}

void loop() {
  //button 1 is mapped to left click
  bool button1Press = !digitalRead(BUTTON1_PIN);
  bool button2Press = !digitalRead(BUTTON2_PIN);
  bool button3Press = !digitalRead(BUTTON3_PIN);
  
  if (button1PressPrev != button1Press) {
    button1PressPrev = button1Press;
    if (button1Press) {
      Mouse_press(BUTTON_LEFT);
    } else {
      Mouse_release(BUTTON_LEFT);
    }
  }
  if (button2PressPrev != button2Press) {
    button2PressPrev = button2Press;
    if (button2Press) {
      Mouse_press(BUTTON_RIGHT);
    } else {
      Mouse_release(BUTTON_RIGHT);
    }
  }
  if (button3PressPrev != button3Press) {
    button3PressPrev = button3Press;
    if (button3Press) {
      Mouse_scroll(2);
    } else {
      Mouse_scroll(0);
    }
  }

  delay(50);  //naive debouncing

}
