#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userUsbCdc/USBCDC.h"
extern __xdata uint8_t LineCoding[]; //lineCoding of CDC is located in this array
extern __xdata uint8_t controlLineState;
__xdata uint32_t oldBaudRate = 9600;
__xdata uint8_t oldLineState = 0;

#define USB_SWITCH_PIN 33
#define POWER_TARGET_PIN 34
#define USBDP_PULLUP_TARGET_PIN 14

void bootloaderRoutine() {
  USB_CTRL = 0;
  EA = 0;                                                                    //Disabling all interrupts is required.
  delayMicroseconds(50000);
  delayMicroseconds(50000);
  __asm__ ("lcall #0x3800");                                                 //Jump to bootloader code
  while (1);
}

void setup() {
  USBInit();

  pinMode(USB_SWITCH_PIN, OUTPUT);
  digitalWrite(USB_SWITCH_PIN, LOW);  //connect target to computer USB

  pinMode(POWER_TARGET_PIN, OUTPUT);
  digitalWrite(POWER_TARGET_PIN, LOW);

  pinMode(USBDP_PULLUP_TARGET_PIN, OUTPUT);
  digitalWrite(USBDP_PULLUP_TARGET_PIN, HIGH);

  Serial0_begin(9600);

}

void loop() {
  __xdata uint32_t currentBaudRate = *((__xdata uint32_t *)LineCoding); //both linecoding and sdcc are little-endian
  __xdata uint8_t currentLineState = controlLineState; //both linecoding and sdcc are little-endian

  if (oldBaudRate != currentBaudRate) {
    oldBaudRate = currentBaudRate;
    Serial0_begin(currentBaudRate);
  }

  if (oldLineState != currentLineState) {
    if  ( ((currentLineState & 0x01) == 0) && ((oldLineState & 0x01) == 1)) {
      if (currentBaudRate == 1200) {
        digitalWrite(POWER_TARGET_PIN, HIGH);
        delay(100);
        digitalWrite(USBDP_PULLUP_TARGET_PIN, LOW);
        delay(10);
        digitalWrite(POWER_TARGET_PIN, LOW);
        delay(10);
        digitalWrite(USBDP_PULLUP_TARGET_PIN, HIGH);
      }
    }
    oldLineState = currentLineState;
  }

  if (digitalRead(15) == LOW) {
    delay(10);
    if (digitalRead(15) == LOW) {
      bootloaderRoutine();
    }
  }

}
