/*
  HID Keyboard, Mouse and vendor specific inout device example


  created 2021
  by betaEncoder for use with CH55xduino

  This example code is in the public domain.

*/

//For windows user, if you ever played with other HID device with the same PID C55D
//You may need to uninstall the previous driver completely        


#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userUsbHidMouseKeyGen/USBHIDKeyboard.h"
#include "src/userUsbHidMouseKeyGen/USBHIDMouse.h"
#include "src/userUsbHidMouseKeyGen/USBHIDGeneric.h"

#define ADC_PIN 15
#define BUTTON2_PIN 16
#define BUTTON3_PIN 17

#define LED_BUILTIN 14

bool button1PressPrev = false;
bool button2PressPrev = false;
bool button3PressPrev = false;

uint8_t adc=0;

uint8_t *buff_in;
uint8_t *buff_out;


void setup() {
  USBInit();
  buff_in = get_IN_buffer();
  buff_out = get_OUT_buffer();
  pinMode(ADC_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  adc = analogRead(ADC_PIN);

  if(is_received()){
    clear_received_flag();
    if(buff_in[0]==3){  // report ID match
      if(buff_in[1]==0){
        if(buff_in[2]==1){
          digitalWrite(LED_BUILTIN, HIGH);
        }else{
          digitalWrite(LED_BUILTIN, LOW);
        }
      }else if(buff_in[1]==1){
        buff_out[1] = button2PressPrev;
      }else if(buff_in[1]==2){
        buff_out[1] = 0;
        buff_out[2] = adc;
      }
      buff_out[0] = 3;  // report ID
      USB_EP1_send();
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

  //button 3 is mapped to letter 'a'
  bool button3Press = !digitalRead(BUTTON3_PIN);
  if (button3PressPrev != button3Press) {
    button3PressPrev = button3Press;
    if (button3Press) {
      Keyboard_press('a');
    } else {
      Keyboard_release('a');
    }
  }

  //map capsLock to LED
  //Bit 0: NUM lock, Bit 1: CAPS lock, Bit 2: SCROLL lock, Bit 3: Compose, Bit 4: Kana,
  if (Keyboard_getLEDStatus() & 2) {
    //digitalWrite(LED_BUILTIN, HIGH);
  } else {
    //digitalWrite(LED_BUILTIN, LOW);
  }

  delay(50);  //naive debouncing

}
