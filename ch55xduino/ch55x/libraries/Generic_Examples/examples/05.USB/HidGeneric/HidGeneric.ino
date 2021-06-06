/*
  HID Keyboard example


  created 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

*/

//For windows user, if you ever played with other HID device with the same PID C55D
//You may need to uninstall the previous driver completely        


#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userUsbHidGeneric/USBHIDGeneric.h"

#define ADC_PIN 15
#define BUTTON1_PIN 16

#define LED_BUILTIN 14

bool button1Press = false;
uint8_t adc=0;

uint8_t *buff_in;
uint8_t *buff_out;

void setup() {
  buff_in = get_IN_buffer();
  buff_out = get_OUT_buffer();
  USBInit();
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(ADC_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  adc = analogRead(ADC_PIN);

  if(is_received()){
    clear_received_flag();
    if(buff_in[0]==0){
      if(buff_in[1]==1){
        digitalWrite(LED_BUILTIN, HIGH);
      }else{
        digitalWrite(LED_BUILTIN, LOW);
      }
    }else if(buff_in[0]==1){
      buff_out[0] = button1Press;
    }else if(buff_in[0]==2){
      buff_out[0] = 0;
      buff_out[1] = adc;
    }
    USB_EP1_send();
  }

  //read button 1
  button1Press = !digitalRead(BUTTON1_PIN);
}
