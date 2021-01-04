#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userMassStorage/USBMassStorage.h"

void setup() {
  USBInit();
}

void loop() {
  // put your main code here, to run repeatedly:
}
