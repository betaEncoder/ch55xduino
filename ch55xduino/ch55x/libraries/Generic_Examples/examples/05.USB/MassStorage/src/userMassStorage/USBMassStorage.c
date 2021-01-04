#include <stdint.h>
#include <stdbool.h>
#include "include/ch554.h"
#include "include/ch554_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"
#include "USBMassStorage.h"
#include "USBSCSI.h"
#include "USBMediumAccess.h"

__xdata uint8_t Bot_State = BOT_IDLE;
__xdata Bulk_Only_CBW CBW;
__xdata uint16_t dataResidue;
__xdata uint32_t SCSI_LBA;
__xdata uint16_t SCSI_BlkLen;

__xdata uint8_t SCSI_Sense_Key;  //we only has 1 logic device
__xdata uint8_t SCSI_Sense_Asc;

//!!!
#pragma callee_saves sendCharDebug
void sendCharDebug(char c);

void USBInit(){
    USBDeviceCfg();                                                       //Device mode configuration
    USBDeviceEndPointCfg();                                               //Endpoint configuration   
    USBDeviceIntCfg();                                                    //Interrupt configuration    
    UEP0_T_LEN = 0;
    UEP1_T_LEN = 0;                                                       //Pre-use send length must be cleared
}

void Mass_Storage_Reset (void) {
    Bot_State = BOT_IDLE;
}
    
void Mass_Storage_In (void) {
     switch (Bot_State) {
         case BOT_CSW_Send:
         case BOT_ERROR:
             Bot_State = BOT_IDLE;
             // Enable the Endpoint to receive the next cmd
             BOT_EP_Rx_Valid();
             break;
         /*case BOT_DATA_IN:
         if (CBW.CB[0] == SCSI_READ10) {
         SCSI_Read10_Cmd();
         }
         break;*/
         case BOT_DATA_IN_LAST:
             Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
             BOT_EP_Rx_Valid();
             break;
         case BOT_DATA_IN_LAST_FAIL:
             Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
             BOT_EP_Rx_Valid();
             break;
         default:
             break;
     }
}

void Mass_Storage_Out (void) {
    if (!U_TOG_OK)
        return;    // Discard unsychronized packets
    
    switch (Bot_State) {
        case BOT_IDLE:
            CBW_Decode();
            break;
        /*case BOT_DATA_OUT:
            if (CBW.CB[0] == SCSI_WRITE10) {
                SCSI_Write10_Cmd();
            } else {
                Bot_Abort(DIR_OUT);
                Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
                Set_CSW (CSW_PHASE_ERROR, SEND_CSW_DISABLE);
            }
            break;*/
        default:
            Bot_Abort(BOTH_DIR);
            //Set_Scsi_Sense_Data
            SCSI_Sense_Key = ILLEGAL_REQUEST;
            SCSI_Sense_Asc = INVALID_FIELED_IN_COMMAND;
            Set_CSW (CSW_PHASE_ERROR, SEND_CSW_DISABLE);
    } // switch (Bot_State)
}


void CBW_Decode(void) {
    //Decode the received CBW and call the related SCSI command

    // Copy CBW from Endpoint Rx Buffer
    for (uint8_t i = 0; i < BOT_EP_Rx_Length; i++){
        ((uint8_t*)(&CBW))[i] = BOT_Rx_Buf[i];
        //sendCharDebug(BOT_Rx_Buf[i]);//!!!!!!!
    }
    
    //U32_XLittle(&CBW.dDataLength, &BOT_Rx_Buf[8]);
    dataResidue = CBW.dDataLength;
    
    
    P1_1=0;//!!!!!!!
    sendCharDebug('L');//!!!!!!!
    sendCharDebug(BOT_EP_Rx_Length);//!!!!!!!
    
    if (BOT_EP_Rx_Length != BOT_CBW_PACKET_LENGTH) {
        Bot_Abort(BOTH_DIR);
        // Clear the CBW.dSignature to disable the clear feature until receiving a Mass storage reset
        CBW.dSignature0 = 0;
        CBW.dSignature1 = 0;
        CBW.dSignature2 = 0;
        CBW.dSignature3 = 0;
        //Set_Scsi_Sense_Data
        SCSI_Sense_Key = ILLEGAL_REQUEST;
        SCSI_Sense_Asc = PARAMETER_LIST_LENGTH_ERROR;
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
        return;
    }
        
        P1_1=1;//!!!!!!!
    sendCharDebug('W');//!!!!!!!
    sendCharDebug(CBW.bCBLength);//!!!!!!!
    sendCharDebug('C');//!!!!!!!
    sendCharDebug(CBW.CB[0]);//!!!!!!!
    
    if ((CBW.CB[0] == SCSI_READ10 ) || (CBW.CB[0] == SCSI_WRITE10 )) {
        // Calculate Logical Block Address, convert from BigEndian to LittleEndian
        U32_XBig(&SCSI_LBA, &CBW.CB[2]);
        
        // Calculate the Number of Blocks to transfer, convert from BigEndian to LittleEndian
        U16_XBig(&SCSI_BlkLen, &CBW.CB[7]);
    }
    
    if (
        CBW.dSignature0 == BOT_CBW_SIGNATURE0 &&
        CBW.dSignature1 == BOT_CBW_SIGNATURE1 &&
        CBW.dSignature2 == BOT_CBW_SIGNATURE2 &&
        CBW.dSignature3 == BOT_CBW_SIGNATURE3 ){
        
        if ((CBW.bLUN > 0) || (CBW.bCBLength < 1) || (CBW.bCBLength > 16)) {
            //This ensures bLUN always 0
            Bot_Abort(BOTH_DIR);
            //Set_Scsi_Sense_Data
            SCSI_Sense_Key = ILLEGAL_REQUEST;
            SCSI_Sense_Asc = INVALID_FIELED_IN_COMMAND;
            Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
        } else {
            // Valid CBW
            switch (CBW.CB[0]) {
                case SCSI_REQUEST_SENSE:  //0x03
                    {
                        for (uint8_t i=0; i<REQUEST_SENSE_DATA_LEN; i++)
                            BOT_Tx_Buf[i] = Scsi_Sense_Data[i];
                        
                        BOT_Tx_Buf[2] = SCSI_Sense_Key;
                        BOT_Tx_Buf[12] = SCSI_Sense_Asc;
                        
                        if (CBW.CB[4] <= REQUEST_SENSE_DATA_LEN) {
                            Reply_Request(CBW.CB[4]);
                        } else {
                            Reply_Request(REQUEST_SENSE_DATA_LEN);
                        }
                    }
                    break;
                case SCSI_INQUIRY:  //0x12
                    {
                        __code uint8_t* Inquiry_Data;
                        uint16_t Inquiry_Data_Length;
                        
                        if (CBW.CB[1] & 0x01) {/*Evpd is set*/
                            Inquiry_Data = (__code uint8_t*) Page00_Inquiry_Data;
                            Inquiry_Data_Length = PAGE00_INQUIRY_DATA_LEN;
                        } else {
                            Inquiry_Data = (__code uint8_t*) emuDisk_Inquiry_Data;
                            
                            if (CBW.CB[4] <= STANDARD_INQUIRY_DATA_LEN)
                                Inquiry_Data_Length = CBW.CB[4];
                            else
                                Inquiry_Data_Length = STANDARD_INQUIRY_DATA_LEN;
                        }
                        Transfer_Data_Request(Inquiry_Data, Inquiry_Data_Length);
                    }
                    break;
                /*case SCSI_START_STOP_UNIT:
                    SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
                    break;*/
                case SCSI_ALLOW_MEDIUM_REMOVAL:  //0x1E
                    {
                        if (CBW.CB[4] & 1) {
                            //Set_Scsi_Sense_Data
                            SCSI_Sense_Key = ILLEGAL_REQUEST;
                            SCSI_Sense_Asc = INVALID_COMMAND;
                            Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
                        } else {
                            Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
                        }
                    }
                    break;
                case SCSI_MODE_SENSE6:  //0x1A
                    {
                        Transfer_Data_Request((__code uint8_t*)Mode_Sense6_data, MODE_SENSE6_DATA_LEN);
                    }
                    break;
                /*case SCSI_MODE_SENSE10:
                    SCSI_ModeSense10_Cmd (CBW.bLUN);
                    break;
                case SCSI_READ_FORMAT_CAPACITIES:
                    SCSI_ReadFormatCapacity_Cmd(CBW.bLUN);
                    break;*/
                case SCSI_READ_CAPACITY10:  //0x25
                    {
                        if (LUN_GetStatus()) {
                            //Set_Scsi_Sense_Data
                            SCSI_Sense_Key = NOT_READY;
                            SCSI_Sense_Asc = MEDIUM_NOT_PRESENT;
                            Transfer_Failed_ReadWrite();
                            return;
                        }
                        
                        // Index of the last block,
                        for (uint8_t i=0;i<8;i++){
                            BOT_Tx_Buf[i]=diskCapacity[i];
                        }
                        
                        Reply_Request(READ_CAPACITY10_DATA_LEN);
                    }
                    break;
                case SCSI_TEST_UNIT_READY:  //0x00
                    {
                        if (LUN_GetStatus()) {
                            //Set_Scsi_Sense_Data
                            SCSI_Sense_Key = NOT_READY;
                            SCSI_Sense_Asc = MEDIUM_NOT_PRESENT;
                            Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
                            Bot_Abort(DIR_IN); // not necessary?
                        } else {
                            Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
                        }
                    }
                    break;
                /*case SCSI_READ10:
                    SCSI_Read10_Cmd();
                    break;
                case SCSI_WRITE10:
                    SCSI_Write10_Cmd();
                    break;
                case SCSI_VERIFY10:
                    SCSI_Verify10_Cmd(CBW.bLUN);
                    break;
                case SCSI_FORMAT_UNIT:
                    SCSI_Format_Cmd(CBW.bLUN);
                    break;*/
                    
                default:
                    Bot_Abort(BOTH_DIR);
                    //Set_Scsi_Sense_Data
                    SCSI_Sense_Key = ILLEGAL_REQUEST;
                    SCSI_Sense_Asc = INVALID_COMMAND;
                    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
            } // switch (CBW.CB[0])
        }
    } else { // End of checking signature
        // Invalid CBW
        Bot_Abort(BOTH_DIR);
        //Set_Scsi_Sense_Data
        SCSI_Sense_Key = ILLEGAL_REQUEST;
        SCSI_Sense_Asc = INVALID_COMMAND;
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    }
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



void Transfer_Data_Request(__code uint8_t* Data_Pointer, __xdata uint8_t Data_Len) {
    //Send the request response to the PC HOST.
    dataResidue -= Data_Len;
    
    BOT_EP_Tx_Count(Data_Len);
    while (Data_Len--)
        BOT_Tx_Buf[Data_Len] = Data_Pointer[Data_Len];
    
    BOT_EP_Tx_Valid();    // Enable Tx
    
    Bot_State = BOT_DATA_IN_LAST;
}

void Reply_Request(uint8_t Data_Len) {
    dataResidue -= Data_Len;
    BOT_EP_Tx_Count(Data_Len);
    BOT_EP_Tx_Valid();    // Enable Tx
    
    Bot_State = BOT_DATA_IN_LAST;
}

void Transfer_Failed_ReadWrite(void) {
    BOT_EP_Tx_Count(0);    // Send an empty packet
    BOT_EP_Tx_Valid();    // Enable Tx
    
    Bot_State = BOT_DATA_IN_LAST_FAIL;
}

void Set_CSW (uint8_t CSW_Status, __xdata uint8_t Send_Permission)
{
    //Set the SCW with the needed fields.
    //uint8_t CSW_Status this filed can be CSW_CMD_PASSED,CSW_CMD_FAILED,or CSW_PHASE_ERROR.
    
    //maybe optimized by improve parameters and DPTR access
    
    // Make CSW
    BOT_EP_Tx_Count(CSW_DATA_LENGTH);
    // CSW.dSignature
    BOT_Tx_Buf[0] = BOT_CSW_SIGNATURE0;
    BOT_Tx_Buf[1] = BOT_CSW_SIGNATURE1;
    BOT_Tx_Buf[2] = BOT_CSW_SIGNATURE2;
    BOT_Tx_Buf[3] = BOT_CSW_SIGNATURE3;
    
    // CSW.dTag
    BOT_Tx_Buf[4] = CBW.dTag0;
    BOT_Tx_Buf[5] = CBW.dTag1;
    BOT_Tx_Buf[6] = CBW.dTag2;
    BOT_Tx_Buf[7] = CBW.dTag3;
    
    // CSW.dDataResidue (little-endian)
    BOT_Tx_Buf[8]= ((uint8_t *)(&dataResidue))[0];
    BOT_Tx_Buf[9]= ((uint8_t *)(&dataResidue))[1];
    
    BOT_Tx_Buf[10] = 0;
    BOT_Tx_Buf[11] = 0;
    
    // CSW.bStatus
    BOT_Tx_Buf[12] = CSW_Status;
    
    Bot_State = BOT_ERROR;
    if (Send_Permission) {
        Bot_State = BOT_CSW_Send;
        // SetEPTxStatus(ENDP1, EP_TX_VALID);
        BOT_EP_Tx_Valid();    // Enable Tx
    }
}

void Bot_Abort(uint8_t Direction) {
    //Stall the needed Endpoint according to the selected direction.
    switch (Direction) {
        case DIR_IN :
            BOT_EP_Tx_Stall();
            break;
        case DIR_OUT :
            BOT_EP_Rx_Stall();
            break;
        case BOTH_DIR :
            BOT_EP_Tx_Stall();
            BOT_EP_Rx_Stall();
            break;
        default:
            break;
    }
}
