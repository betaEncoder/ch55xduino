#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"

volatile uint8_t received = 0;

volatile __xdata uint8_t UpPoint1_Busy  = 0;   //Flag of whether upload pointer is busy
volatile __xdata uint8_t USBByteCountEP1 = 0;      //Bytes of received data on USB endpoint
volatile __xdata uint8_t USBBufOutPointEP1 = 0;    //Data pointer for fetching

__xdata uint8_t HIDRep[64];

typedef void( *pTaskFn)( void );

void delayMicroseconds(uint16_t us);

void USBInit(){
	uint8_t i;
    USBDeviceCfg();                                                       //Device mode configuration
    USBDeviceEndPointCfg();                                               //Endpoint configuration   
    USBDeviceIntCfg();                                                    //Interrupt configuration    
    UEP0_T_LEN = 0;
    UEP1_T_LEN = 0;                                                       //Pre-use send length must be cleared
	for(i=0;i<sizeof(HIDRep);i++){	//clear buffer
		HIDRep[i] = 0;
	}
}


uint8_t* get_IN_buffer(){
    return Ep1Buffer;
}

uint8_t* get_OUT_buffer(){
    return HIDRep;
}

bool is_received(){
    return received;
}

void clear_received_flag(){
    received = 0;
        USBByteCountEP1 = 0;//USB_RX_LEN;
        USBBufOutPointEP1 = 0;                                             //Reset Data pointer for fetching

}

void USB_EP1_IN(){
    UEP1_T_LEN = 0;
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           // Default NAK
    UpPoint1_Busy = 0;                                                  //Clear busy flag
}


void USB_EP1_OUT(){
    if ( U_TOG_OK )                                                     // Discard unsynchronized packets
    {
		received = 1;
        if (USBByteCountEP1)    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_R_RES | UEP_R_RES_NAK;       //Respond NAK after a packet. Let main code change response after handling.
    }
}

uint8_t USB_EP1_send(){
    uint16_t waitWriteCount = 0;
    
    waitWriteCount = 0;
    while (UpPoint1_Busy){//wait for 250ms or give up
        waitWriteCount++;
        delayMicroseconds(5);   
        if (waitWriteCount>=50000) return 0;
    }
    
    for (uint8_t i=0;i<sizeof(HIDRep);i++){                                  //load data for upload
        Ep1Buffer[64+i] = HIDRep[i];
    }
                   
    UEP1_T_LEN = sizeof(HIDRep);                                             //data length
    UpPoint1_Busy = 1;
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                //upload data and respond ACK
    
    return 1;
}
