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

#include "src/userUsbHidMouseKey/USBHIDKeyboard.h"
#include "src/userUsbHidMouseKey/USBHIDMouse.h"

#define BUTTON1_PIN 15
#define BUTTON2_PIN 16
#define BUTTON3_PIN 17

#define LED_BUILTIN 14

bool button1PressPrev = false;
bool button2PressPrev = false;
bool button3PressPrev = false;


void setup() {
  USBInit();
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  //button 1 is mapped to letter 'a'
  bool button1Press = !digitalRead(BUTTON1_PIN);
  if (button1PressPrev != button1Press) {
    button1PressPrev = button1Press;
    if (button1Press) {
      Keyboard_press('a');
    } else {
      Keyboard_release('a');
    }
  }

  //button 2 is mapped to left click
  bool button2Press = !digitalRead(BUTTON2_PIN);
  if (button2PressPrev != button2Press) {
    button2PressPrev = button2Press;
    if (button2Press) {
      Mouse_press(BUTTON_LEFT);
    }else{
      Mouse_release(BUTTON_LEFT);
    }
  }

  //button 3 is mapped to Capslock
  bool button3Press = !digitalRead(BUTTON3_PIN);
  if (button3PressPrev != button3Press) {
    button3PressPrev = button3Press;
    if (button3Press) {
      Keyboard_press(KEY_CAPS_LOCK);
      delay(100); //a quick capslock press is not recognized on mac
      Keyboard_release(KEY_CAPS_LOCK);
    }
  }

  //map capsLock to LED
  //Bit 0: NUM lock, Bit 1: CAPS lock, Bit 2: SCROLL lock, Bit 3: Compose, Bit 4: Kana,
  if (Keyboard_getLEDStatus() & 2) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(50);  //naive debouncing

}
