#include "USBMassStorage.h"
#include "USBMediumAccess.h"

/* External variables --------------------------------------------------------*/
// Defined in USBMassStorage.c
extern __xdata Bulk_Only_CBW CBW;

//FAT info: http://www.c-jump.com/CIS24/Slides/FAT/lecture.html
//http://elm-chan.org/docs/fat_e.html
//FAT12<4087 clusters. FAT16>4087 <65526clusters
//FAT16 FAT is easier to generate

__code const uint8_t diskCapacity[8]=
{
    0x00,0x00,0x3F,0xFF, //Max accessible logic block address
    0x00,0x00,0x02,0x00  //size of block
    //Size of this disk is:
    //(0x3FFF+1)*0x200 = 0x800000 = 8*1024*1024 = 8MB.
};

__code const uint8_t formatCapacity[8]=
{
    0x00,0x00,0x40,0x00, //Max accessible logic block count
    0x02,0x00,0x02,0x00  //size of block // 02:Descriptor Code: Formatted Media
    //Size of this disk is:
    //(0x3FFF+1)*0x200 = 0x800000 = 8*1024*1024 = 8MB.
};

__xdata uint8_t emuDisk_Status = MAL_OK;

__code const uint8_t emuDisk_Inquiry_Data[] =
  {
    0x00,          /* Direct Access Device */
    0x80,          /* RMB = 1: Removable Medium */
    0x04,          /* Version: No conformance claim to standard, 4: The device complies to ANSI INCITS 351-2001 (SPC-2) */
    0x02,          /* Response Data Format = 2, The data complies with the SCSI-3 specification.*/

    0x1f,          /* Additional Length */
    0x00,          /* SCCS = 1: Storage Controller Component */
    0x00,
    0x00,
    /* Vendor Identification */
    'D', 'e', 'q', 'i', 'n', 'g', ' ', ' ',				/* Manufacturer: 8 bytes */
    /* Product Identification */
    'C', 'H', '5', '5', 'x', 'd', 'u', 'i',				/* Product : 16 Bytes */
		'n', 'o', ' ', 'M', 'S', 'D', ' ', ' ',
    /* Product Revision Level */
    '1', '.', '0', ' '														/* Version : 4 Bytes */
  };

__code const uint8_t DBR_data[62]={
    0xeb, 0x3e, 0x90,          //Instructions to jump to boot code
    'M','S','D','O','S','5','.','0', //Name string (MSDOS5.0)
    0x00, 0x02,                //Bytes/sector (0x0200 = 512)
    0x01,                      //Sectors/cluster (1)
    0x01, 0x00,                //Size of reserved area (1 sector)
    0x02,                      //Number of FATs (2)
    0x00, 0x02,                //Max. number of root directory entries (0x0200 = 512)
    0x00, 0x00,                //Total number of sectors (0, not used)
    0xF8,                      //Media type (hard disk)
    0x20, 0x00,                //FAT size (0x0020 = 32 sectors)
    0x20, 0x00,                //Sectors/track (0x0020 = 32)
    0x40, 0x00,                //Number of heads (0x0040 = 64)
    0x00, 0x00, 0x00, 0x00,    //Number of sector before partition (0)
    0x00, 0x40, 0x00, 0x00,    //Total number of sectors，8M is 0x4000, 16384 clusters, FAT16 is more than 4087 clusters
    0x80,                      //Drive number (hard disk)
    0x00,                      //Reserved
    0x29,                      //Extended boot signature
    0x00, 0x00, 0x00, 0x00,    //Volume serial number
    
    //Volume label
    'C', 'H', '5', '5', 'X', ' ', 'M', 'S', 'D', ' ', ' ',
    
    //File system type label
    'F', 'A', 'T', '1', '6', 0x20,0x20, 0x20,
 
};


//1 cluster = 1 sectors = 0.5KB
//emulated file allocation table
//little-endian
//Unused (0x0000) Cluster in use by a file(Next cluster) Bad cluster (0xFFF7) Last cluster in a file (0xFFF8-0xFFFF)。
//__code const uint8_t FAT_data[]={   //first 2 sector reserved
//    0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
//};


extern __code File_Entry filesOnDrive[];    //refer to main file
extern __code uint8_t filesOnDriveCount;    //refer to main file


//Root directory
__code const uint8_t RootDir[32]={
    //label, match DBR
    'C', 'H', '5', '5', 'X', ' ', 'M', 'S', 'D', ' ', ' ',
    0x08,                  //文件属性，表示磁盘标卷
    0x00,                  //保留
    0x00,                  //创建时间毫秒时间戳, Byte 13
    
    //文件创建时间，15点27分35秒
    TIME_LB(15,27,35), TIME_HB(15,27,35),
    
    //文件创建日期，2008年8月19日
    DATE_LB(2008,8,19), DATE_HB(2008,8,19),
    
    //最后访问日期，2008年8月20日
    DATE_LB(2008,8,20), DATE_HB(2008,8,20),
    
    0x00, 0x00,            //起始簇号高位字节，FAT12/16必须为0
    
    //最后修改时间，15点36分47秒
    TIME_LB(15,36,47), TIME_HB(15,36,47),
    
    //最后修改日期，2008年8月19日
    DATE_LB(2008,8,19), DATE_HB(2008,8,19),
    
    0x00, 0x00,            //起始簇低字
    0x00, 0x00, 0x00, 0x00,   //文件长度
};

uint8_t LUN_GetStatus () {
    return emuDisk_Status;
}

void LUN_Read_func_DBR(uint16_t DBR_data_index){    //separate funcs relieve the register usage
    for (uint8_t i=0;i<BULK_MAX_PACKET_SIZE;i++){
        if (DBR_data_index<62){
            BOT_Tx_Buf[i] = DBR_data[DBR_data_index];
        }else if (DBR_data_index==510){
            BOT_Tx_Buf[i] = 0x55;
        }else if (DBR_data_index==511){
            BOT_Tx_Buf[i] = 0xAA;
        }else{
            BOT_Tx_Buf[i] = 0x90;   //nop for boot code
        }
        DBR_data_index++;
    }
}

void LUN_Read_func_FAT(uint16_t FAT_data_index){    //separate funcs relieve the register usage
    for (uint8_t i=0;i<BULK_MAX_PACKET_SIZE;i++){
        uint8_t fileIndex = ((FAT_data_index-4)/2)/64;  //64 sector reserved per file
        uint8_t fileOffset = ((FAT_data_index-4)/2)%64;
        uint8_t sendVal=0;
        uint8_t clusterUsage = (filesOnDrive[fileIndex].filesize+511)/512;
        
        if (fileIndex<filesOnDriveCount){
            if (fileOffset<clusterUsage){
                uint16_t fatEntry = 0xFFF8; //end of file
                if ((fileOffset+1)<clusterUsage){
                    fatEntry = 64*fileIndex + fileOffset + 3;   //make FAT link to next cluster.
                }
                if (FAT_data_index&1){
                    sendVal = fatEntry>>8;
                }else{
                    sendVal = fatEntry;
                }
            }
        }
        BOT_Tx_Buf[i] = sendVal;
        FAT_data_index++;
    }
}

void LUN_Read_func_Root_DIR(uint16_t rootAddrIndex){    //separate funcs relieve the register usage
    for (uint8_t i=0;i<BULK_MAX_PACKET_SIZE;i++){
        uint8_t sendVal=0;
        if (rootAddrIndex<32){  // RootDir entry
            sendVal = RootDir[rootAddrIndex];
        }else if (rootAddrIndex< (32*(1+filesOnDriveCount)) ){
            uint8_t fileIndex = (rootAddrIndex/32)-1;
            uint8_t offsetIndex = rootAddrIndex%32;
            if (offsetIndex<11){
                sendVal = filesOnDrive[fileIndex].filename[offsetIndex];
            }else if (offsetIndex==11){ //File Attributes
                sendVal = 0x01; //Read Only.
            }else if (offsetIndex==12){ //Reserved
                sendVal = 0x00;
            }else if (offsetIndex==13){ //Create time, fine resolution: 10 ms units
                sendVal = 0x00;
            }else if (offsetIndex<16){ //Create time
                sendVal = filesOnDrive[fileIndex].filetime[offsetIndex-14];
            }else if (offsetIndex<18){ //Create date
                sendVal = filesOnDrive[fileIndex].filedate[offsetIndex-16];
            }else if (offsetIndex<20){ //Last access date
                sendVal = filesOnDrive[fileIndex].filedate[offsetIndex-18];
            }else if (offsetIndex<22){ //High two bytes of first cluster number, keep 0 for FAT12/16
                sendVal = 0;
            }else if (offsetIndex<24){ //Last modified time
                sendVal = filesOnDrive[fileIndex].filetime[offsetIndex-22];
            }else if (offsetIndex<26){ //Last modified date
                sendVal = filesOnDrive[fileIndex].filedate[offsetIndex-24];
            }else if (offsetIndex<28){ //Start of file in clusters in FAT12 and FAT16.
                uint16_t startClusters = 2 + fileIndex*64;  //make file 64 clusters, 32K apart. Should be enough.
                if (offsetIndex == 26){
                    sendVal = startClusters & 0xFF;
                }else{
                    sendVal = (startClusters>>8) & 0xFF;
                }
            }else if (offsetIndex<30){ //Low 2 byte of file size
                uint16_t filesize = filesOnDrive[fileIndex].filesize;
                if (offsetIndex == 28){
                    sendVal = filesize & 0xFF;
                }else{
                    sendVal = (filesize>>8) & 0xFF;
                }
            }else{
            }
        }else{
        }
        BOT_Tx_Buf[i] = sendVal;
        rootAddrIndex++;
    }
}

void LUN_Read_func_Files(uint16_t file_data_index){    //separate funcs relieve the register usage
    uint8_t fileIndex = file_data_index/32768;
    uint16_t fileOffset = file_data_index%32768;
    uint16_t fileSize = filesOnDrive[fileIndex].filesize;
    if (fileIndex<filesOnDriveCount && fileOffset<fileSize){
        //check file response type
        uint8_t responseType = filesOnDrive[fileIndex].fileCallBackType;
        if (responseType == CONST_DATA_FILE){
            __code const uint8_t *dateArray = filesOnDrive[fileIndex].filePtr.constPtr;
            for (uint8_t i=0;i<BULK_MAX_PACKET_SIZE;i++){
                BOT_Tx_Buf[i] = (fileOffset<fileSize)?dateArray[fileOffset]:0;
                fileOffset++;
            }
        }else{
            pFileCBFn funcPtr = filesOnDrive[fileIndex].filePtr.funcPtr;
            funcPtr(fileOffset);
        }
    }else{
        for (uint8_t i=0;i<BULK_MAX_PACKET_SIZE;i++){
            BOT_Tx_Buf[i] = 0x0;
        }
    }
}

// Read BULK_MAX_PACKET_SIZE bytes of data from device to BOT_Tx_Buf
void LUN_Read (uint32_t curAddr) {
    if ( curAddr<512 ){  //Boot Sector
        LUN_Read_func_DBR(curAddr);
    }else if ( curAddr<(512+512*64L) ){  //0x200 0x4200, Each FAT is 32 sectors
        if (curAddr>=512*33L){  //FAT2
            LUN_Read_func_FAT(curAddr-512*33L);
        }else{                  //FAT1
            LUN_Read_func_FAT(curAddr-512);
        }
    }else if ( (curAddr<512*97L) ){ //0x8200    Root directory, 512 items, 32 bytes each
        LUN_Read_func_Root_DIR(curAddr - 512*65L);
    }else {  //0xC200 1st usable cluster, with index 2.
        LUN_Read_func_Files(curAddr - 512*97L);
    }
}

// Write BULK_MAX_PACKET_SIZE bytes of data from BOT_Rx_Buf to device
void LUN_Write (uint32_t curAddr) {
    //EEPROM_Write(BOT_Rx_Buf, U32B1(curAddr), U32B0(curAddr), BULK_MAX_PACKET_SIZE);
}


void LUN_Eject () {
    emuDisk_Status = MAL_FAIL;
}
