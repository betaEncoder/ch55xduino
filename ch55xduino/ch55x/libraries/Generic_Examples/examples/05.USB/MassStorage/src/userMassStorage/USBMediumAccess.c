#include "USBMassStorage.h"
#include "USBMediumAccess.h"

/* External variables --------------------------------------------------------*/
// Defined in USBMassStorage.c
extern __xdata Bulk_Only_CBW CBW;

//FAT info: http://www.c-jump.com/CIS24/Slides/FAT/lecture.html

__code const uint8_t diskCapacity[8]=
{
    0x00,0x00,0x4F,0xFF, //能够访问的最大逻辑块地址
    0x00,0x00,0x02,0x00  //块的长度
    //所以该磁盘的容量为
    //(0x4FFF+1)*0x200 = 0xA00000 = 10*1024*1024 = 10MB.
};

__code const uint8_t formatCapacity[8]=
{
    0x00,0x00,0x50,0x00, //能够访问的最大逻辑块地址
    0x02,0x00,0x02,0x00  //块的长度, // 02:Descriptor Code: Formatted Media
    //所以该磁盘的容量为
    //(0x4FFF+1)*0x200 = 0xA00000 = 10*1024*1024 = 10MB.
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
    0x08,                      //Sectors/cluster (8)
    0x01, 0x00,                //Size of reserved area (1 sector)
    0x02,                      //Number of FATs (2)
    0x00, 0x02,                //Max. number of root directory entries (0x0200 = 512)
    0x00, 0x00,                //Total number of sectors (0, not used)
    0xF8,                      //Media type (hard disk)
    0x20, 0x00,                //FAT size (0x0020 = 32 sectors)
    0x20, 0x00,                //Sectors/track (0x0020 = 32)
    0x40, 0x00,                //Number of heads (0x0040 = 64)
    0x00, 0x00, 0x00, 0x00,    //Number of sector before partition (0)
    0x00, 0x50, 0x00, 0x00,    //Total number of sectors，10M is 0x5000, 2560 clusters, FAT12 is less than 4087 clusters
    0x80,                      //Drive number (hard disk)
    0x00,                      //Reserved
    0x29,                      //Extended boot signature
    0x00, 0x00, 0x00, 0x00,    //Volume serial number
    
    //Volume label
    'C', 'H', '5', '5', 'X', ' ', 'M', 'S', 'D', ' ', ' ',
    
    //File system type label
    'F', 'A', 'T', '1', '6', 0x20,0x20, 0x20,
 
};


//1 cluster = 32 sectors = 16KB
//模拟的文件分配表
//其中项0为0xFF8，项1为0xFFF，表示已经使用。
//项2为0xFFF，表示文件结束。其余项为0，表示未使用
//yz Zx XY, FAT12 xyz is the one pointer entry and XYZ is the second pointer entry.
__code const uint8_t FAT_data[64]={
    0xF0, 0xFF, 0xFF,
    0xFF, 0x4F, 0x00,
    0x05, 0x60, 0x00,
    0xFF, 0x0F, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x00 ,0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x00 ,0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
    0x00, 0x00 ,0x00, 0x00,  0x00, 0x00, 0x00, 0x00
};


//测试文件的数据
__code const uint8_t ReadmeFileData[]=
{
    "This is a test file on speed demon.\n\r"
};


/********** 时间格式（16Bits）为： **************
 Bits15~11表示小时，可以取值为0~23；
 Bits10~5表示分，可以取值为0~59；
 Bits4~0表示秒，可以取值为0~29，每单位为2秒，即实际秒值为该值的2倍。
 */

/********* 日期格式（16Bits）为：  *************
 Bits15~9表示年份，可以取值为0~127，它表示距离1980年差值，
 即实际的年份为该值加上1980，最大可表示到2107年；
 
 Bits8~5表示月份，可以取值为1~12；
 Bits4~0表示号数，可以取值为1~31。
 */

//求出16位时间格式的高字节
#define TIME_HB(H,M,S) (((((H)<<3))|((M)>>3)))
//求出16位时间格式的低字节
#define TIME_LB(H,M,S) (((0))|((M)<<5)|(S))

//求出16位日期格式的高字节
#define DATE_HB(Y,M,D) (((((Y)-1980)<<1)|((M)>>3)))
//求出16位日期格式的低字节
#define DATE_LB(Y,M,D) ((0)|((M)<<5)|(D))

#define FILE_LEN 65536L
#define FILE_CLUSTER_LIMIT (((FILE_LEN+4095)/4096)+3)


//根目录
__code const uint8_t RootDir[96]={
    //label, match DBR
    'S', 'p', 'e', 'e', 'd', ' ', 'D', 'e', 'm', 'o', 'n',
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
    
    //根目录下的测试文件
    //文件名“TEST.TXT”
    'R',  'E',   'A',  'D', 'M', 'E', ' ', ' ',  'T', 'X', 'T',
    0x01,                  //文件属性，表示只读文件
    0x00,                  //保留
    0x00,                  //创建时间毫秒时间戳
    //文件创建时间，15点48分26秒
    TIME_LB(15,48,26), TIME_HB(15,48,26),
    
    //文件创建日期,2008年8月19日
    DATE_LB(2008,8,19), DATE_HB(2008,8,19),
    
    //最后访问日期
    DATE_LB(2008,8,20), DATE_HB(2008,8,20),
    
    0x00, 0x00,            //起始簇号高位字节，FAT12/16必须为0
    
    //最后修改时间,15点50分33秒
    TIME_LB(15,50,33), TIME_HB(15,50,33),
    
    //最后修改日期，2008年8月19日
    DATE_LB(2008,8,19), DATE_HB(2008,8,19),
    
    0x02, 0x00,            //起始簇低字，簇2。  The first cluster has an address of 2. I.e., there is no addressable cluster 0 or 1
    
    //文件长度
    (sizeof(ReadmeFileData)-1),((sizeof(ReadmeFileData)-1)>>8), 0x00, 0x00,
    
    //根目录下的测试文件
    //文件名“TEST.TXT”
    'T',  'E',   'S',  'T', ' ', ' ', ' ', ' ',  'T', 'X', 'T',
    0x01,                  //文件属性，表示只读文件
    0x00,                  //保留
    0x00,                  //创建时间毫秒时间戳
    //文件创建时间，15点48分26秒
    TIME_LB(15,48,26), TIME_HB(15,48,26),
    
    //文件创建日期,2008年8月19日
    DATE_LB(2008,8,19), DATE_HB(2008,8,19),
    
    //最后访问日期
    DATE_LB(2008,8,20), DATE_HB(2008,8,20),
    
    0x00, 0x00,            //起始簇号高位字节，FAT12/16必须为0
    
    //最后修改时间,15点50分33秒
    TIME_LB(15,50,33), TIME_HB(15,50,33),
    
    //最后修改日期，2008年8月19日
    DATE_LB(2008,8,19), DATE_HB(2008,8,19),
    
    0x03, 0x00,            //起始簇低字，簇3。  The first cluster has an address of 2. I.e., there is no addressable cluster 0 or 1
    
    //文件长度
//    (sizeof(TestFileData)-1),((sizeof(TestFileData)-1)>>8), 0x00, 0x00,
    ((FILE_LEN)&0xFF),((FILE_LEN>>8)&0xFF), ((FILE_LEN>>16)&0xFF), ((FILE_LEN>>24)&0xFF),   //511 sector*4KB. Each byte in flash takes 4 bytes to display

};

uint8_t LUN_GetStatus () {
    return emuDisk_Status;
}

// Read BULK_MAX_PACKET_SIZE bytes of data from device to BOT_Tx_Buf
void LUN_Read (uint32_t curAddr) {
    if ( (curAddr>=0 && curAddr<512)){  //Boot Sector
        uint16_t DBR_data_index = curAddr;
        __code uint8_t* DBR_data_ptr=&DBR_data[DBR_data_index];
        for (uint8_t i=0;i<BULK_MAX_PACKET_SIZE;i++){
            if (DBR_data_index<62){
                BOT_Tx_Buf[i] = *DBR_data_ptr++;
            }else if (DBR_data_index==510){
                BOT_Tx_Buf[i] = 0x55;
            }else if (DBR_data_index==511){
                BOT_Tx_Buf[i] = 0xAA;
            }else{
                BOT_Tx_Buf[i] = 0x90;   //nop for boot code
            }
            DBR_data_index++;
        }
    }else{
        for (uint8_t i=0;i<BULK_MAX_PACKET_SIZE;i++){
            BOT_Tx_Buf[i] = 0;
        }
    }
    
    
}

// Write BULK_MAX_PACKET_SIZE bytes of data from BOT_Rx_Buf to device
void LUN_Write (uint32_t curAddr) {
    uint8_t lun=CBW.bLUN;
    if (lun == 0) {
        //EEPROM_Write(BOT_Rx_Buf, U32B1(curAddr), U32B0(curAddr), BULK_MAX_PACKET_SIZE);
    }
}


void LUN_Eject () {
    emuDisk_Status = MAL_FAIL;
}
