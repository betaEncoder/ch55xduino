#ifndef __USB_HID_KBD_H__
#define __USB_HID_KBD_H__

#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"

//extern volatile uint8_t received;
//extern __xdata uint8_t  Ep1Buffer[];
//extern __xdata uint8_t HIDRep[];

#ifdef __cplusplus
extern "C" {
#endif

void USBInit(void);

uint8_t* get_IN_buffer();
uint8_t* get_OUT_buffer();
bool is_received();
void clear_received_flag();
uint8_t USB_EP1_send();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
