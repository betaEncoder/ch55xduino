#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userMassStorage/USBMassStorage.h"

__code const uint8_t ReadmeFile[] =
{
  "This is a test file on speed demon.\n\r"
};

void fileCallback(uint16_t a) {

}

__code File_Entry filesOnDrive[1] = {
  {
    .filename = {'R', 'E', 'A', 'D', 'M', '2', ' ', ' ', 'T', 'X', 'T'},
    .filetime = {TIME_LB(15, 48, 26), TIME_HB(15, 48, 26),},
    .filedate = {DATE_LB(2008, 8, 19), DATE_HB(2008, 8, 19)},
    .filesize = 10,
    .fileCallBackType = CONST_DATA_FILE,
    .filePtr = {.constPtr = ReadmeFile},
  },
  {
    .filename = {'R', '1', '1', 'D', 'M', 'E', ' ', ' ', 'T', 'X', 'T'},
    .filetime = {TIME_LB(15, 48, 26), TIME_HB(15, 48, 26),},
    .filedate = {DATE_LB(2008, 8, 19), DATE_HB(2008, 8, 19)},
    .filesize = 10,
    .fileCallBackType = FUNCTION_CALLBACK_FILE,
    .filePtr = {.funcPtr = fileCallback},
  }
};

__code uint8_t filesOnDriveCount = sizeof(filesOnDrive)/sizeof(filesOnDrive[0]);



void setup() {
  USBInit();
}

void loop() {
  // put your main code here, to run repeatedly:
}
