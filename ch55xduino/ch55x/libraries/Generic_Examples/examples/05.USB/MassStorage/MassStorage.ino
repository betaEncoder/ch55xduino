#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userMassStorage/USBMassStorage.h"

__code const uint8_t ReadmeFileContent[] =
{
  "This is a test file on Ch55xduino.\r\n"
};

#define LONGFILE_SIZE (2000)

void longfileCallback(uint16_t offset) {
  for (uint8_t i = 0; i < BULK_MAX_PACKET_SIZE; i++) {
    if (offset < LONGFILE_SIZE) {
      BOT_Tx_Buf[i] = 'a';
    } else {
      BOT_Tx_Buf[i] = 0;
    }
    offset++;
  }
}

__code File_Entry filesOnDrive[] = {  //keep filename UPPERCASE
  {
    .filename = {'R', 'E', 'A', 'D', 'M', 'E', ' ', ' ', 'T', 'X', 'T'},
    .filetime = {TIME_LB(15, 48, 26), TIME_HB(15, 48, 26),},
    .filedate = {DATE_LB(2008, 8, 19), DATE_HB(2008, 8, 19)},
    .filesize = 10,
    .fileCallBackType = CONST_DATA_FILE,
    .filePtr = {.constPtr = ReadmeFileContent},
    },
  {
    .filename = {'L', 'O', 'N', 'G', 'F', 'I', 'L', 'E', 'T', 'X', 'T'},
    .filetime = {TIME_LB(15, 48, 26), TIME_HB(15, 48, 26),},
    .filedate = {DATE_LB(2008, 8, 19), DATE_HB(2008, 8, 19)},
    .filesize = LONGFILE_SIZE,
    .fileCallBackType = FUNCTION_CALLBACK_FILE,
    .filePtr = {.funcPtr = longfileCallback},
  }
};

__code uint8_t filesOnDriveCount = sizeof(filesOnDrive) / sizeof(File_Entry);



void setup() {
  USBInit();
}

void loop() {
  // put your main code here, to run repeatedly:
}
