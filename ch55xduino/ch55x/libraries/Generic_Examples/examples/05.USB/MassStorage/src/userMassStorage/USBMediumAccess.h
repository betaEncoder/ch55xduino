#ifndef __USB_MAL_H
#define __USB_MAL_H

#include <stdint.h>

#define MAL_MAX_LUN 0

#define MAL_OK   0
#define MAL_FAIL 1

extern __code const uint8_t emuDisk_Inquiry_Data[];

//make sure they match
extern __code const uint8_t diskCapacity[8];
extern __code const uint8_t formatCapacity[8];
#define MASS_BLOCK_COUNT 0x8000
#define MASS_BLOCK_SIZE 512

uint8_t LUN_GetStatus();
/*__code uint8_t* LUN_GetInquiryData (uint8_t lun);
void LUN_Eject (uint8_t lun);
// Read BULK_MAX_PACKET_SIZE bytes of data from device to BOT_Tx_Buf
void LUN_Read (uint32_t curAddr);
// Write BULK_MAX_PACKET_SIZE bytes of data from BOT_Rx_Buf to device
void LUN_Write (uint32_t curAddr);*/

#endif /* __USB_MAL_H */
