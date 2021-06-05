#ifndef __USB_HID_MOUSE_H__
#define __USB_HID_MOUSE_H__

#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"


#define BUTTON_LEFT     0x01
#define BUTTON_RIGHT    0x02
#define BUTTON_CENTER   0x04

#ifdef __cplusplus
extern "C" {
#endif

volatile extern __xdata uint8_t UpPoint1_Busy;

//void USBInit(void);

uint8_t Mouse_press(uint8_t k);
uint8_t Mouse_release(uint8_t k);
void Mouse_releaseAll(void);
void Mouse_move(int8_t x, int8_t y);
void Mouse_scroll(int8_t tilt);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

