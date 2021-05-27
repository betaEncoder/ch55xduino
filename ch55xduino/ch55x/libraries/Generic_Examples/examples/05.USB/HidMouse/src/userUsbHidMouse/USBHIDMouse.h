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

void USBInit(void);

uint8_t Keyboard_press(uint8_t k);
uint8_t Keyboard_release(uint8_t k);
void Keyboard_releaseAll(void);

uint8_t Keyboard_write(uint8_t c);

uint8_t Keyboard_getLEDStatus();

#ifdef __cplusplus
} // extern "C"
#endif

#endif

