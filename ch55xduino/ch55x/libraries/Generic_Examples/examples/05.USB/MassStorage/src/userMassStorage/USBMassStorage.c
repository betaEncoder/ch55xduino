#include <stdint.h>
#include <stdbool.h>
#include "include/ch554.h"
#include "include/ch554_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"

extern __xdata __at (EP0_ADDR) uint8_t  Ep0Buffer[];
extern __xdata __at (EP1_ADDR) uint8_t  Ep1Buffer[];




void USBInit(){
    USBDeviceCfg();                                                       //Device mode configuration
    USBDeviceEndPointCfg();                                               //Endpoint configuration   
    USBDeviceIntCfg();                                                    //Interrupt configuration    
    UEP0_T_LEN = 0;
    UEP1_T_LEN = 0;                                                       //Pre-use send length must be cleared
}

void Mass_Storage_In (void) {
    /*switch (Bot_State) {
     case BOT_CSW_Send:
     case BOT_ERROR:
     Bot_State = BOT_IDLE;
     
     // Enable the Endpoint to receive the next cmd
     // SetEPRxStatus(ENDP2, EP_RX_VALID);
     BOT_EP_Rx_Valid();
     break;
     case BOT_DATA_IN:
     if (CBW.CB[0] == SCSI_READ10) {
     SCSI_Read10_Cmd();
     }
     break;
     case BOT_DATA_IN_LAST:
     Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
     // SetEPRxStatus(ENDP2, EP_RX_VALID);
     BOT_EP_Rx_Valid();
     break;
     case BOT_DATA_IN_LAST_FAIL:
     Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
     
     // SetEPRxStatus(ENDP2, EP_RX_VALID);
     BOT_EP_Rx_Valid();
     break;
     default:
     break;
     }*/
}

void Mass_Storage_Out (void) {
    if (!U_TOG_OK)
        return;    // Discard unsychronized packets
    
    /*switch (Bot_State) {
        case BOT_IDLE:
            CBW_Decode();
            break;
        case BOT_DATA_OUT:
            if (CBW.CB[0] == SCSI_WRITE10) {
                SCSI_Write10_Cmd();
            } else {
                Bot_Abort(DIR_OUT);
                Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
                Set_CSW (CSW_PHASE_ERROR, SEND_CSW_DISABLE);
            }
            break;
        default:
            Bot_Abort(BOTH_DIR);
            Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
            Set_CSW (CSW_PHASE_ERROR, SEND_CSW_DISABLE);
    } // switch (Bot_State)*/
}

/*void USB_EP2_IN(){
    UEP2_T_LEN = 0;                                                    // No data to send anymore
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //Respond NAK by default
    UpPoint2_Busy = 0;                                                  //Clear busy flag
}

void USB_EP2_OUT(){
    if ( U_TOG_OK )                                                     // Discard unsynchronized packets
    {
        USBByteCountEP2 = USB_RX_LEN;
        USBBufOutPointEP2 = 0;                                             //Reset Data pointer for fetching
        if (USBByteCountEP2)    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_R_RES | UEP_R_RES_NAK;       //Respond NAK after a packet. Let main code change response after handling.
    }
}*/

