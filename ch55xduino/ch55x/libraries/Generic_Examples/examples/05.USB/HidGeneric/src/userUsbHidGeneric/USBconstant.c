#include "USBconstant.h"


//Device descriptor
__code uint8_t DevDesc[] = {
    0x12,0x01,
    0x10,0x01,  //USB spec release number in BCD format, USB1.1 (0x10, 0x01).
    0x00,0x00,0x00, //bDeviceClass, bDeviceSubClass, bDeviceProtocol 
    DEFAULT_ENDP0_SIZE, //bNumConfigurations
    0x09,0x12,0x5D,0xC5, // VID PID 
    0x00,0x01,  //version
    0x01,0x02,0x03, //bString
    0x01    //bNumConfigurations
};

__code uint16_t DevDescLen = sizeof(DevDesc);

__code uint8_t CfgDesc[] ={
    0x09,0x02,sizeof(CfgDesc) & 0xff,sizeof(CfgDesc) >> 8,
    0x01,0x01,0x00,0x80,0x64,             //Configuration descriptor (1 interface)
    // Interface 1 (HID) descriptor
    0x09,0x04,0x00,0x00,0x02,0x03,0x01,0x00,0x00,    // HID Keyboard, 2 endpoints
    0x09,0x21,0x10,0x01,0x21,0x01,0x22,sizeof(ReportDesc) & 0xff,sizeof(ReportDesc) >> 8,    //HID Descriptor
    0x07,0x05,0x01,0x03,0x40,0x00,0x01,                       //endpoint descriptor
    0x07,0x05,0x81,0x03,0x40,0x00,0x01,                       //endpoint descriptor
};

__code uint16_t ReportDescLen = sizeof(ReportDesc);

__code uint8_t ReportDesc[] ={
	0x06, 0x00, 0xFF,		// Usage Page = 0xFF00 (Vendor Defined Page 1)
	0x09, 0x01,			// Usage (Vendor Usage 1)
	0xA1, 0x01,			// Collection (Application)
	0x19, 0x01,			//   Usage Minimum 
	0x29, 0x40,			//   Usage Maximum 64 input usages total (0x01 to 0x40)
	0x15, 0x01,			//   Logical Minimum (data bytes in the report may have minimum value = 0x00)
	0x25, 0x40,			//   Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)
	0x75, 0x08,			//   Report Size: 8-bit field size
	0x95, 0x40,			//   Report Count: Make eight 8-bit fields (the next time the parser hits an "Input", "Output", or "Feature" item)
	0x81, 0x00,			//   Input (Data, Array, Abs): Instantiates input packet fields based on the above report size, count, logical min/max, and usage.
	0x19, 0x01,			//   Usage Minimum 
	0x29, 0x40,			//   Usage Maximum 64 output usages total (0x01 to 0x40)
	0x91, 0x00,			//   Output (Data, Array, Abs): Instantiates output packet fields.  Uses same report size and count as "Input" fields, since nothing new/different was specified to the parser since the "Input" item.
	0xC0
};

__code uint16_t CfgDescLen = sizeof(CfgDesc);

//String Descriptors
__code uint8_t LangDes[]={0x04,0x03,0x09,0x04};           //Language Descriptor
__code uint16_t LangDesLen = sizeof(LangDes);
__code uint8_t SerDes[]={                                 //Serial String Descriptor
    0x14,0x03,
    'C',0x00,'H',0x00,'5',0x00,'5',0x00,'x',0x00,' ',0x00,'H',0x00,'I',0x00,'D',0x00
};
__code uint16_t SerDesLen = sizeof(SerDes);
__code uint8_t Prod_Des[]={                                //Produce String Descriptor
    0x16,0x03,
    'C',0x00,'H',0x00,'5',0x00,'5',0x00,'x',0x00,'d',0x00,
    'u',0x00,'i',0x00,'n',0x00,'o',0x00
};
__code uint16_t Prod_DesLen = sizeof(Prod_Des);

__code uint8_t Manuf_Des[]={
    0x0E,0x03,
    'b',0x00,'e',0x00,'t',0x00,'a',0x00
};
__code uint16_t Manuf_DesLen = sizeof(Manuf_Des);
