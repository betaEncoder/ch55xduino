#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userMassStorage/USBMassStorage.h"

#define LED_BUILTIN 33

__code const uint8_t ReadmeFileContent[] =
{
  "This is a test file on Ch55xduino.\r\n"
};

__code const uint8_t LEDControlFileContent[] =
{
  "1\r\nThe first byte in this file controls the LED on board. Use a texteditor without autosave to avoid the disk full error.\r\n"
};

void LEDControlCallback(uint16_t offset) {
  for (uint8_t i = 0; i < BULK_MAX_PACKET_SIZE; i++) {
    uint8_t inputVal = BOT_Rx_Buf[i];
    if (offset == 0) {
      if (inputVal & 1) { //odd value, such as 1 or '1'
        digitalWrite(LED_BUILTIN, HIGH);
      } else { //even value, such as 0 or '0'
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
    offset++;
  }
}

//File should be smaller than 32K
#define LONGFILE_SIZE (2000)

void longfileCallback(uint16_t offset) {
  for (uint8_t i = 0; i < BULK_MAX_PACKET_SIZE; i++) {
    uint8_t returnVal = 0;
    if (offset < LONGFILE_SIZE) {
      //make each line 32 charactors.
      uint16_t lineCount = offset / 32;
      uint8_t lineOffset = offset % 32;
      if (lineOffset == 30) {
        returnVal = '\r';
      } else if (lineOffset == 31) {
        returnVal = '\n';
      } else if (lineOffset == (lineCount % 32)) {
        returnVal = '*';
      } else {
        returnVal = ' ';
      }
    }
    BOT_Tx_Buf[i] = returnVal;
    offset++;
  }
}

__code File_Entry filesOnDrive[] = {  //keep filename UPPERCASE
  {
    .filename = {'R', 'E', 'A', 'D', 'M', 'E', ' ', ' ', 'T', 'X', 'T'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = sizeof(ReadmeFileContent),
    .fileCallBackType = CONST_DATA_FILE,
    .fileReadHandler = {.constPtr = ReadmeFileContent},
    .fileWriteHandler = 0,  //NULL
  },
  {
    .filename = {'L', 'O', 'N', 'G', 'F', 'I', 'L', 'E', 'T', 'X', 'T'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = LONGFILE_SIZE,
    .fileCallBackType = FUNCTION_CALLBACK_FILE,
    .fileReadHandler = {.funcPtr = longfileCallback},
    .fileWriteHandler = 0,  //NULL
  },
  {
    .filename = {'L', 'E', 'D', '_', 'C', 'T', 'R', 'L', 'T', 'X', 'T'},
    .filetime = {TIME_LB(12, 34, 56), TIME_HB(12, 34, 56)},
    .filedate = {DATE_LB(2021, 1, 2), DATE_HB(2021, 1, 2)},
    .filesize = sizeof(LEDControlFileContent),
    .fileCallBackType = CONST_DATA_FILE,
    .fileReadHandler = {.constPtr = LEDControlFileContent},
    .fileWriteHandler = LEDControlCallback, //always a function
  }
};

__code uint8_t filesOnDriveCount = sizeof(filesOnDrive) / sizeof(File_Entry);



void setup() {
  USBInit();
}

void loop() {

}
