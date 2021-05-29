#include "USBconstant.h"


//Device descriptor
__code uint8_t DevDesc[] = {
    0x12,       //bLength size of this descriptor
    0x01,       //bDescriptorType device descriptor type
    0x10,0x01,  //bcdUSB USB spec release number in BCD format, USB1.1 (0x10, 0x01).
    0x00,       //bDeviceClass
    0x00,       //bDeviceSubClass
    0x00,       //bDeviceProtocol 
    DEFAULT_ENDP0_SIZE, //bMaxPacketSize0 Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid)
    0x09,0x12,  //VID
    0x5D,0xC5,  //PID 
    0x00,0x01,  //bcdDevice
    0x01,       //iManufacturer Index of String descriptor describing manufacturer
    0x02,       //iProduct Index of string descriptor describing product
    0x03,       //iSerialNumber Index of String descriptor describing the device’s serial number
    0x01        //bNumConfigurations
};

__code uint16_t DevDescLen = sizeof(DevDesc);

__code uint8_t CfgDesc[] ={
    0x09,       //bLength Size of this descriptor in bytes
    0x02,       //bDescriptorType
    sizeof(CfgDesc) & 0xff,sizeof(CfgDesc) >> 8,    //wTotalLength
    0x01,       //bNumInterfaces Number of interfaces supported by this configuration
    0x01,       //bConfigurationValue Value to use as an argument to Set Configuration to select this configuration
    0x00,       //iConfiguration Index of string descriptor describing this configuration. In this case there is none.
    0x80,       //bmAttributes Configuration characteristics(bit7:Bus Powered)
    0x05,       //MaxPower Maximum current consumption in 2mA/LSB(5 means 10mA)
    // Interface 1 (HID) descriptor
    0x09,       //bLength Size of this descriptor in bytes
    0x04,       //bDescriptorType Interface descriptor
    0x00,       //bInterfaceNumber Zero-based value identifying the index in the array of concurrent interfaces supported by this configuration
    0x00,       //bAlternateSetting
    0x01,       //bNumEndpoints Number of endpoints used by this interface (excluding endpoint zero)
    0x03,       //bInterfaceClass Class code (HID code assigned by USB)
    0x01,       //bInterfaceSubClass
    0x02,       //bInterfaceProtocol(Mouse)
    0x00,       //iInterface Index of string descriptor describing this interface
    // HID Descriptor
    0x09,       //bLength Size of this descriptor in bytes.
    0x21,       //bDescriptorType HID descriptor type
    0x10,0x01,  //HID Class Specification release number
    0x00,       //bCountryCode Hardware target country
    0x01,       //bNumDescriptors Number of HID class descriptors to follow
    0x22,       //bDescriptorType Reportdescriptor type
    sizeof(ReportDesc) & 0xff,sizeof(ReportDesc) >> 8,    //wDescriptorLength Total length of Reportdescriptor
    // Endpoint Descriptor for EP1
    0x07,       //ValuebLength Size of this descriptor in bytes
    0x05,       //bDescriptorType Endpoint descriptor type
    0x81,       //bEndpointAddress The address of the endpoint on the USB device described by this descriptor(EP1:IN)
    0x03,       //bmAttributes Interrupt
    0x04,0x00,  //ValuewMaxPacketSize Maximum packet size this endpoint is capable of sending or receiving
    0x0A,       //bInterval Interval for polling endpoint for data transfers. Expressed in milliseconds.
};

__code uint16_t ReportDescLen = sizeof(ReportDesc);

__code uint8_t ReportDesc[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x38,                    //     USAGE (Wheel)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};

__code uint16_t CfgDescLen = sizeof(CfgDesc);

//String Descriptors
__code uint8_t LangDes[]={0x04,0x03,0x09,0x04};           //Language Descriptor
__code uint16_t LangDesLen = sizeof(LangDes);
__code uint8_t SerDes[]={                                 //Serial String Descriptor
    0x18,0x03,
    'C',0x00,'H',0x00,'5',0x00,'5',0x00,'x',0x00,' ',0x00,'m',0x00,'o',0x00,'u',0x00,'s',0x00,'e',0x00
};
__code uint16_t SerDesLen = sizeof(SerDes);
__code uint8_t Prod_Des[]={                                //Produce String Descriptor
    0x16,0x03,
    'C',0x00,'H',0x00,'5',0x00,'5',0x00,'x',0x00,'d',0x00,
    'u',0x00,'i',0x00,'n',0x00,'o',0x00
};
__code uint16_t Prod_DesLen = sizeof(Prod_Des);

__code uint8_t Manuf_Des[]={
    0x0A,0x03,
    'b',0x00,'e',0x00,'t',0x00,'a',0x00
};
__code uint16_t Manuf_DesLen = sizeof(Manuf_Des);
