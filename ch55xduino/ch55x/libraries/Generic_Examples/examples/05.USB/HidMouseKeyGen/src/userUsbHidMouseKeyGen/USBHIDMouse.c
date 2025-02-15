#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"

/*extern*/ __xdata __at (EP0_ADDR) uint8_t  Ep0Buffer[];
/*extern*/ __xdata __at (EP1_ADDR) uint8_t  Ep1Buffer[];

//volatile __xdata uint8_t UpPoint1_Busy;   //Flag of whether upload pointer is busy

__xdata uint8_t HIDMouse[5] = {0x2, 0x0, 0x0, 0x0, 0x0};

typedef void( *pTaskFn)( void );

void delayMicroseconds(uint16_t us);
/*
void USBInit(){
    USBDeviceCfg();                                                       //Device mode configuration
    USBDeviceEndPointCfg();                                               //Endpoint configuration   
    USBDeviceIntCfg();                                                    //Interrupt configuration    
    UEP0_T_LEN = 0;
    UEP1_T_LEN = 0;                                                       //Pre-use send length must be cleared	  
}



void USB_EP1_IN(){
    UEP1_T_LEN = 0;
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           // Default NAK
    UpPoint1_Busy = 0;                                                  //Clear busy flag
}


void USB_EP1_OUT(){
    if ( U_TOG_OK )                                                     // Discard unsynchronized packets
    {

    }
}*/

static uint8_t USB_EP1_send(){
    uint16_t waitWriteCount = 0;
    
    waitWriteCount = 0;
    while (UpPoint1_Busy){//wait for 250ms or give up
        waitWriteCount++;
        delayMicroseconds(5);   
        if (waitWriteCount>=50000) return 0;
    }
    
    for (uint8_t i=0;i<sizeof(HIDMouse);i++){                                  //load data for upload
        Ep1Buffer[64+i] = HIDMouse[i];
    }
                   
    UEP1_T_LEN = sizeof(HIDMouse);                                             //data length
    UpPoint1_Busy = 1;
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                //upload data and respond ACK
    
    return 1;
}

uint8_t Mouse_press(uint8_t k) {
	HIDMouse[0] = 2;
	HIDMouse[1] |= k;
	USB_EP1_send();
	return 1;
}

uint8_t Mouse_release(uint8_t k) {
	HIDMouse[1] &= ~k;

	USB_EP1_send();
	return 1;
}

void Mouse_releaseAll(void){
    HIDMouse[1] = 0;
	USB_EP1_send();
}

void Mouse_move(int8_t x, int8_t y){
    HIDMouse[2] = x;
    HIDMouse[3] = y;
	USB_EP1_send();
}

void Mouse_scroll(int8_t tilt){
    HIDMouse[4] = tilt;
	USB_EP1_send();
}